### Commit 阶段（Renderer）

> 从根 fiber 出发通过`firstEffect`遍历所有副作用节点，**同步处理节点更新和生命周期钩子**的过程

`commit`阶段的主要工作分为三部分：

- before mutation 阶段（执行DOM操作前）
- mutation 阶段（执行DOM操作）
- layout 阶段（执行DOM操作后）

#### before mutation 阶段

遍历`effectList`并对每个节点调用`commitBeforeMutationEffects()`，主要工作有三：

1. 处理DOM节点渲染/删除后的 autoFocus/blur 逻辑
2. 调用`getSnapshotBeforeUpdate()`生命周期钩子
3. **异步调度`useEffect`**（将在 commit 之后执行，防止阻塞浏览器渲染）

```typescript
function commitBeforeMutationEffects() {
  while (nextEffect !== null) {
    const current = nextEffect.alternate;

    if (!shouldFireAfterActiveInstanceBlur && focusedInstanceHandle !== null) {
      // 【工作1】...focus blur相关
    }

    const effectTag = nextEffect.effectTag;

    // 【工作2】调用 getSnapshotBeforeUpdate 生命周期
    if ((effectTag & Snapshot) !== NoEffect) {
      commitBeforeMutationEffectOnFiber(current, nextEffect);
    }

    // 【工作3】调度useEffect
    if ((effectTag & Passive) !== NoEffect) {
      if (!rootDoesHavePassiveEffects) {
        rootDoesHavePassiveEffects = true;
        scheduleCallback(NormalSchedulerPriority, () => {
          flushPassiveEffects();
          return null;
        });
      }
    }
    nextEffect = nextEffect.nextEffect;
  }
}
```

#### mutation 阶段

遍历`effectList`并对每个节点调用`commitMutationEffects()`，主要工作有三：

1. 根据`ContentReset effectTag`重置文字节点
2. 更新`ref`（commitDetachRef）
3. 根据`effectTag`分别执行不同的DOM更新，调用相关生命周期钩子和 hook

重点关注第三步中不同`effectTag`的处理逻辑：

【Placement】表示 fiber 节点对应的DOM需要插入到页面中，对应方法`commitPlacement()`

1. 获取当前 fiber 节点的父级DOM节点

   ```typescript
   const hostParentFiber = getHostParentFiber(finishedWork);
   // 父级DOM节点
   const parentStateNode = parentFiber.stateNode;
   ```

2. 获取当前 fiber 节点的兄弟DOM节点

   ```typescript
   const before = getHostSibling(finishedWork);
   ```

3. 根据兄弟DOM节点是否存在，决定调用`parentNode.insertBefore`或`parentNode.appendChild`来执行DOM插入

   ```typescript
   // parentStateNode是否是rootFiber
   if (isContainer) {
     insertOrAppendPlacementNodeIntoContainer(finishedWork, before, parent);
   } else {
     insertOrAppendPlacementNode(finishedWork, before, parent);
   }
   ```

PS：并不是所有 fiber 节点都有对应的DOM，只有 tag 为`HostComponent`的 fiber 才会有对应的 DOM，所以从当前 fiber 向上寻找父级DOM和向右向下寻找兄弟DOM是可能跨层遍历的

___

【Update】表示 fiber 节点需要更新，对应方法`commitWork()`，根据 fiber 的 tag 分别处理

- 对`FunctionComponent`，**同步执行`useLayoutEffect`的销毁函数**
- 对`HostComponent`，将 render-completeWork 阶段中收集的`updateQueue`（props key-value 对）更新到DOM上

___

【Deletion】表示 fiber 节点对应的DOM需要删除，对应方法`commitDeletion()`

1. 递归调用当前 fiber 节点及其子孙 fiber 节点中 tag 为`ClassComponent`的`componentWillUnmount()`生命周期钩子，并从页面中移除 fiber 节点对应的DOM
2. 解绑 ref
3. **异步调度`useEffect`的销毁函数**（将在 commit 之后执行，防止阻塞浏览器渲染）

#### layout 阶段

遍历`effectList`并对每个节点调用`commitLayoutEffects()`，主要工作有二：

1. 调用相关生命周期钩子和 hook
   - 对于`ClassComponent`，根据是`mount`还是`update`调用`componentDidMount/Update()`生命周期钩子；`this.setState()`的回调也会调用
   - 对于`FunctionComponent`，**同步执行`useLayoutEffect`的回调，并根据依赖变化填充`useEffect`的`effetc`执行数组**，然后再次异步调度`useEffect`（注意：layout 阶段`useEffect`调度和 beforeMutation 阶段`useEffect`调度是互斥的，如果之前调度过，本次的调度会被跳过）
2. 更新`ref`（commitAttachRef）

【PS】current Fiber 树的切换发生在 mutation 阶段之后，layout 阶段之前。为的是能让 mutation 阶段的`componentWillUnmount()`能够获取到卸载前的实例，让 layout 阶段的`componentDidMount/Update()`能够获取到更新后的实例

```typescript
// mutation 阶段
root.current = finishedWork;
// layout 阶段
```



### 总结

> `commit`阶段：三次遍历`effectList`上的节点，分别按序执行`before mutation`、`mutation`、`layout`逻辑
>
> `before mutation`
>
> - 处理DOM节点渲染/删除后的 autoFocus/blur 逻辑
> - 调用`getSnapshotBeforeUpdate()`生命周期钩子
> - **异步调度`useEffect`**
>
> `mutation`
>
> 1. 根据`ContentReset effectTag`重置文字节点
> 2. 更新`ref`（commitDetachRef）
> 3. 根据`effectTag`分别执行不同的DOM更新，调用相关生命周期钩子和 hook
>    - Placement，获取当前 fiber 的父级DOM和兄弟DOM，插入到相应位置
>    - Update，函数组件**同步执行`useLayoutEffect`的销毁函数**，原生组件将`updateQueue`更新到DOM上
>    - Deletion，递归执行子孙类组件的`componentWillUnmount()`；移除当前DOM；解绑 ref；**异步调度`useEffect`的销毁函数**
>
> `layout`
>
> 1. 调用相关生命周期钩子和 hook
>    - 类组件调用`componentDidMount()`或`componenteDidUpdate()`，及 setState 回调
>    - 函数组件**同步执行`useLayoutEffect`的回调，根据依赖变化填充`useEffect`的`effetc`执行数组**
> 2. 更新`ref`（commitAttachRef）

### 参考

[React 技术揭秘](https://react.iamkasong.com/renderer/prepare.html)