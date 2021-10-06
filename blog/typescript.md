## 基本类型

布尔值`boolean`、数字`number`、字符串`string`

数组`T[]`/`Array<T>`

元组`[T1, T2]`，表示一个已知元素数量和类型的数组。访问一个已知索引的元素，会得到正确的类型；**访问一个越界的元素，会使用联合类型替代**

任意类型`any`，可以忽略编译阶段的检查，尽量少用，**变量类型能确定尽量就确定**

没有任意类型`void`，通常用于返回值类型表示无返回值

`undefined`和`null`各有自己的同名类型，默认情况下**`null`和`undefined`是所有类型的子类型**

非原始类型`object`，除`number`，`string`，`boolean`，`symbol`，`null`或`undefined`之外的类型

`never`和`unknown`

- `never`取值范围为空集的类型，如不会有返回值/抛出错误的函数，`never`类型是任何类型的子类型，反之不是
- `unknown`不确定的类型（全集），在不确定类型之前不能作赋值操作

使用`T1 as T2`将类型T1断言为类型T2



## 枚举

数字枚举，枚举成员默认从零开始自增，**遇到用户指定值则从该值开始自增**

```typescript
enum Direction {
  Up,
  Down,
  Left,
  Right,
} // Up Down Left Right 分别为 0 1 2 3

enum Direction {
  Up,
  Down,
  Left = 10,
  Right,
} // Up Down Left Right 分别为 0 1 10 11
```

**数字枚举存在反向映射**，比如在上面代码中有`Direction[0]==='Up'`

------

字符串枚举，每个枚举成员都要设置为一个字符串或者另外一个字符串枚举成员

```typescript
enum Direction {
  Up = 'UP',
  Down = 'DOWN',
  Left = 'LEFT',
  Right = 'RIGHT',
} // Up Down Left Right 分别为 'UP' 'DOWN' 'LEFT' 'RIGHT'
```

当枚举的所有成员都是常量（自增、数字、字符串）时，该枚举的所有成员成为一种类型

```typescript
enum Shape {
  Circle,
  Square,
}
interface Circle {
  kind: Shape.Circle; // 枚举成员作为类型
  radius: number;
}
```



## 接口

接口可以用于描述对象、函数的“形状”

```typescript
// 描述对象类型
interface Person {
  readonly name: string; // 只读属性，对象初始化后不能修改
  sex: number;
  age?: number; // 可选属性
  [x: string]: any; // 索引签名，sex、age的类型需要兼容该类型
}
const person: Person = { name: 'A', sex: 1 }

// 描述函数类型
interface SearchFun {
  (source: string, subString: string): boolean;
}
const serachFun: SearchFun = (src: string, subString: string) => src.search(subString) > -1
```

接口可以限制类，**实现接口的类需要实现接口中的所有方法**。当一个类实现了一个接口时，**只对其实例部分进行类型检查**。constructor属于类的静态部分，不在检查的范围内

```typescript
interface ClockInterface {
  currentTime: Date;
  setTime(d: Date);
}

class Clock implements ClockInterface {
  currentTime: Date;
  setTime(d: Date) {
  	this.currentTime = d;
  }
  constructor(h: number, m: number) { }
}
```

接口可以相互继承，**一个接口可以继承多个接口**，从而扩展“形状”

```typescript
interface Shape {
  color: string;
}
interface Square extends Shape {
  sideLength: number;
}
const square: Square = { color: 'red', sideLength:10 }
```



## 类

类的成员可以设置以下访问修饰符

- `public`-无修饰符的成员默认是`public`的，即可以任意访问
- `private`-私有成员，只能在本类中访问
- `protected`-只能在本类和派生内中访问
- `readonly`-属性只读，只读属性必须在声明时或构造函数里被初始化

构造函数也可以被标记成`protected`，这意味着这个类不能在包含它的类外被实例化，但是能被继承

只带有`get`不带有`set`的存取器自动被推断为`readonly`



