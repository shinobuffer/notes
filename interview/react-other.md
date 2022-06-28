### 为什么采用链表来描述 Fiber 树

> 因为基于堆栈的同步递归模型一旦执行便不能停止
>
> 而基于链表异步遍历模型可以随时停止在某个节点，并随时从该节点继续遍历

在渲染阶段，React 需要遍历整棵 fiber 树来执行渲染工作（state/props 更新、声明周期函数调用，diff 等），如果采用传统递归遍历方式，一旦开始就只能等待递归自己结束，整个渲染工作可能耗时超过 16ms，导致页面卡顿。

浏览器提供了一个`requestIdleCallback()` API，浏览器会在空闲时调用回调，告诉使用者剩余空闲时间

```typescript
requestIdleCallback((deadline) => {
	console.log(deadline.timeRemaining(), deadline.didTimeout)
});
```

如果能够在这段空闲时间内做一部分渲染工作，然后在下一次空闲时接着上次的工作继续，是不是就能避免递归的弊端了！**为了使用这个 API，需要一种将渲染工作分解为增量单元的方法**

链表中的节点有三个指针：

- child ---- 第一个子节点，如果没有子节点为 null
- sibling ---- 第一个兄弟节点，如果没有兄弟为 null
- return ---- 父节点，根节点为 null

遍历步骤如下：

```typescript
function walk(o) {
  // 根节点
  const root = o;
  // 当前遍历到的节点
  let current = o;

  while (true) {
    // 使用doWork，连接根节点和子节点，并返回根节点的第一个子节点
    const child = doWork(current);

    // 如果有子节点，将当前的指针指向子节点，并进入下一个循环（深度优先）
    if (child) {
      current = child;
      continue;
    }

    // 如果当前指针等于根节点，结束遍历
    if (current === root) {
      return;
    }

    // 如果没有兄弟，进入while循环
    while (!current.sibling) {
      // 如果当前指针等于根节点，结束遍历
      if (!current.return || current.return === root) {
        return;
      }
      // 如果没有子节点，并且没有兄弟节点，返回父级的节点
      current = current.return;
    }
    // 如果有兄弟节点，将当前指针设置为兄弟节点，进入下一次迭代
    current = current.sibling;
  }
}
```

// TODO

