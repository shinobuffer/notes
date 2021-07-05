## let & const
用于声明变量和常量，有以下特点：
- 块级作用域
- 无变量提升（需先声明后使用）
- 暂时性死区（作用域内声明变量之前不可用，就算有同名全局var变量）
- 同作用域内不允许重复声明
- `const`必须声明时同时赋值

`const`能保证基本类型的变量不能改变；但对于引用类型变量如数组，并不能阻止对数组增删元素，只能阻止变量指向的改变

在ES6中，浏览器中全局变量与顶层对象的属性脱钩，即全局变量 `let a` 不和 `window.a` 等价

## 字符串相关
扩展运算符支持中文分割，可以用于正确获取字符串长度。
增加了遍历接口，可以使用for-of进行遍历并支持中文遍历。
增加模板字符串如\`my name is \${name}\`，可以嵌入变量或表达式（自动调用其toString），可以实现跨行字符串。
增加标签模板：模板字符串可以紧跟在函数名之后，相当于调用该函数处理该模板字符串
```javascript
let a = 5;
let b = 10;

tag`Hello ${ a + b } world ${ a * b }`;
// tag是一个函数，上面的调用传参等同于
tag(['Hello ', ' world ', ''], 15, 50);
//被参数分割的字符串数组，和参数序列


//可以用来多语言转换
i18n`Welcome to ${siteName}, you are visitor number ${visitorNumber}!`
// "欢迎访问xxx，您是第xxxx位访问者！"
```
___
字符串新增实例方法
- `includes(str), startsWith(), endsWith()`判断目标字符串是否在原字符串中/开头/结尾
- `repeat(n)`返回原字符串重复n次的副本
- `padStart(n,str=' ')，padEnd()`字符串不够指定长度，在头部或尾部补全
- `trimStart()，trimEnd()`，参考`trim()`

## 正则相关
增加`u`修饰符，使用后，支持识别四字节字符（码点大于`0xFFFF`的Unicode字符），支持正则中使用大括号表示Unicode字符。正则对象中新添`unicode`属性表示是否设置该修饰符。

增加`y`修饰符，表示**粘连**，与`g`修饰符类似，但进行下一次匹配时要从剩余的第一个位置开始，不能跳过（隐含了头部匹配标志`^`）。正则对象中新添`sticky`属性表示是否设置该修饰符。

正则对象新增`flags`属性，返回修饰符字符串。

增加后行（否定）断言：
```javascript
let a = /(?<=y)x/ //x只有在y后面才匹配
let b = /(?<!y)x/ //x只有不在y后面才匹配
```
增加**具名组匹配**支持，允许为每个分组命名，可以在`exec`方法返回结果的`groups`属性上引用该组名
```javascript
const reg = /(?<year>\d{4})-(?<month>\d{2})-(?<day>\d{2})/;

const matchObj = reg.exec('1999-12-31');
const year = matchObj.groups.year; // 1999
const month = matchObj.groups.month; // 12
const day = matchObj.groups.day; // 31

//字符串替换时，使用$<组名>引用具名组
let re = /(?<year>\d{4})-(?<month>\d{2})-(?<day>\d{2})/u;

'2015-01-02'.replace(re, '$<day>/$<month>/$<year>')
// '02/01/2015'
```
字符串新增`matchAll(reg)`实例方法，返回遍历器，每个元素对应一次`exec`的结果。

## 数值相关
新增Number类方法：
- `Number.isFinite()`、`Number.isNaN()`、`Number.isInteger()`如名
- `Number.isSafeInteger()`判断数值是否在精确表示的范围间

Math对象扩展：
- `Math.trunc()`小数去尾，如果不是数值先转数值，无法处理返回`NaN`
- `Math.sign()`判断一个数到底是正数、负数、零（如果不是数值先转数值）。返回`1`、`-1`、`0`、`NaN`
- 新增指数运算符`**`，右结合

## 函数相关
允许形参设置默认值（通过传入`undefined`触发），允许形参配合解构赋值使用。
新增箭头函数，函数体内的`this`指向**定义时**所在的对象且不可改变，不可使用`arguments`和`yield`
允许`catch`命令省略

## 数组相关
新增扩展运算符`...`，++将数组转为内部元素的序列++，**非常好用**（拷贝、拼接、替代apply）。扩展运算符可以作用于实现了 Iterator 接口的对象，并将其展开成序列。

`Array.from`方法用于将两类对象转为真正的数组：类数组的对象（array-like object）和可遍历（iterable）的对象。`Array.from`的第二个参数是`map`函数，相当于转为数组之后的预处理函数
```javascript
let arrayLike = {
    '0': 'a',
    '1': 'b',
    '2': 'c',
    length: 3
};

