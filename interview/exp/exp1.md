#### 函数式编程的理解
> 首先，函数式编程是一种「编程范式」

相比命令式编程，函数式编程中的函数更**贴近数学上的函数，强调函数输入和输出的一对一映射关系**而不是执行的过程，函数间可通过组合形成更复杂的映射关系

函数式编程有几个重要规定：

- **数据是不可变的**：如果需要修改数据就必须生成新的数据
- **函数是无副作用的**：所有函数都应该是纯函数，函数不依赖外部只依赖输入，确定的输入必定返回确定的输出
- **函数只接受一个参数**

**函数式编程强调数据不可变性和无副作用函数**，将一个复杂的任务分解为多个独立纯函数的组合，有利于**提高代码的复用性、可预测性和可测试性**

> 但更多时候，我们把函数式编程看作一种非强制的「编程风格」

**JS 不是严格意义上的函数式编程语言，但开发者去可以去践行函数式编程的部分理念，来享受函数式带来的一些好处**。比如 Redux 就是一个很明显的例子，像纯函数 reducer、柯里化中间件，就是函数式编程风格的一种实践


#### React Hook 的理解，解决问题？

> **Hook 可以理解为函数组件的加强，它使得函数组件也能实现类组件的特性**（状态、生命周期等）

Hook 的意思是「钩子」，函数组件如果需要引入状态等副作用，就用相应的「钩子」把他们「钩」进来。比如 React 内置 hook 中，`useState()`为函数组件引入状态、`useEffect()`为函数组件引入副作用的处理。**这是 hook 解决的第一个问题，即「函数组件无法实现类组件特性」的问题**



**hook 解决的第二个问题是「业务逻辑难以分离和复用」的问题**

在 hook 出现之前，涉及状态和生命周期的组件必须使用类组件，但是类组件也有自身的一些缺点：

- **状态相关业务逻辑难以分离复用**，需要引入 render props/高阶组件等复杂模式解决
- 按生命周期划分副作用，导致**不相关的逻辑堆积在同一个生命周期，相关的逻辑却分散在不同的生命周期**

hook 的出现使得开发者可以基于内置 hook 的组合，将相关逻辑封装为一个自定义钩子供函数组件使用，实现逻辑的抽离和复用。合理的封装 hook 来将相关副作用抽离，能一定程度的提高函数组件的纯度，使得数据和视图之间的映射更加清晰，实现更好的可读性、可复用性、可测性

#### Redux 理解

> **Redux 是一个单纯的状态管理库**

Redux 有三个基本原则，**确保状态以可预测、可回溯的方式进行更新**：

- **单一数据源**（状态由 Store 对象持有，通过`store.getState()`获取状态）
- **state 是只读的**（想要修改 state 只能返创建新的 state 替换原 state）
- **使用纯函数来执行修改**（Reducer 不能有副作用）

Redux 工作流程相当于一个状态机：

- UI 层通过 dispatch 派发一个的 Action 对象（type+payload），表示发起状态变更的请求
- Action 最终交由 Reducer 处理，Reducer 根据当前 state 和传入的 Action 生成新的 state
- 新的 state 替换当前 state，完成一次状态更新，并通知所有订阅者



Redux 中还有一个中间件的概念，**中间件是一个经过柯里化的函数，在传入第一个参数 Store 对象后，中间件返回一个 oldDispatch => newDispatch 映射函数**。也就是说**中间件能对传入的`dispatch`方法进行增强，返回一个新的增强后的`dispatch`方法（装饰器模式）**。
```typescript
(store) => (next) => (action) => {
	// ...
}
```
**多个中间件通过 compose 组合将增强效果整合在一起**，在传入原始`store.dispatch`方法后返回最终增强的`dispatch`方法，用于替换原始`dispatch`方法，这样开发者就能调用加强后的`dispatch`方法

> 使用中间件加强的流程：
>
> `applyMiddleware(middlewares)` => `enhancer`
>
> `enhancer(createStore)` => `enhancedCreateStore`
>
> `enhancedCreateStore(reducer)` => `({ enhancedDispatch })`

比如 redux-thunk 中间件通过增强`dispatch`让`dispatch`支持函数入参（Action Creator），通过往函数中注入`dispatch`和 state 的方式，让开发者可以在 Action Creator 中更优雅的处理异步

