> 本文更新流程基于类组件展开

### 更新流程

React 每产生一个更新，就会把更新结构化的记录在 fiber 上，然后调度执行更新任务，流程如下：

```typescript
产生更新
    |
    v
调度前准备（`markUpdateLaneFromFiberToRoot`）
    |
    v
调度更新任务（`ensureRootIsScheduled`）
    |
    v
// ============================分割线以上为本文主要内容=============================
// render-commit 阶段可能会被更高优的更新任务中断，转而进入新的 render-commit 阶段
render 阶段（`performSyncWorkOnRoot` 或 `performConcurrentWorkOnRoot`）
    |
    v
commit 阶段（`commitRoot`）
```

#### 产生更新

在 React 中，有以下方法能触发状态更新：

- `ReactDom.render()`
- 类组件中的`this.setState()`、`this.forceUpdate()`
- 函数组件中的`useState()`、`useReducer()`

> 不同类型组件工作方式不同，对应的`update`结构也不同，但他们的工作机制是基本相同的，本文的更新流程以类组件为例，函数组件的流程可以在《[hook 原理](./react-hook-mini.md)》了解 

当类组件调用`setState()`时，意味着对应的 fiber 节点产生了一个更新，实际上是调用`enqueueSetState()`完成创建 Update 对象（下称`update`）到发起调度的过程

1. 获取产生更新的 fiber 节点
2. 计算更新优先级（来源于合成事件的优先级）
3. 创建`update`并放入 fiber 的`updateQueue`
4. 进入调度流程`scheduleUpdateOnFiber()`

```typescript
Component.prototype.setState = function(partialState, callback) {
  this.updater.enqueueSetState(this, partialState, callback, 'setState');
};

// enqueueSetState 主要逻辑
enqueueSetState(inst, payload, callback) {
  //【步骤1】获取当前触发更新的 fiber 节点。inst 是组件实例
  const fiber = getInstance(inst);
  // eventTime 是当前触发更新的时间戳
  const eventTime = requestEventTime();
  const suspenseConfig = requestCurrentSuspenseConfig();

  //【步骤2】获取本次 update 的优先级
  const lane = requestUpdateLane(fiber, suspenseConfig);

  //【步骤3】创建 update 对象
  const update = createUpdate(eventTime, lane, suspenseConfig);
  // payload 就是 setState 的第一个参数，可以是一个对象或者函数。
  update.payload = payload;
  // 将 update 放入 fiber 的 updateQueue
  enqueueUpdate(fiber, update);

  //【步骤4】进入更新调度流程
  scheduleUpdateOnFiber(fiber, lane, eventTime);
}
```

这里重点讲下【步骤3】产生的`update`是个怎样的结构，以及他是怎样加到`fiber.updateQueue`上的

```typescript
// 类组件的 update 结构
const update = {
  // 任务时间
  eventTime,
  // 更新优先级
  lane,
  // Suspense 相关
  suspenseConfig,
  // 更新的类型，包括 UpdateState | ReplaceState | ForceUpdate | CaptureUpdate
  tag: UpdateState,
  // 更新携带的状态，this.setState 的第一个参数
  payload: null,
  // 更新回调，this.setState 的第二个参数
  callback: null,

  // 指向下一个 update
  next: null,
};
```

一个类组件可以通过多次调用`setState()`来产生多个`update`，这些`update`按调用顺序连接形成链表结构记录在对应 fiber 的`updateQueue`属性中

```typescript
// 类组件的 updateQueue 结构
const updateQueue: UpdateQueue<State> = {
  // 本次更新任务的初始 state
  baseState: fiber.memoizedState,
  // 上次更新任务遗留的 update（因为优先级不够高在上次更新被跳过），形成单向链表
  // firstBaseUpdate 为表头，lastBaseUpdate 为表尾部，称这条链表为 baseUpdate
  firstBaseUpdate: null,
  lastBaseUpdate: null,
  // 上次更新任务以来产生的 update，形成环形单向链表，shared.pending 指向环形表尾
  shared: {
    pending: null,
  },
  // 带更新回调的 update 集
  effects: null,
};
```