使用`abstract`关键字定义抽象类和抽象方法，抽象类不能实例化。**抽象类可以包含具体方法，也可以定义抽象方法**，抽象类中的抽象方法必须在派生类中实现

```typescript
abstract class Animal {
  abstract makeSound(): void;
  move(): void {
    console.log('moving');
  }
}

class Cat extends Animal {
  makeSound() {
    console.log('nya')
  }
}
```

对类使用`typeof`会取得类的构造函数的类型，这个类型包含了类的所有静态成员和构造函数

```typescript
class People {
  constructor(private name: string) {}
}

const peopleMaker: typeof People = People;
const people = new peopleMaker('A');
```

类本身也是一种类型，**子类变量可以赋值给父类变量**



## 函数

完整的函数类型包含两部分：参数类型和返回值类型。赋值语句的一边指定了类型但是另一边没有类型的话，会自推断出缺失的类型

```typescript
const add: (a: number, b: number) => number = (a, b) => a+b
const add = (a: number, b: number): number => a+b

// 经过推断，上面两个表达式都相当于下面这个完整表达式
const add: (a: number, b: number) => number = (a: number, b: number): number => a+b
```

函数可选参数必须放在参数列表的最后；**默认参数是可选参数的一个特例，可以不放在最后**（通过传入`undefined`来激活使用默认值）；剩余参数必须放在参数列表的最后，是一个数组类型

```typescript
function concatName(n1='Bersder', n2?: string) {
  return n2 ? n1 + n2 : n1;
}

function getName(n1: string, ...ns: string[]) {
  return `${n1} ${ns.join(' ')}`;
}
```

如果非箭头函数内使用了`this`，在参数列表的最前显式地提供一个`this`虚参数并指定其类型

```typescript
const Obj = {
  vars: ['1', '2'],
  fun: function(this: { vars: string[] }) {
  	console.log(this.vars)
	}
}
Obj.fun()
```

可以通过**定义多个函数类型来实现函数重载**（函数不同入参对应不同出参）,编译器检查时按序尝试重载列表，因此定义重载的时候，一定要把最**精确的定义放在最前面**

```typescript
function random(): number; // 重载1
function random<T>(array: T[]): T; // 重载2
function random(x): any {
  if (typeof x === 'number')
    return Math.random()
  else if (Array.isArray(x))
    return x[Math.floor(Math.random() * x.length)]
}
```



## 泛型

泛型可以用在函数上，在使用泛型函数时指定泛型或者让编译器自己进行类型推断

```typescript
function initArray<T>(placeholder: T, num: number): T[] {
  return new Array(num).fill(placeholder);
}

const strArr = initArray<string>('0', 10);
const numArr = initArray(0, 10); // 自动推断T为number
```

泛型也能用在接口和类上，但泛型类指的是实例部分的类型，类的静态属性不能使用泛型类型

```typescript
interface Dict<K, V> {
  [x: K]: V;
}
const dict: Dict<string, boolean> = { k1: true }

class MyArray<T> {
  arr: T[];
  constructor(...items: T[]) {
    this.arr = items;
  }  
}
```

**泛型使用`extends`进行约束**

```typescript
function getLen<T extends { length: number }>(arg: T) {
  return arg.length;
}

console.log(getLen([])) // 0
console.log(getLen(0)) // error，整形变量无length属性
```



## 类型的兼容[有点乱]

TypeScript结构化类型系统的基本规则是，如果`x`要兼容`y`，那么`y`至少具有与`x`相同的属性（`y`包含`x`），可以简记为**少的兼容多的，多的可以赋值给少的**，如

```typescript
interface Named {
  name: string;
}

let x: Named;
// 注意这里直接x = { name: 'Alice', location: 'Seattle' }是不行的
// 需要让y得到{ name: string; location: string; }的类型
let y = { name: 'Alice', location: 'Seattle' };
x = y;
```

函数的兼容较复杂，返回值一致的情况下**参数少的可以赋值给多的**；参数一致的情况下返回值适用上面提到的规则