#### React-redux


> Redux 只是一个单纯的状态管理库，与 UI 无关，所以在 React 中使用 Redux 通常会配合使用 **`react-redux`将 UI 与 Redux 关联在一起**

`react-redux`提供了一个`Provider`组件，利用 Context API 向下传递包含 store 和 subscription 对象的上下文

`react-redux`提供了一个柯里化的`connect`函数，作用是将目标组件关联到 store 上，在 store 状态发生变更时触发组件的更新

`connect`第一层函数接受两个 map 方法，他描述了需要将哪些状态和 dispatch 调用映射到目标组件的 props 上

`connect`第二层函数是一个高阶组件，对传入的目标组件进行增强并返回一个包装组件：

- 创建一个的 subscription 对象订阅父级 subscription 对象，用来**订阅父级 connect 组件的更新，父级发生更新时，触发自身的更新检查**
- **包装组件进行更新检查时，会重新计算新的 props 并跟之前的 props 进行比较，发现不同就强制触发自身更新**
- 包装组件更新时，除了将最新的 props 传递给目标组件，还会**将上下文中的父级 subscription 替换为自己的 subscription 向下传递**

> **subscription 机制让 connect 组件存在层级嵌套时，可以嵌套收集订阅，父级更新后执行子节点的订阅回调通知子节点进行更新检查，确保「父->子」的更新顺序**

[代码参考](https://github.com/dennis-jiang/Front-End-Knowledges/blob/master/Examples/React/react-redux/src/my-react-redux/connect.js)

在函数式组件中，会使用`useSelector`和`useDispatch`两个 hook 替代`connect`函数

```typescript
export default function useSelector<T, Selected extends unknown>(
  selector: (state: T) => Selected,
  equalityFn?: EqualityFn<Selected>,
): Selected {
  //【1】从上下文中获取 store
  const store = useContext(StoreContext);
  //【2】借助无意义的useState/useReducer，实现强制触发更新
  const [, forceRender] = useReducer((s) => s + 1, 0);


  const latestStoreState = useRef<T>(store.getState());
  //【3】使用 ref 记录更新前的目标 state
  const latestSelectedState = useRef<Selected>(selector(latestStoreState.current));

  useEffect(() => {
    function checkUpdate() {
      const newState = store.getState();
      if (newState === latestStoreState) return;

      const newSelectedState = selector(newState);
      if (!equalityFn) equalityFn = (a, b) => a === b;

      // 【5】比较更新前后的目标 state，如果发生变动就触发强制更新
      if (!equalityFn(newSelectedState, latestSelectedState.current)) {
        latestSelectedState.current = newSelectedState;
        latestStoreState.current = newState;
        forceRender();
      }
    }
    // 【4】监听 store 状态的更新，store 更新时触发自身更新检查
    const unsubscribe = store.subscribe(checkUpdate);
    return () => unsubscribe();
  }, [store]);

  // 【6】返回目标 state
  return latestSelectedState.current;
}
```

#### rematch

> rematch 是对 redux 的二次封装状态管理框架，他简化了 redux 的使用，对 TS 友好，能极大的提高开发体验

- **rematch 最大的特点是他以 model 来组织状态**，状态及状态的操作（无理同步还是异步）都统一定义在 model 中，状态定义在`state`字段、同步操作定义在`reducers`字段、异步操作定义在`effects`字段，**更加内聚，也简化了代码组织**
- 定义 model 本身就在定义 ActionType，开发者无需额外定义 ActionType，也不用在 reducer 中对 ActionType 进行判断，派发 Action 时只需执行相应方法即可，**rematch 根据函数名自动生成 ActionType，减少了大量模板代码**
- 基于 model 组织状态的方案，**天然支持 namespace，解决了 ActionType 冲突的可能**
- rematch **副作用实现机制更简单（使用async/await）**，他实现了一个中间件加强了 dispatch 方法，根据 ActionType 携带的函数名判断是同步还是异步更新并执行对应的操作，**统一了同步和异步更新中 dispatch 的调用方式**
- rematch **对 TS 极其友好，拥有完善的类型推算**，状态和操作函数全部都有类型约束

此外 rematch **支持以插件的形式来加强 redux**，并提供了多种可选插件，比如 immer 插件可以实现 immutable