`fiber.updateQueue`上存在两条`update`链表：

- 一条是由`firstBaseUpdate->lastBaseUpdate`组成的单向链表（称之为`baseUpdate`），他代表了上次更新任务因优先级不够高被遗留的`update`，需要等待再次被调度

- 另一条是由`shared.pending`表示的环形链表，他代表了上次更新任务以来产生的`update`，`setState()`产生的`update`就是加入到这条环形链表上

  ```typescript
  // 环形链表图示，shared.pending 始终指向表尾
  // 一开始链表为空
  shared.pending = null;
  
  // 创建并插入第一个更新u0，链表里只有u0，表头表尾都是u0
  shared.pending = u0 ---> u0
                    ^       |
                    |       |
                    ---------
  
  // 创建并从表尾插入第二个更新u1
  shared.pending = u1 ---> u0   
                    ^       |
                    |       |
                    ---------
  ```
  

两条`update`链一起组成了本次更新任务需要处理`update`，在更新任务的 render 阶段，两条链表将合并在一起，选取高优`update`，跳过低优`update`，并结合初始 state 计算出本次更新的 state ，供 render 阶段构建`workInProgressFiber`使用。具体`update`处理计算状态的逻辑，可以在【render-beginWork 处理更新】小节中了解

#### 调度前准备

在创建好`update`并加入到`fiber.updateQueue`后，将执行`scheduleUpdateOnFiber()`进入调度流程，通过区分`update.lane`将同步更新和异步更新分流，分别进入各自的流程

但在真正调度更新之前还需完成一些重要的调度准备工作：

1. 检查是否存在嵌套更新，如在 render 函数中 setState

2. 从产生更新的 fiber 开始向上遍历，将`fiber.lanes`一直向上收集，收集到父级节点的`childLanes`中

   - `fiber.childLanes`是识别 fiber 子树是否需要更新的关键，是在 render-beginWork 阶段复用子 fiber 的判断标准

   ```typescript
   let parent = sourceFiber.return;
   while (parent !== null) {
     parent.childLanes = mergeLanes(parent.childLanes, lane);
     parent = parent.return;
   }
   ```

3. 将`update.lane`收集到`rootFiber.pendingLanes`上，确保能取到正确的`renderLanes`（即渲染优先级，取自`rootFiber.pendingLanes`中最优先的部分）进行更新任务的调度

【PS】取到的`renderLanes`不一定包含当前`update.lane`，因为在他之前可能存在过期`update`或更高优的`update`

```typescript
export function scheduleUpdateOnFiber(
  fiber: Fiber,
  lane: Lane,
  eventTime: number,
) {
  // ===========================调度准备===========================
  //【步骤1】检查是否有无限更新
  checkForNestedUpdates();
  //【步骤2】向上收集 fiber.childLanes
  const root = markUpdateLaneFromFiberToRoot(fiber, lane);
  //【步骤3】在 root 上标记更新，将 update.lane 收集到 root.pendingLanes
  markRootUpdated(root, lane, eventTime);

  // ===========================开始调度===========================
  // 根据 Scheduler 的优先级获取到更新任务的优先级（计算自 renderLanes）
  const priorityLevel = getCurrentPriorityLevel();

  if (lane === SyncLane) {
    // 本次更新是同步的
    if (
      (executionContext & LegacyUnbatchedContext) !== NoContext &&
      (executionContext & (RenderContext | CommitContext)) === NoContext
    ) {
      //【情况1】如果是本次更新是同步的，而且当前没有更新任务正在进行，主线程是空闲的，
      // 直接调用 performSyncWorkOnRoot 开始同步执行本次更新任务
      performSyncWorkOnRoot(root);
    } else {
      //【情况2】如果是本次更新是同步的，但当前有更新任务正在进行，
      // 调用 ensureRootIsScheduled 对本次更新任务进行调度，
      // 目的是去复用已经在更新的任务，让这个已有的任务把这次更新顺便做了
      ensureRootIsScheduled(root, eventTime);
    }
  } else {
    //【情况3】如果本次更新是异步的，调用 ensureRootIsScheduled 对本次更新任务进行调度
    ensureRootIsScheduled(root, eventTime);
    schedulePendingInteractions(root, lane);
  }
}
```

