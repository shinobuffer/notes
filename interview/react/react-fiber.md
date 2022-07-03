> Fiber 架构让可中断的异步更新成为现实

### Fiber 含义

Fiber 的三层含义

1. 作为架构，新的`Reconciler`基于 Fiber 节点实现可中断的异步更新，称为`Fiber Reconciler`
2. 作为静态数据结构，Fiber 节点对应着 ReactElement，记录了组件类型、对应DOM节点等信息，其实就相当于虚拟DOM
3. 作为动态**工作单元**，Fiber 节点保存了本次更新中该组件改变的状态、要执行的工作

```typescript
function FiberNode(
  tag: WorkTag,
  pendingProps: mixed,
  key: null | string,
  mode: TypeOfMode,
) {
  // 作为静态数据结构的属性
  this.tag = tag;
  this.key = key;
  this.elementType = null;
  this.type = null;
  this.stateNode = null;

  // 用于连接其他Fiber节点形成Fiber树
  this.return = null;
  this.child = null;
  this.sibling = null;
  this.index = 0;

  this.ref = null;

  // 作为动态工作单元的属性
  this.pendingProps = pendingProps;
  this.memoizedProps = null;
  this.updateQueue = null;
  this.memoizedState = null;
  this.dependencies = null;

  this.mode = mode;

  this.effectTag = NoEffect;
  this.nextEffect = null;

  this.firstEffect = null;
  this.lastEffect = null;

  // 调度优先级相关
  this.lanes = NoLanes;
  this.childLanes = NoLanes;

  // 指向该fiber在另一次更新时对应的fiber
  this.alternate = null;
}
```

Fiber 节点间通过三个属性相互连接形成树

```typescript
// 指向父级Fiber节点
this.return = null;
// 指向子Fiber节点
this.child = null;
// 指向右边第一个兄弟Fiber节点
this.sibling = null;
```

比如有一个组件如下

```tsx
const App: React.FC = () => {
  return (
    <div>
      i am
      <span>KaSong</span>
    </div>
  )
}
```

其对应的 Fiber 树长这样的

<img src="react-fiber.assets/fiber.png" alt="Fiber架构" style="zoom:33%;" />

不像传统树结构需要使用递归来实现遍历，Fiber 这种链式结构组成的树，可以通过使用循环遍历的方式来遍历整棵树。每个 Fiber 节点都是一个工作单元，每段时间可以遍历处理若干个 Fiber 节点的工作，然后中断等待下一段时间的到来继续从上次中断的地方继续遍历，直至重新回到跟节点完成整个遍历，是不是有**【可中断的异步更新】**那味了（在后面介绍 render 阶段时，会介绍遍历节点时的具体工作）

具体的遍历算法是一个深度优先遍历：

- 有孩子优先前往孩子
- 没孩子优先前往右边的兄弟节点
- 没有孩子又没有右边兄弟就返回父节点
- 如果回到了根节点，终止遍历

```typescript
function walk(o) {
  // 根节点
  const root = o;
  // 当前遍历到的节点
  let current = o;

  while (true) {
    // 使用doWork，连接根节点和子节点，并返回根节点的第一个子节点
    const child = doWork(current);

    // 如果有子节点，将当前的指针指向子节点，并进入下一个循环
    if (child) {
      current = child;
      continue;
    }

    // 如果当前指针等于根节点，结束遍历
    if (current === root) {
      return;
    }

    // 如果没有兄弟，返回父节点，继续循环判断
    while (!current.sibling) {
      // 如果当前指针等于根节点，结束遍历
      if (!current.return || current.return === root) {
        return;
      }
      // 如果没有子节点，并且没有兄弟节点，返回父节点
      current = current.return;
    }
    // 如果有兄弟节点，将当前指针设置为兄弟节点，进入下一个循环
    current = current.sibling;
  }
}
```

### 双缓存 Fiber 树

在 React 中最多会同时存在两棵 Fiber树。当前屏幕上显示内容对应的 Fiber树 称为`current Fiber树`（其节点称为`currentFiber`），正在内存中构建的 Fiber树 称为`workInProgress Fiber树`（其节点称为`workInProgressFiber`）

`currentFiber`和`workInProgressFiber`可以通过`alternate`属性找到对方

```typescript
currentFiber.alternate === workInProgressFiber;
workInProgressFiber.alternate === currentFiber;
```

React 应用的根节点通过使`current`指针在不同 Fiber 树的`rootFiber`间切换来完成`current Fiber树`指向的切换。当`workInProgress Fiber树`构建完成并交给`Renderer`渲染上屏之后，应用根节点的`current`就会指向`workInProgress Fiber树`，此时`workInProgress Fiber树`成为`current Fiber树`，有更新来了就生成新的`workInProgress Fiber树`重复上述流程

> 应用根节点`fiberRootNode`是唯一的，他由首次执行`ReactDom.render()`创建
>
> `rootFiber`是需要挂载的组件对应的 Fiber 树的根节点

下面就以组件的 mount 和 update 阶段为例介绍这个双缓存机制，先来看一小段代码，这是我们要挂载的组件

```typescript
function App() {
  const [num, add] = useState(0);
  return (
    <p onClick={() => add(num + 1)}>{num}</p>
  )
}

ReactDOM.render(<App/>, document.getElementById('root'));
```

#### mount 阶段

| 图示                                                         | 说明                                                         |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| <img src="react-fiber.assets/rootfiber-20220701152618767.png" alt="rootFiber" style="zoom:100%;" /> | 1.首次调用`ReactDom.render()`，创建应用根节点`fiberRootNode`和挂载组件的根节点`rootFiber`，并让`fiberRootNode`的`current`指向`rootFiber` |
| <img src="react-fiber.assets/workInProgressFiber-20220701152636546.png" alt="workInProgressFiber" style="zoom:100%;" /> | 2.由于是首屏渲染，当前`current Fiber树`是空的（图左），开始进入 render 阶段，根据组件返回的 ReactElement 创建 Fiber 节点并连接起来构建 Fiber 树，被称为`workInProgress Fiber树`（图右） |
| <img src="react-fiber.assets/wipTreeFinish-20220701152650289.png" alt="workInProgressFiberFinish" style="zoom:100%;" /> | 3.构建完成后的`workInProgress Fiber树`在 commit 阶段被渲染上屏，然后让`fiberRootNode`的`current`指向`workInProgress Fiber树`，`workInProgress Fiber树`成为`current Fiber树`（图左） |

#### update 阶段

点击 p 标签触发更新

| 图示                                                         | 说明                                                         |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| <img src="react-fiber.assets/wipTreeUpdate.png" alt="wipTreeUpdate" style="zoom:100%;" /> | 1.更新触发新一轮 render 阶段，再次构建一个`workInProgress Fiber树`（图右），构建过程中可能复用`current Fiber树`（图左）对应的节点（是否复用由 diff 算法决定） |
| <img src="react-fiber.assets/currentTreeUpdate.png" alt="currentTreeUpdate" style="zoom:100%;" /> | 2.构建完成后的`workInProgress Fiber树`在 commit 阶段被渲染上屏，然后让`fiberRootNode`的`current`指向`workInProgress Fiber树`，`workInProgress Fiber树`成为`current Fiber树`（图左） |

### 参考

[React 技术揭秘](https://react.iamkasong.com/process/fiber.html)
