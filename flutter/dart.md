> 适合有一定高级语言基础（特别是JS/TS）的同学食用

## Dart基础

### 变量

跟TS类似，Dart是强类型语言，存在类型推断和类型校验。除了使用具体的类型来声明变量，还可以使用`var`和`dynamic`声明变量
- `var`-类型通过推断得到
- `dynamic`-类型不做限制

如果需要常量可以使用`final`和`const`关键字（`const`隐式为`final`）
- `final`-可以先声明后初始化，**运行时常量**
- `const`-需要声明时同时初始化，**编译时常量**

内置的类型有`int,double,String,Boolean,List,Set,Map,Rune,Symbol`

`String`即字符串，使用**单/双引号创建单行字符串**，使用**三个单/双引号创建跨行字符串**，使用`r`前缀创建原始字符串（类似python），在字符串中使用`${expression}`的方式内嵌表达式，使用`+`连接字符串

`Boolean`即布尔值，只有`true`和`false`两个值，Dart是**类型安全**的，即**条件表达式必须是布尔值**而不能是`0/null`之类的值

`List`即数组，可以通过字面量`<T>[]`定义或`new List<T>()`定义，数组中的元素需要同类型

`Set`即集合（去重数组），可以通过字面量`<T>{}`定义或`new Set<T>()`定义，集合中的元素需要同类型

`Map`通过字面量`{}`定义或`Map<K,V>()`定义，**索引时必须使用中括号，默认情况下键值类型不做统一限制**

几个有点特殊的运算符
- `...`-展开可迭代变量
- `~/`-取整除法
- `?.`-如果对象的属性非空予以访问和修改
- `??=`-被赋值对象为空则执行赋值
- `??`-`A ?? B`相当于`A==null?B:A`
- `is`-类型判断如`'str' is String`



