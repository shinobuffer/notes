### Render 阶段（Reconciler）

> 异步可中断的，构造`workInProgress Fiber树`的过程

`render阶段`开始于`performSyncWorkOnRoot()`或`performConcurrentWorkOnRoot()`方法的调用。这取决于本次更新是同步更新还是异步更新

在异步更新中，会额外将`shouldYield()`作为循环条件，`shouldYield()`是`Scheduler`提供的方法，该方法为当前的更新任务提供中断判断，在浏览器帧没有空闲时间时或有高优任务插队时阻塞循环，等待时机合适时再继续循环，实现`时间切片`

```typescript
// performSyncWorkOnRoot 会调用该方法
function workLoopSync() {
  while (workInProgress !== null) {
    performUnitOfWork(workInProgress);
  }
}

// performConcurrentWorkOnRoot 会调用该方法
function workLoopConcurrent() {
  while (workInProgress !== null && !shouldYield()) {
    performUnitOfWork(workInProgress);
  }
}
```

`performUnitOfWork()`的本质是从`rootFiber`开始对`workInProgress Fiber树`进行深度优先遍历，**边构造树边遍历**：

- 【递】为第一次访问的`workInProgressFiber`（通过`child/sibling`属性访问）执行`beginWork()`方法，该方法会根据传入的`currentFiber`创建子`workInProgressFiber`，并将新创建`workInProgressFiber`与现有`workInProgressFiber`连接起来，构造`workInProgress Fiber树`
- 【归】为第二次访问的 `workInProgressFiber` （通过`return`属性访问）执行`completeWork()`方法

#### 【递】beginWork

> 根据`current`和`workInProgress`创建子 Fiber 节点，将子 Fiber 节点接到`workInProgress`上并返回该子节点

```typescript
function beginWork(
  // 当前组件上次更新的 Fiber 节点，即 currentFiber/workInProgressFiber.alternate
  current: Fiber | null,
  // 当前组件本次更新的 Fiber 节点，即 workInProgressFiber
  workInProgress: Fiber,
  // 优先级相关
  renderLanes: Lanes,
): Fiber | null {
  // ...省略函数体
}
```

从【双缓存 Fiber 树】一节中，我们知道在`mount`阶段，由于是首次渲染，`current Fiber树`树为空，此时入参`current`为空，因此`beginWork()`通过判断`current`是否为空来区分`mount`阶段和`update`阶段：

- `update`阶段：如果`current`存在，在满足一定条件下可以复用`current`，直接克隆`current.child`作为子 Fiber 节点；如果不能复用就像`mount`阶段一样创建节点
- `mount`阶段：根据`workInProgress.tag`创建不同类型的子 Fiber 节点

```typescript
function beginWork(
  current: Fiber | null,
  workInProgress: Fiber,
  renderLanes: Lanes
): Fiber | null {

  // update时：如果current存在可能存在优化路径，可以复用current（即上一次更新的Fiber节点）
  if (current !== null) {
    // ...省略
    // 复用current
    return bailoutOnAlreadyFinishedWork(
      current,
      workInProgress,
      renderLanes,
    );
  } else {
    didReceiveUpdate = false;
  }

  // mount时/update时无法复用：根据tag不同，创建不同的子Fiber节点
  switch (workInProgress.tag) {
    case FunctionComponent: 
      // ...省略
    case ClassComponent: 
      // ...省略
    case HostComponent:
    // ...省略其他类型
  }
}
```

___

如果在`mount`阶段或者`update`阶段没法复用，就会走到节点创建逻辑，对于常见的组件类型，最终进入`reconcileChildren()`方法

`reconcileChildren()`方法同样会通过`current`入参来区分`mount`阶段和`update`阶段，最终生成子 Fiber 节点赋值给`workInProgress.child`，作为`beginWork()`方法的返回值，并作为下次`performUnitOfWork()`的入参