#### 了解更新任务

> **一次完整的 render 阶段就是一个更新任务**

更新任务（下简称任务）可以看作 render 阶段入口函数的执行，即`performSyncWorkOnRoot()`或`performConcurrentWorkOnRoot()`

每产生一个`update`，都会产生一个新任务，React 会根据最新的`renderLanes`计算出新任务的优先级，并与当前进行中的任务的优先级比较：

- 若新任务优先级与当前任务优先级一致，无需调度，当前任务会顺便完成新任务的活
- 若新任务优先级与当前任务优先级不一致，说明新任务优先级更高，需要调度

当新任务的优先级比当前任务优先级高时，React 会借助`Scheduler`重新调度一个高优任务替代当前任务：

1. 取消当前任务

2. 将新任务连被包装成一个任务对象加入到任务队列中（任务队列是一个小顶堆，按照`sortIndex`排序）

   ```typescript
   var newTask = {
       id: taskIdCounter++,
       // 任务函数回调
       callback,
       // 任务调度优先级（计算自任务优先级，和任务优先级不是用一个东西）
       priorityLevel,
       // 任务开始执行的时间点
       startTime,
       // 任务的过期时间
       expirationTime,
       // sortIndex 是任务在小顶堆中排序的依据，综合调度优先级和其他因素得来
       sortIndex: -1,
     };
   ```

3. `Scheduler`返回任务队列中的`sortIndex`最小的任务对象，对应最高优的任务

完成调度生成一个最高优的任务后，其任务对象就会被挂载到`rootFiber.callbackNode`上，对应的任务优先级也会被挂载到`rootFiber.callbackPriority`，代表当前进行中的任务，供后面的新任务进行优先级比较，决定是否开始新一轮的任务调度

#### 调度更新任务

在产生`update`并完成调度前准备工作后，我们可以说产生了一个更新任务，接下来需要判断是否需要发起一次调度，这就是`ensureRootIsScheduled()`做的事：

1. 从`rootFiber.callbackNode/callbackPriority`中取到旧任务及其优先级
2. 检查是否存在过期任务，将过期任务放到`rootFiber.expiredLanes`上，确保下面计算`renderLanes`能优先取到`expiredLanes`，让过期任务能以同步优先级进行调度（立即执行）
3. 计算`renderLanes`，如果`renderLanes`为空，说明**无需调度提前返回**
4. 根据`renderLanes`计算出新任务的优先级`newCallbackPriority`
5. 判断新旧任务的优先级
   - 新旧任务优先级相等，**无需调度，直接复用旧任务**
   - 新旧任务优先级不相等，说明新任务优先级更高，取消旧任务，**发起调度**（见上一小节）



需要注意的是，根据新任务的优先级，会有不同的调度模式：

- 同步优先级：调用`scheduleSyncCallback()`去同步执行任务，过期任务会进入该调度模式
- 同步批量优先级：调用`scheduleCallback()`将任务以`ImmediateSchedulerPriority`调度优先级加入调度
- 其他（concurrentMode 的优先级）：根据任务优先级计算调度优先级，调用`scheduleCallback()`加入调度

在前两种调度模式中，被加入任务队列的新任务对应`performSyncWorkOnRoot()`入口函数，对应同步不可中断的 render 阶段

最后一种调度模式中，被加入任务队列的新任务对应`performConcurrentWorkOnRoot()`入口函数，对应异步可中断的 render 阶段，由`Scheduler`进行`时间切片`

> `ensureRootIsScheduled()`在任务调度层面解决了高优先级任务的插队和任务饥饿问题

#### render-beginWork 处理更新

在更新任务的 render-beginWork 阶段，类组件会调用`processUpdateQueue()`逐个处理当前`fiber.updateQueue`上的`update`来计算新的状态，用以构建`workInProgressFiber`，这个过程分为准备阶段和处理阶段：