```typescript
let x = (a: number) => 0;
let y = (a: number, b: string) => 0;
y = x; // OK
x = y; // Error

let x = () => ({name: 'Alice'});
let y = () => ({name: 'Alice', location: 'Seattle'});

x = y; // OK
y = x; // Error
```

函数的可选参数在兼容时可以不参与检查，但一旦参与检查，类型需要对应才能兼容；剩余参数相当于不定个可选参数

```typescript
let x = (a: number) => 0;
let y = (a: number, b: number) => 0;
let z = (a: number, b?: string) => 0;
z = x; // OK
z = y; // Error
```

数字枚举类型与数字类型相互兼容，但不同枚举间互不兼容

```typescript
enum Status { Ready, Waiting };
enum Color { Red, Blue, Green };

let status = Status.Ready;
status = Color.Green;  // Error
```



## 高级类型

交叉类型是将多个类型合并为一个类型，**相当于取并集**，使用`&`来交叉多个类型

```typescript
interface A {
  name: string;
}
interface B {
  age: number;
}
type C = A & B // 相当于C里有name和age
```

联合类型表示实际类型可能是一堆类型中的一个，**相当于多选一**，使用`|`联合多个类型，**默认情况下只能访问此联合类型的所有类型里共有的成员**

```typescript
function getLen(arg: any[] | string) {
  return arg.length // length是数组和string共有成员可以访问
}
```

如果想让联合类型收敛为其中一个成员类型并使用其独有方法，使用`typeof`或`instanceof`进行收敛

```typescript
function padLeft(value: string, padding: string | number) {
    if (typeof padding === 'number') {
        return Array(padding + 1).join(" ") + value;
    } else if (typeof padding === 'string') {
        return padding + value;
    }
    throw new Error(`Expected string or number, got '${padding}'.`);
}

class Fish {
  swim(){ }
}
class Bird {
  fly() { }
}
function move (obj: Fish | Bird) {
  if (obj instanceof Fish) {
    obj.siwm()
  }else if (obj instanceof Bird) {
    obj.fly()
  }
}
```

使用`type`来定义类型别名，其实相当于替换，它能描述接口无法描述的类型如元组、交叉类型、联合类型等，但不能像接口一样被`implement`或`extends`。考虑到“对扩展开放，对修改封闭”的原则，**应该尽量使用接口代替类型别名**

```typescript
type Name = string;
type NameResolver = () => string;
type NameOrResolver = Name | NameResolver;
type LinkedList<T> = T & { next: LinkedList<T> };
```

字符串字面量也可以作为类型，字符串字面量类型配合联合类型、类型别名，可以实现类似字符串枚举的功能

```typescript
type Easing = 'ease-in' | 'ease-out' | 'ease-in-out';
enum EasingEnum {
  EaseIn = 'ease-in',
  EaseOut = 'ease-out',
  EaseInOut = 'ease-in-out',
}
const e1: Easing = 'ease-in';
const e2: EasingEnum = EasingEnum.EaseIn;
```

当联合类型的成员都有一个属性唯一表示该类型时，称这种联合为可辨识联合，那个用于区分的属性称为可辨识的属性，编译器可以通过区分该属性来收敛类型

```typescript
interface Square {
  kind: "square"; // kind为可辨识的属性
  size: number;
}
interface Rectangle {
  kind: "rectangle";
  width: number;
  height: number;
}
interface Circle {
  kind: "circle";
  radius: number;
}
type Shape = Square | Rectangle | Circle;
function area(s: Shape) {
  switch (s.kind) {
    case "square": return s.size * s.size;
    case "rectangle": return s.height * s.width;
    case "circle": return Math.PI * s.radius ** 2;
  }
}
```

索引类型相关的两个操作符：

- `keyof T`-得到`T`上已知的公共属性名的联合
- `T[K]`-前提`K extends keyof T`，得到`T[K]`的类型