```typescript
export function reconcileChildren(
  current: Fiber | null,
  workInProgress: Fiber,
  nextChildren: any,
  renderLanes: Lanes
) {
  if (current === null) {
    // 对于mount的组件，创建新的子 Fiber 节点
    workInProgress.child = mountChildFibers(
      workInProgress,
      null,
      nextChildren,
      renderLanes,
    );
  } else {
    // 对于update的组件，执行 Diff 尝试复用子节点，并打标记
    workInProgress.child = reconcileChildFibers(
      workInProgress,
      current.child,
      nextChildren,
      renderLanes,
    );
  }
}
```

此外，`reconcileChildren()`还会根据 Diff 的结果给生成的子节点打上相应标记，如果一个节点有对应DOM（`fiber.stateNode`非空）且存在标记（`fiber.effectTag`有标记），后续`Renderer`会根据他们来执行相应DOM操作

```typescript
// 标记以二进制掩码的形式记录，方便通过位运算来检测标记/打标记
export const Placement = /*                */ 0b00000000000010;
export const Update = /*                   */ 0b00000000000100;
export const PlacementAndUpdate = /*       */ 0b00000000000110;
export const Deletion = /*                 */ 0b00000000001000;

// 给节点打上插入标记
fiber.effectTag = effectTag | Placement
// 检查节点是否有插入标记
if(fiber.effectTag & Placement !== 0){/*...*/}
```

#### 【归】completeWork

`completeWork()`里重点关注对页面渲染所必须的 Host Component（原生`DOM组件`对应的`Fiber节点`） 的处理，同样的这里会根据`current`入参来区分`mount`阶段和`update`阶段（但判断`update`时还需要考虑 Fiber 节点是否有对应DOM）

```typescript
function completeWork(
  current: Fiber | null,
  workInProgress: Fiber,
  renderLanes: Lanes,
): Fiber | null {
  const newProps = workInProgress.pendingProps;

  switch (workInProgress.tag) {
    case HostComponent: {
      popHostContext(workInProgress);
      const rootContainerInstance = getRootHostContainer();
      const type = workInProgress.type;

      if (current !== null && workInProgress.stateNode != null) {
        // update的情况
        // ...省略
      } else {
        // mount的情况
        // ...省略
      }
      return null;
    }
}
```

在`update`阶段，主要是调用`updateHostComponent()`处理 props（回调Props、styleProps、childrenProps等），找出 props 的增量更新队列。在`updateHostComponent()`内部，被处理完的`props`会被赋值给`workInProgress.updateQueue`，并最终会在`commit阶段`被渲染在页面上

```typescript
workInProgress.updateQueue = (updatePayload: any);
// updatePayload为数组形式，偶数索引的值为变化的propKey，奇数索引的值为变化的propValue
```

___

在`mount`阶段，由于是新上屏的`workInProgressFiber`，该阶段需要干三件事

1. 为`workInProgressFiber`生成对应的DOM
2. 将子孙DOM插入到刚生成的DOM上
3. 像`update`阶段一样处理 props

```typescript
// mount的情况
const currentHostContext = getHostContext();
// 为fiber创建对应DOM节点
const instance = createInstance(
    type,
    newProps,
    rootContainerInstance,
    currentHostContext,
    workInProgress,
  );
// 将子孙DOM节点插入刚生成的DOM节点中
appendAllChildren(instance, workInProgress, false, false);
// DOM节点赋值给fiber.stateNode
workInProgress.stateNode = instance;

// 与update逻辑中的updateHostComponent类似的处理props的过程
if (
  finalizeInitialChildren(
    instance,
    type,
    newProps,
    rootContainerInstance,
    currentHostContext,
  )
) {
  markUpdate(workInProgress);
}
```

#### effectList

在整个`render阶段`的遍历的过程中，`beginWork()`和`completeWork()`是交替执行的，`beginWork()`会给部分节点打标记（`effectTag`），作为`commit阶段`执行`effect`（生命周期函数、DOM操作等）的依据

在`commit阶段`如何找到这些有标记的节点成为关键问题，如果重新遍历一次树，这显然是很低效的。为了解决这个问题，`completeWork()`在其上层函数`completeUnitWork()`完成`effectList`的向上收集：