let iterableSet = new Set(['a', 'b']);
//上面两个皆可以用Array.form转为数组
```
`Array.of`将参数序列转为数组。

数组实例新增方法：
- `find(fun[,obj])`找出第一个符合条件的数组成员并返回，第一个参数为返回布尔值回调函数，第二个参数绑定回调函数`this`对象
- `findIndex(fun[,obj])`，同上不过返回的是目标位置
- `fill(n[,start,end])`在指定位置用n填充数组，常用于初始化
- `entries(), keys() 和 values()`返回遍历器，对数组的键值对、键和值的遍历
- `includes(tar)`判断数组是否包含给定值
- `flat(n=1)`将多维数组展开n层并返回，跳过空位

## 对象相关
支持属性和方法的简写，支持属性名表达式，在字面量定义时使用中括号动态的定义键名
```javascript
let obj = {
  ['a'+'bc']:'I am abc',
  ['f'+'un'](){
    console.log('I am fun')
  }
}
```
对象的每个属性都有描述对象，用来控制属性行为。可以通过`Object.getOwnPropertyDescriptor`方法查看。描述对象的`enumerable`表示了该属性的可枚举性。不可遍历的属性会被`for...in`（会遍历继承的属性！）、`Object.keys`、`JSON.stringfy`、`Object.assign`忽略。

`Object.getOwnPropertyNames(obj)`返回一个数组，包含对象自身的所有属性（包括不可枚举的）。

新增`super`关键字，指向当前对象的原型对象，**只能用在简写对象方法中**。

对象也可以使用扩展运算符，非常好用（拷贝、拼接），但会**忽略继承的属性和不可遍历的属性**。

**链判断运算符**`?.`：通常获取对象属性时会判断该属性是否存在，该运算符能免去以往判断的代码。该运算符会**短路**，如果找不到属性就返回`undefined`
```javascript
let a = {
  aa:{},
  fun(){return 1}
}

let old = a && a.aa && a.aa.target || 'default'
let new = a?.aa?.target || 'defalut'
// 还能作用于方法，如果函数存在才执行，否则返回 undefined
let _ = a.fun?.()
```

**Null判断运算符**`??`：类似于`||`但只针对`null`和`undefined`放行
___
对象新增方法：
- `Object.is(o1,o2)`，与`===`相似，但是`+0`不等于`-0`，二是`NaN`等于自身
- `Object.assign(tar,objs...)`，用于对象的合并，将源对象**非继承可遍历**的属性浅拷贝到目标对象.取值函数先求值后拷贝，因此不能拷贝到get/set函数
- `Object.getOwnPropertyDescriptors()`返回指定对象所有自身属性（**非继承**）的描述对象，配合`Object.defineProperties()`可以实现正确拷贝
- `Object.setPrototypeOf()，Object.getPrototypeOf()`设置对象原型，获取对象原型
- `Object.keys()，Object.values()，Object.entries()`返回对象**非继承可遍历**的键、值、键值对数组
- `Object.fromEntries()`将一个键值对数组如`Map`转为对象

## Symbol
第七种数据类型，表示独一无二的值，不会和普通属性名冲突，避免被改写。通过`Symbol([key])`获取一个唯一值，可以选择传入描述参数。把 symbol 作为属性名时，不能使用点运算符只能使用中括号。

Symbol 类型可以用于定义一组常量，变量值唯一，变量名自身作为区分
```javascript
const COLOR = {
  RED:Symbol('red'),
  GREEN:Symbol('green')
}
```
`for...in`、`for...of`、`Object.keys()`、`Object.getOwnPropertyNames()`、`JSON.stringify()`会略过 Symbol 类型的键。`Object.getOwnPropertySymbols(obj)`可以获取 Symbol 类型的键集合。

如果需要使用同一 Symbol 值，使用`Symbol.for(key)`全局注册并登记一个 Symbol，以后可以通过key找回该 Symbol 值。使用`Symbol.keyFor(sym)`获取 Symbol 值的 key。

## Set & Map
`Set`表示集合类型/数据结构，其成员不会重复。`Set`可以选择接受一个可遍历的变量作为参数初始化。利用此特点可以用于数组去重（Set 内部使用类`===`进行比较）。
```javascript
let a = new Set([1,2,3,3,'3'])
let b = [...a]
//[1,2,3,'3'] 
```
Set 实例属性和方法有：
- `size`成员数
- `add(v)`添加成员，并返回 `Set` 本身（因此可以连续add）
- `delete(v)`删除成员，并返回布尔表示是否删除成功
- `has(v)`判断是否含有指定成员
- `clear()`清空
- `keys()，values()，entries()，forEach(fun)`

有了集合，可以实现并交差集
```javascript
// 并集
let union = new Set([...a, ...b]);
// Set {1, 2, 3, 4}