```typescript
const person = {
  name: 'A',
  age: 10,
};
function getProperty<T, K extends keyof T>(o: T, name: K): T[K] {
  return o[name];
}
const name: string = getProperty(person, 'name');
const age: number = getProperty(person, 'age');
```

映射类型，即将原类型上的属性都以相同的方式映射后得到的新类型。映射的方式可以自己写，也可以使用TS提供的内置映射，映射时使用`in`关键字来属性迭代

```typescript
type Keys = 'nameChecked' | 'ageChecked';
type Flags = { [K in Keys]: boolean }; 
// 自定义映射后得到类型 { nameChecked: boolean, ageChecked: boolean }
```

TS提供的几个内置映射类型：

- `Partial<T>`-将T中所有属性全改为可选属性
- `Required<T>`-将T中所有属性全改为必选属性
- `Readonly<T>`-将T中所有属性全改为只读属性
- `Pick<T, K extends keyof T>`-选取T中部分属性
- `Record<K extends keyof any, T>`-`[P in K]: T`

TS提供的几个条件类型：

- `Exclude<T, U>`-从`T`中剔除可以赋值给`U`的类型
- `Extract<T, U>`-提取`T`中可以赋值给`U`的类型
- `NonNullable<T>`-从`T`中剔除`null`和`undefined`
- `ReturnType<T>`-获取函数类型`T`返回值类型
- `InstanceType<T>`-获取构造函数类型`T`的实例类型



## 模块

TS兼容ES6的模块语法，而且增加了类型别名和接口的导出和导入。如果使用了模块语法，就不要使用命名空间了

为了兼容CommonJS和AMD中的`exports`，TS提供了`export =`语法，相当于ES6中的默认导出。若使用`export =`导出一个模块，则必须使用特定语法`import module = require("module")`来导入此模块

```typescript
// 被引用模块 Validator.ts
export = class Validator {};

// index.ts
import Validator = require('./Validator');
```



## 装饰器

> 装饰器是一项实验性特性，需在`tsconfig.json`里启用`experimentalDecorators`

装饰器是一种特殊类型的声明，它能够被附加到类声明，方法，访问符，属性或参数上，用来修改原代码的行为。

装饰器使用`@decorator`的形式，装饰器本质是一个函数，它会在运行时被调用，被装饰的声明信息做为参数传入。**如果修饰器需要传参，修饰器需要返回一个函数作为真正的修饰器**。多个修饰器可以作用在一个目标上

```typescript
function t1(target) {
  console.log('无参修饰器');
}
function t2(...args: any[]) {
  return function (target) { // 真正的修饰器
    console.log('带参修饰器', args);
  }
}

@t1
@t2('2333')
class Test {}
```

TS中装饰器可以修饰五种目标：类，属性，访问器，方法以及方法参数

### 类装饰器

 类装饰器应用于类构造函数，类的构造函数作为装饰器的唯一参数。如果类装饰器返回一个值，这个值会作为新的构造函数替换被修饰类的构造函数（注意处理原型链）

```typescript
function sealed(constructor: Function) {
  Object.seal(constructor);
  Object.seal(constructor.prototype);
}

@sealed
class Greeter {
  greeting: string;
  constructor(message: string) {
    this.greeting = message;
  }
  greet() {
    return this.greeting;
  }
}
```

### 方法装饰器

方法装饰器应用到方法的属性描述符上，可以用来监视，修改或者替换方法定义。方法装饰器会在运行时传入下列3个参数：

1. 对于静态成员来说是类的构造函数，对于实例成员是类的原型对象
2. 成员的名字
3. 成员的属性描述符

```typescript
function enumerable(value: boolean) {
  return function (target: any, propertyKey: string, descriptor: PropertyDescriptor) {
    descriptor.enumerable = value;
  };
}

class Greeter {
  greeting: string;
  constructor(message: string) {
    this.greeting = message;
  }
  @enumerable(false)
  greet() {
    return this.greeting;
  }
}
```

### 访问器装饰器