- 将当前节点现有的`effectList`拼接到父节点的`effectList`上
- 如果当前节点有标记，把自身接到父节点的`effectList`上

> `effectList`是一个链表结构，他把带标记的节点串起来
>
> 每个节点都有属于自己范围的`effectList`，`fiber.firstEffect`指定表头，`fiber.lastEffect`指定表尾，通过`fiber.nextEffect`访问下一个标记节点。节点的`effectList`表示为了更新当前组件，需要处理的节点顺序
>
> 父节点的`effectList`必定包含子节点的的`effectList`，如父`effectList`为A->B->C，子`effectList`为A->B

在完成`render阶段`的整个遍历后，`workInProgress Fiber树`的根节点就会得到完整的`effectList`，在`commit阶段`只需遍历这条`effectList`即可执行所有`effect`

```typescript
function completeUnitOfWork(unitOfWork: Fiber): void {
  let completedWork = unitOfWork;
  do {
    const current = completedWork.alternate;
    const returnFiber = completedWork.return;

    const next = completeWork(current, completedWork, subtreeRenderLanes);

    // effect list构建
    if (returnFiber !== null && (returnFiber.flags & Incomplete) === NoFlags) {
      // 【1】先向父节点传递现有effectList
      if (returnFiber.firstEffect === null) {
        // 父节点是第一次收到子节点的effectList，不管实际有没有effect先接上去再说
        returnFiber.firstEffect = completedWork.firstEffect;
      }
      if (completedWork.lastEffect !== null) {
        // 当前节点有effect
        if (returnFiber.lastEffect !== null) {
          // 说明当前节点是靠后的兄弟节点，靠前的子节点有effect，已经给returnFiber.lastEffect赋值过了，
          // 现在需要把兄弟节点的effectList接到父上去
          returnFiber.lastEffect.nextEffect = completedWork.firstEffect;
        }
        // 更新 lastEffect
        returnFiber.lastEffect = completedWork.lastEffect;
      }

      // 【2】如果当前节点有标记，把自身接到父节点的effectList上
      const flags = completedWork.flags;
      if (flags > PerformedWork) {
        // 当前节点有effect连接上effect list
        if (returnFiber.lastEffect !== null) {
          returnFiber.lastEffect.nextEffect = completedWork;
        } else {
          // 父节点没有firstEffect，说明的情况是第一次遇见有effect的节点
          returnFiber.firstEffect = completedWork;
        }
        returnFiber.lastEffect = completedWork;
      }
    }

    // 当前节点有兄弟节点，停止completeWork，对兄弟节点执行beginWork
    const siblingFiber = completedWork.sibling;
    if (siblingFiber !== null) {
      workInProgress = siblingFiber;
      return;
    }
    // 当前节点没有兄弟节点，继续返回父节点，对父节点继续执行completeWork
    completedWork = returnFiber;
    workInProgress = completedWork;
  } while (completedWork !== null);
}

```

### 总结

> `render`阶段：主要工作是构建`workInProgress Fiber树`和收集`effectList`，期间`beginWork()`和`completeWork()`交替执行
>
> `beginWork()`：为【递】遍历到的`workInProgressFiber`节点创建、连接并返回子 Fiber 节点
>
> - `mount阶段`：创建新的子 Fiber 节点
> - `update阶段`：尝试直接复用子`currentFiber`，如果不能直接复用，执行 Diff 尝试复用子`currentFiber`，顺便打标记`effectTag`
>
> `completeWork()`：为【归】遍历到的`workInProgressFiber`节点
>
> - `mount阶段`：为当前节点创建DOM，并将子孙DOM接到新创建的DOM；处理 props 更新
> - `update阶段`：处理 props 更新，收集`updateQueue`
> - 向父节点传递现有`effectList`，如果当前节点有标记，把自身也接到父节点的`effectList`上

### 参考

[React 技术揭秘](https://react.iamkasong.com/process/reconciler.html)

[浅析React中的EffectList](https://juejin.cn/post/6947168516394975239)
