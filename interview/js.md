### JS-原理

> 广义闭包：**当函数可以记住并访问所在的词法作用域时，就产生了闭包**，即使函数是在当前词法作用域之外执行的
>
> 狭义闭包：**有权访问另一个函数作用域中变量的函数**

#### 事件循环

### JS-实现

#### new 操作符

> new 操作符用于创建给定构造函数的实例对象

- 创建一个新的对象`obj`
- 将对象与构造函数通过原型链连接起来
- 将构造函数的`this`绑定到新建的对象`obj`上
- 根据构造函数返回值类型返回 new 结果
  - 如果构造函数返回原始类型（非对象类型），忽略其返回值，返回`obj`作为 new 的结果
  - 如果返回对象类型，将其代替`obj`返回作为 new 的结果

`prototype`是**只有函数类型对象（非箭头函数）拥的显式原型属性**，指向其原型对象；而原型对象中的`constructor`指向其对应的构造函数
`__proto__`是**每个对象都有的隐式原型属性**，指向了创建该对象的构造函数的原型对象

具体实现见 [new 操作符实现](https://github.com/Bersder/leetcode/blob/master/src/base-structure/implementation/new.ts)

#### 节流 throttle 和防抖 debounce

> 都是用来**限制高频调用**的手段

**节流 throttle：**持续触发并不会执行多次，在一段时间内，只执行一次。适用场景如滚动检查/加载

- 实现思路：记录上次触发时刻，如果本次触发时刻距离上次触发时刻超过阈值时间，执行一次

**防抖 debounce：**距离下次触发时间间隔过短的，不予执行；触发后在指定时长内无下一次触发才会予以执行。适用场景如输入校验/处理

- 实现思路：记录一个 timer，每来一次触发就清除上次的 timer，用新的 timer 替代

具体实现见 [throttle 实现](https://github.com/Bersder/leetcode/blob/master/src/base-structure/implementation/throttle.ts)、[debounce 实现](https://github.com/Bersder/leetcode/blob/master/src/base-structure/implementation/debounce.ts)

#### apply/call/bind
> 三个函数都是`Function`的原型方法，**能改变非箭头函数执行时`this`的指向**
>
> `apply(thisArg, args)`临时改变`this`的指向并执行函数，**参数以参数数组的形式传入**
>
> `call(thisArg, arg1,...)`临时改变`this`的指向并执行函数，**参数以参数序列的形式传入**
>
> `bind(thisArg, arg1,...)`返回一个绑定了`this`指向和参数的函数，返回的函数还可以继续传入额外参数

具体实现见 [apply/call/bind 实现](https://github.com/Bersder/leetcode/blob/master/src/base-structure/implementation/apply-call-bind.ts)

#### Promise/A+

【1】**Promise 初始化函数是立即同步执行的**

【2】**Promise 的三个状态， `pending`、 `fulfilled` 和 `rejected`**

-  `pending` 状态可以切换到 `fulfilled` 或 `rejected` 状态，并产生`result`
- 在 `fulfilled` 或者 `rejected` 状态，都不能迁移到其它状态

【3】Promise Thenable（PromiseLike）

- `Promise` 实例必须包含一个 `then` 方法，`then` 方法必须返回一个`Promise`实例
- `then` 方法可以接受两个可选参数，`onFulfilled`, `onRejected` , `onFulfilled` 和 `onRejected` 如果是函数，必须最多执行一次
- `then` 方法可以链式调用，每次注册一组 `onFulfilled` 和 `onRejected` 的 `callback`。它们如果被调用，必须按照注册顺序调用

【4】