访问器装饰器应用于访问器的属性描述符，可以用来监视，修改或替换一个访问器的定义。访问器装饰器会在运行时传入下列3个参数：

1. 对于静态成员来说是类的构造函数，对于实例成员是类的原型对象
2. 成员的名字
3. 成员的属性描述符

注意：**不允许同时装饰一个成员的`get`和`set`访问器**，因为装饰器应用于一个属性描述符时，它联合了`get`和`set`访问器

### 属性装饰器

属性装饰器会在运行时传入下列2个参数：

1. 对于静态成员来说是类的构造函数，对于实例成员是类的原型对象
2. 成员的名字

```typescript
function DefaultValue(value: string) {
  return function (target: any, propertyName: string) {
    target[propertyName] = value;
  }
}

class Hello {
  @DefaultValue("world") greeting: string;
}
```

### 参数装饰器

 参数装饰器应用于类构造函数或方法声明，参数装饰器会在运行时传入下列3个参数：

1. 对于静态成员来说是类的构造函数，对于实例成员是类的原型对象
2. 成员的名字
3. 参数在函数参数列表中的索引



## 声明文件

TS中声明的实体有三类：命名空间、类型、值（**一个实体可以拥有多种身份**）

- 命名空间：一个命名空间内可以包含值或类型
- 类型：可以作为冒号右边的部分
- 值：在编译为JS后唯一会被保留的东西

实体及类别对应关系如下：

- **命名空间 (namespace, module)**: 命名空间, 值（命名空间一旦包含值的声明，它就转变为值）
- **类 (class)**: 类型, 值
- **枚举 (enum)**: 类型, 值
- **接口 (interface)**: 类型
- **类别名 (type)**: 类型
- **函数 (function)**: 值
- **变量 (let, const, var, parameters)**: 值

使用`declare var/const`声明变量；`declare function`声明函数；`declare namespace`声明带属性对象（使用点访问）；`declare class`声明类；`declare module`声明模块；`interface`接口和`type`别名在声明文件中正常使用

```typescript
declare module 'react' {
  // 声明模块
  export interface Component {/* ... */}
}

// 变量声明
declare var v1: string;
// 函数声明（可多次声明来函数重载）
declare function log(msg: string): void;
// 对象声明
declare namespace myLib {
  function f1(v1: string): string;
  let v1: string;
}
// 类声明
declare class Greeter {
  // 类构造器声明
  constructor(greeting: string);
  // 类实力属性方法声明
  greeting: string;
  showGreeting(): void;
}
```

据我观察，编写`declare`的作用有二：

- 为自己编写的库/第三方库提供TS支持，获得代码提示和补全功能
- 为全局/三方库中的扩展变量提供类型支持，解决TS报错的同时获得代码提示和补全功能

```typescript
// 为window对象中的自定义变量提供类型支持
declare global {
  interface Window {
    isSSR: boolean
  }
}
```



## tsconfig

```json
{
  "compileOptions":{
    "target": "ES6", // 编译目标
    "module": "commonjs", // 模块化规范
    "lib": ["dom","es2016"], // 需要用到的库
    "outdir": "./dist", // 编译输出目录
    "allowJs": false, // 默认不对js文件编译
    "checkJs": false, // 默认不对js文件检查
    "removeComments": false, // 编译输出默认不消除注释
    "noEmitOnError": false, // 默认编译不通过也输出结果
    "strict": false, // 严格检查总开关，默认关闭
    "alwaysStrict": false, // 默认不启用严格模式
    "noImplicitAny": false, // 默认允许隐式any
    "noImplicitThis": false, // 默认不强制this类型确定
    "strictNullChecks": false, // 默认不强制空安全
    // 路径别名（配合webpack）
    "paths": {
      "@src": ["src/*"]
    },
    // 声明文件
    "types": [
      "./typings/global"
      "jest",
      "node"
    ]
  },
  "include": ["./src/**/*"],
  "exclude": ["node_modules"],
}
```