### 内置类型
| 类型                                                         | 常用属性                                         | 常用方法                                                     |
| ------------------------------------------------------------ | ------------------------------------------------ | ------------------------------------------------------------ |
| [List](https://api.dart.dev/stable/2.10.5/dart-core/List-class.html) | isEmpty/isNotEmpty, first/last, length, reversed | add/addAll, insert/insertAll,<br/>remove/removeAt, indexOf/indexWhere,<br/>join, removeLast, sort, sublist,<br/>fillRange/getRange/removeRange |
| [Set](https://api.dart.dev/stable/2.10.5/dart-core/Set-class.html) | 基本和List一样（没有reversed）                   | contains/containsAll<br/>union/difference/retainAll/retainWhere |
| [Map](https://api.dart.dev/stable/2.10.5/dart-core/Map-class.html) | isEmpty/isNotEmpty, keys/values                  | containsKey/containsValue, putIfAbsent<br/>addAll/addEntries, remove |

上面三种类型都有以下的遍历方法
- `forEach`-遍历（同JS）
- `map`-映射（同JS）
- `where/any/every`-相当于JS数组的filter/some/every方法




### 函数
在Dart中函数也是对象，对应类型`Function`，类似于JS它能赋值给变量或作为参数传递。一个最基本的函数需要包含以下要素，返回类型不强制标明，但为了可读性最好加上；缺省方法名的函数为匿名方法
```dart
/*
[修饰符、返回类型] [方法名]([参数]){
  //方法体
  //retrun
}

// 如果一个方法只有一个表达式可以用箭头语法简化
[修饰符、返回类型] 方法名([参数]) => 单句表达式
/*
```

可选参数包括位置参数和命名参数，他们都**需要放在最后面**：用**中括号包住参数表示位置参数**，用**大括号包住参数表示命名参数**；命名参数可以使用`@required`注释表示必选参数，调用时使用`:`对应；使用`=`指定默认参数，**默认参数只能配合可选参数使用**
```dart
void fun1(int a, [int b]) {}
void fun2(int a, {int b, @required int c}) {}
void fun3(int a, [int b=2]) {}
```
和JS一样Dart使用词法作用域，即变量的作用域在编写时就已经固定，以所在大括号为作用域；而闭包使用方式同JS

使用`typedef`关键字来定义一个函数的形状如`typedef Sort = bool Function(num a, num b)`，该形状可以作为类型限制函数变量



### 控制流集合

在数组中可以使用`if/else`语句来控制元素的有无，使用`for`来遍历元素（对标RN的JSX中的三元表达式和map），在编写UI时是非常方便的

```dart
Column(children:[
  if(renderWidget1)
  	Widget1(),
  else Widget2(),
  for(var i in [1,2,3]) Text('$i'),
]);
```



### 异常处理
使用`throw`来抛出异常和错误；使用`on`（能指定异常类别）或者`catch`都能捕获异常；使用关键字`rethrow`将异常重新抛出；使用`finally`指定最终执行的代码
```dart
try {
  breedMoreLlamas();
} on OutOfLlamasException {
  // 一个特殊的异常
  buyMoreLlamas();
} on Exception catch (e) {
  // 其他任何异常
  print('Unknown exception: $e');
} catch (e) {
  // 没有指定的类型，处理所有异常
  print('Something really unknown: $e');
} finally {
  print('end');
}
```



### 类

类的构造函数的名字可以是`ClassName`（只能一个）或者`ClassName.identifier`（命名构造函数，可多个）；除非和方法的参数冲突，方法中引用实例变量时可以省略`this`
```dart
class Point {
	num x, y;
  Point(int x, int y) {
    this.x = x;
    this.y = y;
  }
  //上面可以化简为
  //Point(this.x, this.y);
  Point.zero() {
    x = y = 0;
  }
}

Point p1 = new Point(1,1);
Point p2 = new Point.zero();
```
一个跟构造函数有密切关系的符号是冒号`:`，用于在继承场景下调用父构造函数（用`super`引用父类），也能在构造函数执行前初始化实例变量（初始化列表），**他们都无法访问`this`**
```dart
class Animal {
  String name;
  int sex;
  Animal(info) // 下面是初始化列表
    : name = info['name'],
    sex = info['sex'] {
    // 构造函数体
  }
}

class Dog extends Animal {
  // 调用父构造函数
  Dog(info) : super(info) {
    // 构造函数体
  }
}
```
使用`static`关键字来定义静态属性和方法，**静态方法中只能访问静态成员**；**以下划线开始来命名来定义私有属性和方法**。
使用`factory`关键字来定义工厂构造函数（执行构造函数并不总是创建这个类的一个新实例），**工厂构造函数只能访问静态属性**。

```dart
class Logger {
  final String name;
  bool mute = false;

  // _cache 是私有属性。
  static final Map<String, Logger> _cache =
      <String, Logger>{};

  factory Logger(String name) {
    if (_cache.containsKey(name)) {
      return _cache[name];
    } else {
      final logger = Logger._internal(name);
      _cache[name] = logger;
      return logger;
    }
  }

  Logger._internal(this.name);

  void log(String msg) {
    if (!mute) print(msg);
  }
}
```
使用`get`/`set`关键字来定义属性的getter和setter，getter为无参方法，setter带一个参数代表被赋值的值，他们都返回一个值用于最终的读写。

```dart
class Rectangle {
  num left, top, width, height;

  Rectangle(this.left, this.top, this.width, this.height);

  // 定义两个计算属性： right 和 bottom。
  get right => left + width;
  set right(num value) => left = value - width;
  get bottom => top + height;
  set bottom(num value) => top = value - height;
}
```

___

> 继承是一个 "是不是"的关系（适用于代码复用），而接口实现则是 "有没有"的关系（适用于行为约束）。

使用`extends`关键字来继承父类**（只能单继承）**，使用`implements`关键字来实现接口（**可多实现**）。在继承和实现的场景下如果需要实现或重写方法时建议使用`@override`来注释方法。

使用`abstract`关键词来定义抽象类，抽象类中的**抽象方法只有定义没有实现**，必须交由子类实现；但是**抽象类也可以有具体的实现**。

接口没有显式的定义，**每个类都隐式的定义了一个接口**，接口包含了该类所有的实例成员及其实现的接口，接口中的方法必须交由子类实现。

```dart
abstract class Dog {
  void eat();
  void sleep() {
    print('睡大觉');
  }
}

class IFurry {
  void removeFur(){};
}

class Alaska extends Dog implements IFurry{
  @override
  eat() {
    print('吃海豹');
  }
  
  @override
  removeFur() {
    print('脱毛');
  }

  void dragSledge() {
    print('拉雪橇');
  }
}
```

____

使用 `enum` 关键字定义一个枚举类型，枚举中的每个值都有一个`index`属性，代表该枚举值定义时的位置（从零开始），枚举的使用同

```dart
enum Color { Red, Green, Blue };
assert(Color.Red.index == 0);
```

使用`mixin`关键字来定义一个混入，用于任意类之间的代码复用，其功能跟Vue中的mixins类似。类声明时使用`with`关键字来使用混入

```dart
Musical {
  bool canPlayPiano = false;
  bool canCompose = false;
  bool canConduct = false;
}
class Musician with Musical {
  // ...
}
```



### 模块化

> 默认一个文件就是一个库

使用`import`后接一串URI来导入一个内置库：对于内置库URI以`dart:`开头；对于自定义和第三方库则使用文件路径或以`package:`开头。
默认的导入动作是全量导入，相当于库的文件代码复制到导入位置。如果只需导入部分，可以使用`show`（只导入指定的部分）或`hide`（导入除指定部分外的所有部分）。如果导入的库与当前代码存在变量冲突，可以使用`as`给库起名，然后像ES6一样使用即可。

如果要在一个文件中集中导出多个库（实现导一即导多的功能），使用`export`，他也可以配合`show`或`hide`使用。

```dart
import 'dart:math' // 导入math库的所有
import 'dart:io' show File; // 只导入File
import 'package:libs/Validator.dart' as Val;

var validator = new Val.Validator();

export 'package:libs/Watcher.dart'
```

如果需要将一个库拆分成多个文件，需要使用`part`和`part of` 关联两个文件，同时这个库需要使用`library`命名。**被关联文件将完全共享作用域**，可以相互访问。

```dart
// part1.dart
library 'myLib'; // 命名库
import 'dart:math';
part 'part2.dart';
C c = new C(); // 访问part2.dart

// part2.dart
class C {}
part of 'part1.dart';
print(sin(pi)); // 访问part1.dart
```



## Dart进阶(TODO)

### 异步

> import 'dart:async';

`async`和`await`使用方法和JS一致，不过`async`是放在括号之后的，而JS中的`Promise`在Dart中叫`Future`。

调用`Timer.periodic(duration: Duration, callback: (timer)=>{})`来设置一个间隔执行定时器，相当于JS中的`setInterval`，如果在回调函数中执行了`timer.cancel()`，则相当于JS中的`setTimeout`