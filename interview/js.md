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

【2】**Promise 的三个状态， `pending`、 `fulfilled` 和 `rejected`**

-  `pending` 状态可以切换到 `fulfilled` 或 `rejected` 状态，并产生`result`
- 在 `fulfilled` 或者 `rejected` 状态，都不能迁移到其它状态

【3】Promise Thenable（PromiseLike）

- `Promise` 实例必须包含一个 `then` 方法，`then` 方法必须返回一个`Promise`实例
- `then` 方法可以接受两个可选参数，`onFulfilled`, `onRejected` , `onFulfilled` 和 `onRejected` 如果是函数，必须最多执行一次
- `then` 方法可以链式调用，每次注册一组 `onFulfilled` 和 `onRejected` 的 `callback`。它们如果被调用，必须按照注册顺序调用

【4】