// 交集
let intersect = new Set([...a].filter(x => b.has(x)));
// set {2, 3}

// （a - b）差集
let difference = new Set([...a].filter(x => !b.has(x)));
// Set {1}
```
___
`Map`表示映射类型/数据结构，允许对象作为键（通过引用地址绑定）。`Map`可以选择接受一个键值对数组作为参数初始化。扩展运算符可以作用于`Map`得到键值对数组。
Map 实例属性和方法有：
- `size`成员数
- `set(k,v)`添加键值对，冲突覆盖，并返回 `Map` 本身
- `get(k)`根据键获取值，没有的话返回`undefined`
- `has(k)`判断是否包含某键
- `delete(v)`根据键删除值，并返回布尔表示是否删除成功
- `clear()`清空
- `keys()，values()，entries()，forEach(fun)`遍历顺序对应插入顺序

## Proxy
即**代理**，用于代理目标对象的某些操作，实际上重载了点运算符。
```javascript
//let proxy = new Proxy(target, handler);
let proxy = new Proxy({}, {
  get(target, key, receiver) {
    return 'always xxx';
  }
});
```
`Proxy`接受两个参数。第一个参数是所要代理的目标对象。第二个参数是一个配置对象，对于每一个被代理的操作，需要提供一个对应的处理函数用于拦截对应的操作。

Proxy 可以拦截很多操作，下面是一些常用的操作
- `get(target,key[,receiver])`拦截属性访问（可用于修饰返回），三个参数，依次为目标对象、键名和 proxy 实例本身（严格地说，是操作行为所针对的对象）
- `set(target,key,value[,receiver])`拦截属性修改（可用于合法验证），参数类似
- 更多自查

在 Proxy 代理的情况下，目标对象内部的this关键字会指向 Proxy 代理，因此不能获取到目标对象的实例属性。

## Promise
异步编程的一种解决方案，相当于一个容器，保存着某个未来才会结束的事件（通常是一个异步操作）的结果。`Promise`对象代表一个异步操作，有三种状态：`pending`、`resolved`和`rejected`。
```javascript
let p = new Promise(function(resolve, reject) {
  // ... some code

  {/* 异步操作成功 */
    resolve(value);
  } 
  {/* 异步操作失败 */
    reject(error);
  }
});
```
`Promise`构造函数接受一个函数作为参数，该函数的两个参数分别是`resolve`和`reject`，由 JavaScript 引擎提供。**Promise 新建后就会立即执行。**

`resolve`函数的作用是，将Promise对象的状态从从 pending 变为 resolved，在异步操作成功时调用，并将异步操作的结果作为参数传递出去；`reject`函数的作用是，将Promise对象的状态从从 pending 变为 rejected，在异步操作失败时调用，并将异步操作报出的错误作为参数传递出去。

`Promise`实例生成以后，可以用`then`方法分别指定`resolved`状态和`rejected`状态的回调函数。
```javascript
p.then(function(value) {
  // success
}, function(error) {
  // failure
});
```
`then`方法可以接受两个回调函数作为参数，第一个在异步操作成功时调用，第二个在异步操作失败时调用（可选），两个回调函数接收`Promise`对象传出的值作为参数。`then`方法默认返回一个新的`Promise`实例（可以通过return一个`Promise`实例覆盖它），因此可以链式调用。

`catch`方法指定发生错误时的回调函数。一般不会使用`then()`的第二个参数而是使用`catch`方法。链上上流的`promise`抛出的错误和reject都会被下流的`catch`方法捕获。

`finally`方法指定最终执行的回调函数，无论状态怎么变化都会执行
___
`Promise.all(p_array)`，接收一个 Promise 数组，将多个 Promise 实例，包装成一个新的 Promise 实例。当所有子 Promise 全`resolved`时父才会`resolved`；当任一子 Promise `rejected`时父就会`rejected`，调用回调函数。

`Promise.any(p_array)`当所有子 Promise 全`rejected`时父才会`rejected`；当任一子 Promise `resolved`时父就会`resolved`。

`Promise.race(p_array)`任一子 Promise 状态发生变化时父就会跟着变化。

`Promise.allSettled(p_array)`，等待所有子 Promise 状态变化后，父变为`resolved`。

`Promise.resolve()、Promise.reject()`将现有对象转为 Promise 对象并转为 resolve 或 rejected 状态。

## Iterator
Iterator（遍历器） 接口，为所有数据结构，提供了一种统一的访问机制，即`for...of`循环。实现了该接口的数据结构是可遍历的。

默认的 Iterator 接口部署在数据结构的`Symbol.iterator`属性，执行该属性，会返回一个遍历器对象。遍历器对象必含`next`方法，每次调用`next`方法，都会返回一个代表当前成员的信息对象，具有`value`和`done`两个属性。

遍历器对象可以选择设置`return`方法，该方法在遍历提前终止时调用（如break和出错）。

类数组的解构赋值、类数组的展开、`for...of`会调用遍历器接口。

## async & await
`async`函数返回一个 Promise 对象，表示函数包含异步操作。`async`函数内部`return`语句返回的值，会作为参数传给`then`方法。如果`async`函数抛出错误，会导致返回的 Promise 对象变成 rejected 状态，并作为参数传给`catch`方法。

`await`命令只能用在`async`函数中，通常放在 Promise 对象之前，表示等待异步操作的完成。如果该异步操作成功返回其 resolve 的参数；如果该异步操作失败（而自身也没有`catch`），`async`函数会**中断执行**并导致返回的 Promise 对象变成 rejected 状态（为了避免中断可以将`await`放在`try...catch`结构中）。

如果多个`await`的异步操作没有先后关系，可以用`Promise.all()`包装它们，同时触发。

## Class
使用`class`关键字定义类。类的所有方法都定义在类的`prototype`属性上面（且不可枚举）。类的内部可以使用`get`和`set`关键字，拦截属性的读写行为。
```javascript
class Point{
  z = 0;
  constructor(x, y) {
    this.x = x;
    this.y = y;
  }
  get x(){
    return x;
  }
  toString() {
    return `(${this.x},${this.y})`;
  }
}
```
类的`constructor`方法是类的构造函数，实例创建时调用，默认返回实例本身`this`，也可以指定返回其他对象。实例属性可以在构造函数中定义，也可以在类的顶层定义。

- 类可以用表达式定义`let Point = class{}`
- 类无变量提升，只能先定义后使用
- 类有一个`name`属性，表示类名

使用`static`关键字定义类的静态方法，**只能通过类调用**，静态方法中`this`指向类本身，静态方法可以被子类继承。
___
通过`extends`关键字实现继承，子类必须在`constructor`方法中**最先调用**`super`方法，否则无法创建实例。

`super`作为函数调用时，指向父类的构造函数（但还是返回子类实例），只能用在子类构造函数中。

`super`作为对象时，在普通方法中，指向父类的原型对象，通过`super`调用父类方法时，方法内部的`this`指向当前的子类实例；在静态方法中，指向父类，通过`super`调用父类静态方法时，静态方法内部的`this`指向当前的子类。

`extends`可以实现原生构造函数（如Array、Error）的继承。

## 模块
一个模块就是一个独立的文件，文件内部的变量，外部无法获取。如果需要对外开放变量的访问，需要用`export`关键字输出变量。`export`输出的变量是动态绑定的，变量在内部发生变化会反应到外部。
```javascript
export let a = 'a';
export let b = ()=>'b';
//上下两种写法等价，推荐下面写法
let a = 'a';
let b = ()=>'b';
export {a,b}
//可以用 as 起别名，import 同理
//export {a as aa,b}
```
`import`关键字用于从模块加载开放的变量，具有提升效果。
```javascript
import {a,b} from './m.js'
console.log(a)
```
`import`和`export`都是静态执行的，所以只能用在程序顶层。

`export default`关键字用于指定默认输出，只能用一次，其他模块要导入默认输出时**不使用大括号，可以任意起名**。
```javascript
let one = 1;
export default one;
//上输出 out.js，下输入 in.js
import o from 'out.js'
```
`export` 与 `import`可以复合使用实现模块继承，下面子模块完全继承并开放了父模块的所有变量，而且还开放了自己的变量。
```javascript
//son.js
export * from 'father.js';//不包含默认的
export let sonName = 'son';
```