

### React 理念

> React 标榜自己是构建**快速响应**的大型 Web 应用程序的首选方式

影响快速响应的两大因素：

- CPU 瓶颈：计算量过大/设备性能不足导致 JS 执行时间过长，影响布局和绘制导致掉帧
- IO 瓶颈：需要等待异步操作才能进一步操作或展示

为了解决 CPU 瓶颈，React 会在浏览器的每一帧的时间中，预留一些时间给 JS 线程，利用这部分时间更新组件。当预留的时间不够用时，React 会暂停手头的工作，将线程控制权交还浏览器进行渲染，等待下一帧再继续执行被中断的工作

> 这种将长任务分拆到每一帧中，每次执行一小段任务的操作，被称为`时间切片`

而要实现`时间切片`的关键是：将**同步的更新**变为**可中断的异步更新**



至于 IO 瓶颈，这是客观存在前端无法干预的，前端能做的只有减少用户对这种延迟的感知，这属于【人机交互设计】的范畴。不过 React 提供了`Suspense`功能和配套 hook 来支持这些优化，而要支持这些特性，同样需要将**同步的更新**变为**可中断的异步更新**



总结：React 为了践行【快速响应】的理念，落实到实现上，需要将**同步的更新**变为**可中断的异步更新**

### React 老架构

React 老架构（15）分为两层：

1. `Reconciler`（协调器）——负责找出变化的组件
2. `Renderer`（渲染器）——负责将变化的组件渲染到页面上

####  Reconciler（协调器）

在 React 中，可以通过`setState()`、`forceUpdate()`、`ReactDom.render()`等 API 触发更新

每当有更新发生时，`Reconciler`会做如下工作：

- 调用组件`render`方法，将返回的 ReactElement 转化为虚拟DOM
- 将虚拟DOM和上次更新时的虚拟DOM对比，找出本次更新中变化的虚拟DOM
- 通知`Renderer`将变化的虚拟DOM渲染到页面上

#### Renderer（渲染器）

在每次更新发生时，`Renderer`接到`Reconciler`通知，根据底层平台进行不同的调用，将变化的组件渲染在当前宿主环境

`Renderer`是架构中平台相关的部分，不同的平台会有不同的`Renderer`，通常以包的形式独立出来，比如`react-dom-renderer`负责在 H5 中渲染 DOM、`react-native-renderer`负责在 And/IOS 中渲染 Native 视图

#### 老架构缺点

【stack reconciler】是 React 15 及更早的解决方案，在`Reconciler`中，组件在`mount`和`update`阶段会递归的更新子组件。由于是递归执行，所以更新一旦开始，中途就无法中断，如果耗时超过 16ms，交互就会卡顿

为了解决这个痛点，React 在 16.0 重新实现了新的`Reconciler`，引入了`Fiber`架构，为的是用**可中断的异步更新**代替**同步的更新**

### React 新架构

React 新架构（16+）相比之前多了一个`Scheduler`（调度器）

1. `Scheduler`（调度器）——调度任务的优先级，高优任务优先进入`Reconciler`
2. `Reconciler`（协调器）——负责找出变化的组件
3. `Renderer`（渲染器）——负责将变化的组件渲染到页面上

#### Scheduler（调度器）

任务中断的判断标准之一是浏览器是否有剩余空闲时间，为此 React 需要一种机制让浏览器空闲时通知 React

其实部分浏览器提供了`requestIdleCallback(callback)`API 来满足这个需要：浏览器会在空闲时调用回调，告诉开发着剩余空闲时间。但出于兼容性和稳定性的考虑，React 并没有使用，而是实现了功能更完备的`requestIdleCallback`polyfill，这个部分就是`Scheduler`，除了提供空闲时回调，还提供了多种调度优先级供任务设置

#### Reconciler & Renderer

在 React 16 中，更新工作从递归变成了可以中断的循环过程，每次进入工作循环前都会调用`shouldYield()`判断浏览器是否空闲。而能这样做的前提，是React 通过某种方案使得**更新工作可分解为增量单元**（后面 Fiber 架构会详细说明）

```typescript
function workLoopConcurrent() {
  // Perform work until Scheduler asks us to yield
  while (workInProgress !== null && !shouldYield()) {
    workInProgress = performUnitOfWork(workInProgress);
  }
}
```

因为更新工作会被中断，所以在工作完成前`Renderer`是不能介入的。当`Scheduler`将任务交给`Reconciler`后，`Reconciler`会为变化的虚拟DOM打上代表增/删/更新的标记

只有所有的组件都完成`Reconciler`工作后，才会交给`Renderer`，`Renderer`根据`Reconciler`在虚拟DOM上打的标记，执行对应的DOM操作

### 可中断的异步更新

> 用**可中断的异步更新**代替**同步的更新**

这句话在上文重复提到，因为这是新架构中`Reconciler`重构的一大目的

**可中断的异步更新**：【更新】在执行过程中可能会被打断（浏览器时间分片用尽，或有更高优的任务插队），当可以继续执行时恢复之前执行的中间状态

说到中断和恢复，不得不提`Generator`，但是`Generator`执行的`中间状态`是上下文关联的，没办法很好的解决“高优任务插队”的问题，所以 React 并没有采用，而是自己实现了一个 React 版本的 generator，称其为 `Fiber`。他是 React 内部实现的一套状态更新机制：支持任务不同优先级，可中断与恢复，并且恢复后可以复用之前的`中间状态`

### 总结

>老架构采用【stack reconciler】，递归的更新逐渐一旦开始便无法中断，可能占用时间过长导致卡顿
>
>新架构实现 Fiber 架构【fiber reconciler】替代【stack reconciler】，让**可中断的异步更新**成为现实

### 参考

[React 技术揭秘](https://react.iamkasong.com/preparation/idea.html)