【准备阶段】将`updateQueue`上的`shared.pending`切开并接到`baseUpdate`后，并把`shared.pending`置空，此时所有`update`都转移到了`baseUpdate`上

【PS】准备阶段同样会对`currentFiber`的`updateQueue`执行，在`currentFiber`上留下`update`的备份，避免当前更新任务被中断后丢失`update`



【处理阶段】遍历`baseUpdate`，只处理优先级足够的`update`（`update.lane`在`renderLanes`中）计算出新状态赋值给`fiber.memoizedState`；被跳过的低优`update`将成为新的`baseState`（遗留`update`），其`lane`也将合并在一起成为新的`workInProgressFiber.lanes`（遗留优先级）

【PS】为了确保`update`间的依赖关系，第一个被跳过的低优`update`极其后续`update`，无论是否被处理过，都会被保留作为遗留`update`。这意味着这一个`update`可能会被调度多次，相应的`componentWillXXX`生命周期钩子也会被触发多次，这也是为什么这些钩子被标记为`UNSAFE`

___

下面来看一个例子来理解处理阶段的工作，假设当前节点初始状态如下

```typescript
// 其中字母表示 update 的状态，数字表示 update 的优先级，越低优先级越高
baseState: null
baseUpdate: null
shared.pending: A1 -> B1 -> C2 -> D1 -> E2
```

第一次更新任务，`renderLanes`为 1，A1、B1、D1 被处理得到新状态`memoizedState = ABD`

- C2 是第一个被跳过的低优`update`，C2 及后续 `update` 都会被保留下来作为新`baseUpdate`（以确保状态的依赖关系）
- C2 前处理了 A1、B1，新的`baseState = AB`

假设没有高优任务插队，第一次更新任务无中断的完成了，fiber 会变成如下状态，然后开始调度第二次更新任务处理遗留`update`

```typescript
// C2 前处理了 A1、B1，因此新的 baseState 为 AB
baseState: AB
// C2 是第一个被跳过的低优 update，C2 及后续 update 都会被保留下来作为新 baseUpdate（确保状态的依赖关系）
baseUpdate: C2 -> D1 -> E2
// 假设第一次更新任务中没有产生新的 update
shared.pending: null
```

第二次更新任务，`renderLanes`为 2，剩余的 C2、D1、E2 都能被处理得到新状态`memoizedState = ABCDE`，所有`update`被清空

#### render-completeWork 收集未处理 lane

上面提到，`beginWork()`会把当前节点未处理`update`的`lane`合并到`fiber.lanes`上。这里补充另外一个属性`fiber.childLanes`，该属性代表了当前 fiber 子树的整体`lanes`（相当于所有子节点`fiber.lanes`的合并结果）

在更新任务的 render-completeWork 阶段的`completeUnitWork()`中，像`effectList`的逐层向上收集一样，`lanes`也会逐层向上收集，表现为把当前节点的子兄节点的`lanes`和`childLanes`收集到当前节点的`childLanes`上

```typescript
let newChildLanes = NoLanes;

// 遍历当前节点的子兄节点，收集其 lanes 和 childLanes
let child = completedWork.child;
while (child !== null) {
  // 收集过程
  newChildLanes = mergeLanes(
    newChildLanes,
    mergeLanes(child.lanes, child.childLanes),
  );
  child = child.sibling;
}
// 将收集到的 lanes 放到该当前节点的 childLanes 上
completedWork.childLanes = newChildLanes;
```

在完成 render 阶段的整个遍历后，所有的`lanes`就会收集到跟节点的`root.childLanes`，在之后的 commit 阶段中连同`root.lanes`合并到`root.pendingLanes`上（不在 render 阶段操作是因为 render 阶段的`renderLanes`依赖`root.pendingLanes`），然后调用`ensureRootIsScheduled()`重新发起一次调度来处理遗留的低优`update`

### 参考

[React 技术揭秘](https://react.iamkasong.com/state/prepare.html)

[React 中高优先级任务插队机制](https://zhuanlan.zhihu.com/p/348313033)
