### 状态更新 

本文来补全从【触发状态更新】到【render 阶段】路径

```typescript
触发状态更新（不同场景）
    |
    v
创建Update对象
    |
    v
从fiber到root（`markUpdateLaneFromFiberToRoot`）
    |
    v
调度更新（`ensureRootIsScheduled`）
    |
    v
// ============================分割线以上为本文内容=============================
render 阶段（`performSyncWorkOnRoot` 或 `performConcurrentWorkOnRoot`）
    |
    v
commit 阶段（`commitRoot`）
```

在 React 中，有以下方法能触发状态更新：

- `ReactDom.render()`
- 类组件中的`this.setState()`、`this.forceUpdate()`
- 函数组件中的`useState()`、`useReducer()`

每次状态更新都会创建一个 Update 对象（下称`update`），若干个`update`以链表的形式保存在触发更新的 fiber 节点上

因为 render 阶段是从`rootFiber`开始遍历的，所以某个 fiber 节点触发更新后，还需要向上遍历回到`rootFiber`

最后由`Scheduler`根据更新的优先级，决定以同步还是异步的方式调度本次更新，进入到 render 阶段

```typescript
if (newCallbackPriority === SyncLanePriority) {
  // 任务已经过期，需要同步执行render阶段
  newCallbackNode = scheduleSyncCallback(
    performSyncWorkOnRoot.bind(null, root) // render 阶段入口函数（同步）
  );
} else {
  // 根据任务优先级异步执行render阶段
  var schedulerPriorityLevel = lanePriorityToSchedulerPriority(
    newCallbackPriority
  );
  newCallbackNode = scheduleCallback(
    schedulerPriorityLevel,
    performConcurrentWorkOnRoot.bind(null, root) // render 阶段入口函数（异步）
  );
}
```

#### Update 对象（ClassComponent）

不同类型组件工作方式不同，因此存在两种结构的 Update 对象分别对应类组件和函数组件，但他们的工作机制是基本相同的，这里先介绍类组件的 Update 结构，函数组件的 Update 结构可以在后面的《[hook 原理](./react-hook-mini.md)》了解 

```typescript
const update = {
  // 任务时间
  eventTime,
  // 更新优先级
  lane,
  // Suspense 相关
  suspenseConfig,
  // 更新的类型，包括 UpdateState | ReplaceState | ForceUpdate | CaptureUpdate
  tag: UpdateState,
  // 更新挂载的数据，this.setState 的第一个参数
  payload: null,
  // 更新回调，this.setState 的第二个参数
  callback: null,

  // 与其他 update 连接形成链表结构
  next: null,
};
```

一个类组件可以通过多次`this.setState()`来创建多个`update`，这些`update`将形成链表记录在`fiber.updateQueue`中

不同类型的 fiber 其`updateQueue`属性含义不同，在 render-completeWork 中提到过 HostComponent fiber 的`updateQueue`是一个存放 props 的数组。而本节要介绍的 ClassComponent fiber 的`updateQueue`是这样的结构

```typescript
const updatequeue: UpdateQueue<State> = {
  // 本次更新前的 state
  baseState: fiber.memoizedState,
  // 本次更新前遗留的若干个 update（因为优先级不够高在上次更新被跳过），形成单向链表
  // firstBaseUpdate 为表头，lastBaseUpdate 为表尾部，称这条链表为 baseUpdate
  firstBaseUpdate: null,
  lastBaseUpdate: null,
  // 本次更新产生的若干个 update，形成环形单向链表，shared.pending 指向环形表尾
  shared: {
    pending: null,
  },
  // 有更新回调的 update 组成的数组
  effects: null,
};
```

可以看见对于一个 fiber 来说，可能会存在新老两个部分`update`（老：`baseUpdate`；新：`shared.pending`）。在进入 render 阶段的主流程之前，`shared.pending`上的环形链表会被剪开并接到原来的`baseUpdate`尾部（新`update`接在老`update`上）。

最终`baseUpdate`将承载所有`update`，依次遍历`baseUpdate`上的`update`，跳过低优先级`update`，选取高优先级的`update`计算得到本次更新的 state 赋值给`fiber.memoizedState`，供 render 阶段使用。而被跳过的低优`update`成为新的`baseUpdate`

### 优先级调度

状态更新由用户交互产生，交互存在优先级之分，React 根据交互的优先级给交互产生的更新赋予对应的优先级，最终反映到`update.lane`变量上，下面来通过一个例子来了解 React 是如何通过优先级决定更新的顺序

![优先级如何决定更新的顺序](react-update.assets/update-process.png)

在这个例子中，有两个`update`。我们将“关闭黑夜模式”产生的`update`称为`u1`，输入字母“I”产生的`update`称为`u2`。其中`u1`先触发并进入 render 阶段，其优先级较低，执行时间较长，此时：

```typescript
fiber.updateQueue = {
  baseState: {
    blackTheme: true,
    text: 'H'
  },
  firstBaseUpdate: null,
  lastBaseUpdate: null
  shared: {
    pending: u1
  },
  effects: null
};
```

在`u1`完成 render 阶段前用户通过键盘输入字母“I”，产生了`u2`。`u2`属于受控的用户输入，优先级高于`u1`，于是中断`u1`产生的 render 阶段，并追加到`shared.pending`上，此时

```typescript
fiber.updateQueue.shared.pending === u2 ----> u1
                                     ^        |
                                     |________|
```

其中`u2`优先级高于`u1`，开始进入`u2`产生的 render 阶段，`shared.pending`环形链表会被剪开并拼接在`baseUpdate`后面，得到`baseUpdate: u1->u2`，遍历`baseUpdate`处理高优`u2`

由于`u2`不是`baseUpdate`中的第一个`update`，在其之前的`u1`由于优先级不够被跳过。`update`之间可能有依赖关系，所以**被跳过的`update`及其后面所有`update`会成为下次更新的`baseUpdate`**（即`u1 -> u2`），最终`u2`完成 render-commit 阶段，此时：

```typescript
fiber.updateQueue = {
  baseState: {
    blackTheme: true,
    text: 'HI'
  },
  firstBaseUpdate: u1,
  lastBaseUpdate: u2
  shared: {
    pending: null
  },
  effects: null
};
```

在 commit 阶段结尾会再调度一次更新。在该次更新中会基于`baseState`和`baseUpdate: u1->u2`，开启一次新的 render 阶段，





在上一节末尾中提到，在遍历`baseUpdate`计算 state 时，会选取高优，跳过低优的`update`来计算 state，下面就来了解下优先级是如何决定更新顺序的，来看一个例子（其中字母表示`update`的数据，数字表示`update`的优先级，越低优先级越高）

```typescript
// 当前状态
baseState: null
// 之前因低优遗留的 update
baseUpdate: null
// 本次更新产生的 4 个 update
shared.pending: A1 -> B2 -> C1 -> D2
// ==================第一次 render==================
baseState: null
pickedUpdates: [A1,C1]
    |
    v
memoizedState: AC
    |
    v
render 阶段
```

第一次 render，选取高优先的 A1，C1 两个`update`计算得到`memoizedState = AC`，进入

```typescript
// 因为 B2 被跳过，
baseState: A
// B2 因低优被跳过，B2 极其后续的 update 都会被保留下来作为 baseUpdate（确保状态的依赖关系）
baseUpdate: B2 --> C1 --> D2
// 本次更新是否产生 update 取决于第一次 render 中是否触发了更新
shared.pending: null/updates
```

