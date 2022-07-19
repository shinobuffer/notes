### JS-原理

> 广义闭包：**当函数可以记住并访问所在的词法作用域时，就产生了闭包**，即使函数是在当前词法作用域之外执行的
>
> 狭义闭包：**有权访问另一个函数作用域中变量的函数**

#### 原型链

> JS 里一切皆对象，每个对象都有对应的原型

当访问一个对象的属性时，首先尝试在该对象上搜寻，如果不存在则搜寻对象的原型、对象的原型的原型，不断向上搜寻直至找到目标属性或到达原型链尾（找不到）

`__proto__`是**每个对象都有的隐式原型属性**，**指向了创建该对象的构造函数的原型对象**，保证实例对象能够访问在构造函数原型中定义的属性和方法。原型链上的对象靠`__proto__`属性连接在一起的

`prototype`是**只有函数类型对象（非箭头函数）才有的显式原型属性**，指向其原型对象；而原型对象中的`constructor`属性回指向其对应的构造函数，构造函数的原型对象定义了所有实例共享的属性和方法

- 一切对象都是继承自`Object`对象，`Object` 对象直接继承根源对象`null`
- 一切的函数对象（包括 `Object` 对象），都是继承自 `Function` 对象
- `Object` 对象直接继承自 `Function` 对象
- `Function`对象的`__proto__`会指向自己的原型对象，最终还是继承自`Object`对象

 [instanceof 关键字实现](https://github.com/Bersder/leetcode/blob/master/src/implementation/instanceof.ts)

#### 执行上下文和执行栈

> 执行上下文：一种对代码执行环境的抽象描述
>
> 执行栈：以 FIFO 的形式存放创建的执行上下文，对应着函数的调用栈

执行上下文的分类

- 全局执行上下文：只有一个，会在顶层全局代码执行时创建
- 函数执行上下文：可以多个，会在函数调用时创建
- eval 函数执行上下文

执行上下文的生命周期包括三个阶段：创建阶段 → 执行阶段 → 回收阶段

- 【创建阶段】函数调用但内部代码未执行，此时解析内部代码的声明语句，创建一个上下文放入调用栈中
  - 绑定 this
  - 创建词法环境（通过`let/const/try-catch`创建的变量放这里，初始化为`uninitialized`；函数入参和`argument`也会放在此处）
  - 创建变量环境（通过`var`声明或`function(){}`声明的变量/函数放这里，分别初始化为`undefined`和函数引用，**是变量提升的原因**）
  - 词法环境和变量环境都记录着外部环境的引用，形成作用域链
- 【执行阶段】执行函数内部代码
- 【回收阶段】执行上下文出栈

#### 事件循环

> 事件循环：一种设计模式，又称消息分发器，通过接收和分发不同类型的消息，让事件调度更加合理
>
>  JS 是单线程的，每个线程都对应着一个事件循环，事件循环通过引入任务队列来实现异步任务非阻塞的执行

一个线程中，事件循环是唯一的，但是任务队列可以拥有多个，**不同任务源的任务会进入到不同的任务队列**。任务源可以分成两大类：宏任务（macro-task/task）和微任务（micro-task/job）

- 宏任务：script、setTimeout/setInterval、IO、UI渲染/交互事件
- 微任务：process.nextTick、Promise.then

【PS1】任务队列并不是真正的队列，而是任务的集合，**可执行的任务优先出队**

【PS2】同为一大类的不同任务源存在优先级之分，比如同为【微任务】的 process.nextTick 和 Promise，在都可以执行时，前者优先级更高 

___

事件循环从宏任务开始，初始时，宏任务中只有整体代码 script，全局上下文进入函数调用栈开始执行 script，并把执行过程中遇到的异步任务按照任务源区分放到对应的任务队列中。script 执行完成后，第一次循环的宏任务执行完成，开始从微任务集合中提取所有可执行的微任务放入函数调用栈中执行，同样把执行过程中遇到的异步任务按照任务源区分放到对应的任务队列中。至此第一次循环的微任务执行完成，按照先宏任务，后微任务的顺序开始下一个循环

<img src="js.assets/6e80e5e0-7cb8-11eb-85f6-6fac77c0c9b3.png" alt="img" style="zoom:50%;" />

#### 垃圾回收

> 一个对象如果能以某种方式访问到，说明对象是可达的，对于不可达的对象，需要回收其空间

目前主流 JS 引擎采用的垃圾回收算法基于**【标记清除】**，算法分标记和清除两个阶段

- 【标记】执行垃圾回收前所有变量都被标记为垃圾，然后从各个根对象出发开始遍历把可达的对象标记为非垃圾
- 【清除】对于所有标记为非垃圾的对象，销毁并回收其占用空间

标记清除的缺点是其清除的对象在内存空间上是不连续的，会产生大量内存碎片，影响内存的分配

对于这个问题，可以在清除阶段做一些改进，在完成所有对象的标记后，可以先把非垃圾对象在内存空间上向一边移动弄成连续的，剩余的连续区域自然就是待回收的大块连续内存，提升了后续内存分配的效率。这种基于标记清除改进的算法称为**【标记整理】**

更早之前则 JS 引擎还采用过基于【引用计数】的算法，该算法通过统计每个对象被引用的次数来决定回收哪些对象（引用次数为0的对象会被回收），该算法会因为循环引用而无法回收某些对象，因此被弃用了

___

> **代际假说**：大多数对象是临时使用的；而对于已经长期使用的对象，后面很可能会继续使用

V8 引擎采用的是**【分代式垃圾回收】**（同样基于【标记整理】），在该机制下，堆内存被划分为新生代和老生代两个区域，新老生代的垃圾回收分别独立

- 【新生代】存活时间较短的对象放在这里，通常是新创建的对象
- 【老生代】存活时间较长/空间占用大的对象放这里，通常是在新生代的垃圾回收中多次幸存对象

<img src="js.assets/aa2d5ad1d89b4b7b919f20e4a5f8973a~tplv-k3u1fbpfcp-zoom-in-crop-mark:3024:0:0:0.webp" alt="aa2d5ad1d89b4b7b919f20e4a5f8973a~tplv-k3u1fbpfcp-zoom-in-crop-mark:3024:0:0:0" style="zoom:50%;" />

【新生代垃圾回收】新生代同样被划分为【使用区】、【空闲区】两个区域。新创建的对象会放到使用区中，当使用区占用率到达阈值时，就会触发新生代的垃圾回收：

1. 对使用区的对象做标记，把存活的对象复制到空闲区中，并同时完成排序整理，相当于完成了一次【标记整理】
2. 使用区和空闲区角色互换

在新生代垃圾回收中多次存活的对象，会晋升到老生代中；一些占用空间较大的对象，也会直接分配到老生代中

【老生代垃圾回收】老生区是一个大内存区域，一次性回收的代价是很高的，所以通常采用增量标记、惰性增量清理的方案，并且启用多个辅助线程来并发标记和清理


### JS-实现

#### new 操作符

> new 操作符用于创建给定构造函数的实例对象

- 创建一个新的对象`obj`
- 将对象与构造函数通过原型链连接起来
- 将构造函数的`this`绑定到新建的对象`obj`上
- 根据构造函数返回值类型返回 new 结果
  - 如果构造函数返回原始类型（非对象类型），忽略其返回值，返回`obj`作为 new 的结果
  - 如果返回对象类型，将其代替`obj`返回作为 new 的结果

具体实现见 [new 操作符实现](https://github.com/Bersder/leetcode/blob/master/src/implementation/new.ts)

#### 继承

> 当我们说父类或子类时，我们指的是他们的构造函数

原型链继承：让子类原型指向父类实例【缺点：不能传参，父实例引用属性共享】

```typescript
Child.prototype = new Parent();
Child.prototype.constructor = Child;
```

构造函数继承：在子类使用`call()`调用父类【缺点：不能继承父类原型属性/方法】

```typescript
function Child(){
  Parent.call(this);
  // ...
}
```

组合继承：原型链继承 + 构造函数继承【缺点：调用两次了父类构造函数】

```typescript
function Child(){
  Parent.call(this); // 第二次调用，为了继承父类上的属性
  // ...
}
Child.prototype = new Parent(); // 第一次调用，为了继承父类原型上的属性/方法
Child.prototype.constructor = Child;
```

原型式继承/寄生式继承：借用`Object.create()`以父对象为原型创建子对象【缺点：父对象引用属性共享】

```typescript
const parent = {
  name: 'parent',
  tags: ['tag1', 'tag2']
  getName() {
    return this.name;
  }
};

const child = Object.create(parent);
// 以上是原型式继承

// 寄生式继承，在原型式继承的基础上进一步扩展子对象的属性/方法
child.changeName = function(newName) {
  this.name = newName;
}
```

寄生组合式继承：构造函数继承 + 寄生式继承（避免了组合继承的第一次调用）

```typescript
function Child(){
  Parent.call(this); // 通过构造函数继承让子类继承父类上的属性
  // ...
}

// 以父类原型为原型创建一个空对象，并让子类原型指向这个空对象，从而让子类实例能共享父类原型上的属性/方法
// Child.prototype = obj  obj.__proto__ = Parent.prototype
Child.prototype = Object.create(Parent.prototype);
Child.prototype.constructor = Child;
```

#### 节流 throttle 和防抖 debounce

> 都是用来**限制高频调用**的手段

**节流 throttle：**持续触发并不会执行多次，在一段时间内，只执行一次。适用场景如滚动检查/加载

- 实现思路：记录上次触发时刻，如果本次触发时刻距离上次触发时刻超过阈值时间，执行一次

**防抖 debounce：**距离下次触发时间间隔过短的，不予执行；触发后在指定时长内无下一次触发才会予以执行。适用场景如输入校验/处理

- 实现思路：记录一个 timer，每来一次触发就清除上次的 timer，用新的 timer 替代

具体实现见 [throttle 实现](https://github.com/Bersder/leetcode/blob/master/src/implementation/throttle.ts)、[debounce 实现](https://github.com/Bersder/leetcode/blob/master/src/implementation/debounce.ts)

#### apply/call/bind
> 三个函数都是`Function`的原型方法，**能改变非箭头函数执行时`this`的指向**
>
> `apply(thisArg, args)`临时改变`this`的指向并执行函数，**参数以参数数组的形式传入**
>
> `call(thisArg, arg1,...)`临时改变`this`的指向并执行函数，**参数以参数序列的形式传入**
>
> `bind(thisArg, arg1,...)`返回一个绑定了`this`指向和参数的函数，返回的函数还可以继续传入额外参数

具体实现见 [apply/call/bind 实现](https://github.com/Bersder/leetcode/blob/master/src/implementation/apply-call-bind.ts)

#### Promise/A+

【1】**Promise 初始化函数是立即同步执行的**

- 初始化函数接收由 Promise 提供的 resolve、reject 函数
- 如果初始化函数执行抛错，强制走 reject 逻辑，将错误作为 rejectReason

【2】**Promise 的三个状态： `pending`、 `fulfilled` 和 `rejected`**

- promise 被创建时初始状态为`pending`
- promise 被 resolve 后，状态切换到`fulfilled`，并将 resolve 值`value`记录
- promise 被 reject 后，状态切换到`rejected`，并将拒绝原因`reason`记录
- 状态只能切换一次，resolve 和 reject 执行互斥
- promise 状态发生变化后，需要异步按序执行`then`注册的回调

【3】**Promise Thenable**（PromiseLike）

- `Promise` 实例必须包含一个 `then` 方法，`then` 方法必须返回一个`Promise`实例
- `then` 方法接受两个可选回调函数：`onFulfilled`和 `onRejected` ，分别在当前 promise 被 resolve 和 reject 时被调用
- 同一个 promise，`then` 方法可以多次调用：
  - 如果当前 promise 状态为`pending`，将`onFulfilled` 和 `onRejected` 注册为回调等待状态变更时调用
  - 如果当前 promise 状态为`fulfilled`/`rejected`，直接走回调处理


【4】**回调处理**

- 根据当前 promise 的状态，决定走`fulfilled`路径，还是 `rejected`路径
- 如果回调中的 `onFulfilled`/`onRejected` 是函数，将当前 promise 的`value`/`reason`作为入参调用，以函数返回值作为下游 promise 的 resolveValue/rejectReason；如果不是函数，直接将当前 promise 的`value`/`reason`作为下游 promise 的 resolveValue/rejectReason
- 如果 `onFulfilled`/`onRejected` 执行过程中抛错，将错误作为下游 promise 的 rejectReason

【5】**resolve 处理**

- 如果 resolve 执行过程中捕捉到错误，promise 状态切换到`rejected`并将错误作为 rejectReason
- 不能 resolve 自身，否则抛出 `TypeError` 异常
- 如果 resolve 另外一个 promise，直接沿用该 promise 的`state`和`result`向下传递
- 如果 resolve 一个 thenable，取出其`then`方法，封装为新的 promise 向下传递
- 其他情况，promise 状态切换到`fulfilled`并将 resolve 的值作为 resolveValue

具体实现见 [Promise 实现](https://github.com/Bersder/leetcode/blob/master/src/implementation/promise.ts)

