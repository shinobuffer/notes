> Hooks 是一个更贴近底层的东西，他和生命周期不在同一个抽象层次

### 极简 Hook 实现

使用`useState`时，你有没有想过为什么他能当 state 使用，他是怎么知道他所在的组件的并正确触发更新的。下面就以`useState`为例简要实现一个 Hook 来一探其原理

考虑如下`useState`的用例：

```tsx
function App() {
  const [num, updateNum] = useState(0);

  return <p onClick={() => updateNum(num => num + 1)}>{num}</p>;
}
```

可以将工作分为两部分：

1. 通过一些途径产生`更新`，`更新`会造成组件`render`
   - 调用`ReactDOM.render`会产生`mount`的`更新`，`更新`内容为`useState`的`initialValue`（即`0`）
   - 点击`p`标签触发`updateNum`会产生一次`update`的`更新`，`更新`内容为`num => num + 1`
2. 组件`render`时`useState`返回的`num`为更新后的结果

#### 更新是什么

> 通过一些途径产生`更新`，`更新`会造成组件`render`

在这个极简例子中，`更新`（下称`update`）的数据结构如下

```typescript
const update = {
  // 更新执行的函数
  action,
  // 与同一个Hook的其他更新形成链表
  next: null
}
```

调用`updateNum`实际调用的是`dispatchAction.bind(null, hook.queue)`

```typescript
function dispatchAction(queue, action) {
  // 创建update
  const update = {
    action,
    next: null
  }

  // 环状单向链表操作，update.pending始终指向表尾
  if (queue.pending === null) {
    update.next = update;
  } else {
    update.next = queue.pending.next;
    queue.pending.next = update;
  }
  queue.pending = update;

  // 模拟React开始调度更新
  schedule();
}
```

同一个 Hook 产生的`update`按调用顺序连接形成一个**环形单向链表**，由`queue.pending`记录表尾，如果链表非空，则表头为`queue.pending.next`，要遍历`update`时只要从表头`queue.pending.next`遍历到表尾`queue.pending`即可

```typescript
// 一开始链表为空
queue.pending = null;

// 创建并插入第一个更新u0，链表里只有u0，表头表尾都是u0
queue.pending = u0 ---> u0
                ^       |
                |       |
                ---------

// 创建并从表尾插入第二个更新u1
queue.pending = u1 ---> u0   
                ^       |
                |       |
                ---------
```

每个 Hook 都有自己`hook`对象，`hook.queue`保存了由该 Hook 产生的`update`

```typescript
const hook = {
  // 上文介绍的queue，保存了由该hook产生的update
  queue: {
    pending: null
  },
  // 保存hook当前state
  memoizedState: initialState,
  // 与下一个Hook连接形成单向无环链表
  next: null
}
```

同一个组件里的 Hooks 按调用顺序连接形成一个**单向链表**，存储在组件对应的 Fiber 节点中

```typescript
const fiber = {
  // 保存该FunctionComponent对应的Hooks链表
  memoizedState: null,
  // 指向App函数
  stateNode: App
};
```

>总结：
>
>每个`useState`调用对应一个`hook`对象；`hook.queue`保存了该`hook`产生的`update`序列
>
>多个`useState`对应的`hook`按照在组件中的调用顺序组成链表，由组件对应的`fiber.memoizedState`保存

#### 调度更新

在`dispatchAction()`中创建并插入`update`后，调用`schedule()`来调度更新，使用变量`workInProgressHook`来指向当前正在工作的`hook`，在组件`render`时每遇到下一个`useState`，我们移动`workInProgressHook`指向下一个`hook`

```typescript
// 首次render时是mount
isMount = true;

function schedule() {
  // 更新前将workInProgressHook重置为fiber保存的第一个Hook
  workInProgressHook = fiber.memoizedState;
  // 触发组件render
  fiber.stateNode();
  // 组件首次render为mount，以后再触发的更新为update
  isMount = false;
}
```

这样，只要每次组件`render`时`useState`的**调用顺序及数量保持一致**，那么始终可以通过`workInProgressHook`找到当前`useState`对应的`hook`对象

___

> 组件`render`时`useState`返回的`num`为更新后的结果

组件`render`时会执行若干个`useState`，在每个`useState`执行时，主要工作内容如下

1. 找到当前`useState`对应的`hook`
2. 执行当前`useState`对应`hook`的所有`update`
3. 返回所有`update`执行完后的`state`

```typescript
function useState(initialState) {
  // 记录当前useState对应的hook
  let hook;

  // 【步骤1】找到当前useState对应的hook
  if (isMount) {
    // 组件mount时为该useState生成hook，并插到fiber.memoizedState链表末尾
    hook = {
      queue: {
        pending: null
      },
      memoizedState: initialState,
      next: null
    }

    if (!fiber.memoizedState) {
      fiber.memoizedState = hook;
    } else {
      workInProgressHook.next = hook;
    }
    // 移动workInProgressHook指针
    workInProgressHook = hook;
  } else {
    // 组件update时当前useState对应的hook为workInProgressHook
    hook = workInProgressHook;
    // 移动workInProgressHook指针，为下一次useState调用做好准备
    workInProgressHook = workInProgressHook.next;
  }

  // 【步骤2】执行当前useState对应hook的所有update
  let baseState = hook.memoizedState;
  if (hook.queue.pending) {
    // 从update链表中取到第一个update
    let firstUpdate = hook.queue.pending.next;

    do {
      // 执行update action
      const action = firstUpdate.action;
      baseState = action(baseState);
      firstUpdate = firstUpdate.next;

      // 最后一个update执行完后跳出循环
    } while (firstUpdate !== hook.queue.pending.next)

    // 执行完update后清空update链表
    hook.queue.pending = null;
  }
  // 将所有update执行完state作为memoizedState
  hook.memoizedState = baseState;

  return [baseState, dispatchAction.bind(null, hook.queue)];
}
```

#### 与真实 Hook 的区别

- `React Hooks`没有使用`isMount`变量，而是在不同时机使用不同的`dispatcher`。`mount`时的`useState`与`update`时的`useState`不是同一个函数
- `React Hooks`有中途跳过`更新`的优化手段
- `React Hooks`有`batchedUpdates`，当在`click`中触发三次`updateNum`，实际情况只会触发一次更新
- `React Hooks`的`update`有`优先级`概念，可以跳过优先级不高的`update`

### 参考

[React 技术揭秘](https://react.iamkasong.com/hooks/create.html)

