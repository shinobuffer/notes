### 节流 throttle 和防抖 debounce

> 都是用来限制高频调用的手段

**节流 throttle：**持续触发并不会执行多次，在一段时间内，只执行一次。适用场景如滚动检查/加载

**防抖 debounce：**距离下次触发时间间隔过短的，不予执行；触发后在指定时长内无下一次触发才会予以执行。适用场景如输入校验/处理

TS实现见这里

### Promise/A+

【1】**Promise 初始化函数是立即同步执行的**

【2】**Promise 的三个状态， `pending`、 `fulfilled` 和 `rejected`**

-  `pending` 状态可以切换到 `fulfilled` 或 `rejected` 状态，并产生`result`
- 在 `fulfilled` 或者 `rejected` 状态，都不能迁移到其它状态

【3】Promise Thenable（PromiseLike）

- `Promise` 实例必须包含一个 `then` 方法，`then` 方法必须返回一个`Promise`实例
- `then` 方法可以接受两个可选参数，`onFulfilled`, `onRejected` , `onFulfilled` 和 `onRejected` 如果是函数，必须最多执行一次
- `then` 方法可以链式调用，每次注册一组 `onFulfilled` 和 `onRejected` 的 `callback`。它们如果被调用，必须按照注册顺序调用

【4】



