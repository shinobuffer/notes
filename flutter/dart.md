## 特性追加

### 2.15

- 类的构造函数可以像普通函数一样被引用和传递了

  ```dart
  const texts = ['t1', 't2'].map(Text.new).toList();
  ```

- 枚举可使用`.name`获取枚举值的字符值；可使用`.values.byName()`通过字符值获取对应的枚举值；可使用`.values.asNameMap()`获取所有【字符值->枚举值】的映射

  ```dart
  enum MyEnum { one, two }
  
  void main() {
    print(MyEnum.one.name);  // "one"
    print(MyEnum.values.byName('two') == MyEnum.two);  // true
    print(MyEnum.values.asNameMap()); // {one: MyEnum.one, two: MyEnum.two}
  }
  ```

### 2.17

- 枚举可以像类一样支持构造函数、成员变量和方法。每个枚举值对构造函数的显式调用，对应枚举的一个实例

  ```dart
  enum Mass {
    gram('g'), // 枚举值显式调用构造函数
    kilogram('kg');
    
    const Mass(this.symbol);
    
    final String symbol;
    
    @override
    String toString() => "$name-$symbol"
  }
  
  void main() {
    print(Mass.gram);  // "gram-g"
  }
  ```

- 在参数列表中直接使用`super`关键字完成参数向父类构造函数的传递调用

  ```dart
  class MyApp extends StatelessWidget {
    const MyApp({super.key}); 
    // previous
    // const MyApp({Key? key}) : super({key: key})
  	
    @override
    Widget build(BuildContext context) {
      // ...
    }
  }
  ```

- 调用参数列表中，命名参数的位置不再受【必须放在最后】的限制，可随意放置



## Dart 基础（截止至2.13）

### 变量

Dart 是强类型语言，存在类型推断和类型校验。除了使用具体的类型来声明变量，还可以使用`var`和`dynamic`声明变量
- `var`-类型通过推断得到
- `dynamic`-类型不做限制

如果需要常量可以使用`final`和`const`关键字（`const`隐式为`final`）
- `final`-可以先声明后初始化，**运行时常量**
- `const`-需要声明时同时初始化，**编译时常量**

常用类型有`int,double,String,Boolean,List,Set,Map,Rune,Symbol`

`String`即字符串，使用**单/双引号创建单行字符串**，使用**三个单/双引号创建跨行字符串**，使用`r`前缀创建原始字符串（类似 python），在字符串中使用`${expression}`的方式内嵌表达式，**相邻的字符串会自动拼接**，也可以使用`+`连接字符串

```dart
const s1 = 'string';
const s2 = '''
string with line break
''';
print(r'\n') // "\n"
print('$1''666') // "string666"
```

`Boolean`即布尔值，只有`true`和`false`两个值，Dart 是**类型安全**的，即**条件表达式必须是布尔值**而不能是`0/null`之类的值

`List`即数组，可以通过字面量`<T>[]`定义或`new List<T>()`定义，数组中的元素需要同类型

`Set`即集合（去重数组），可以通过字面量`<T>{}`定义或`new Set<T>()`定义，集合中的元素需要同类型

`Map`即映射，可以通过字面量`{}`定义或`Map<K,V>()`定义，**索引时必须使用中括号**

几个有点特殊的运算符
- `...`-展开可迭代变量
- `...?`-展开可空的可迭代变量
- `~/`-取整除法
- `?.`-如果对象的属性非空予以访问和修改
- `??=`-被赋值对象为空则执行赋值
- `??`-`A ?? B`相当于`A == null ? B : A`
- `is`-类型判断如`'str' is String`
- `as`-类型强转
- `..`级联运算符，可在同一个对象上连续调用多个对象的变量或方法

```dart
var paint = Paint() // 获得对象 point
  ..color = Colors.black // 修改 color 变量
  ..clear(); // 调用 clear 方法
// 等同与
var paint = Paint();
paint.color = Colors.black;
paint.clear();
```

### 空安全特性

**类型声明时默认是非空的**，即必须在声明时或者构造函数阶段赋值；若想声明可空变量，需要在类型后面追加`?`如`String? name;`

如果确定变量非空，可在变量后添加`!`来进行非空断言如`name!.length`（为空时会抛异常）

可以用`late`关键字（放在类型前）延迟非空变量的赋值（可不立即赋值），这在一些接口或抽象类里是有用的

现已支持`require`关键字声明必选参数，常用于命名参数或构造函数初始化形式中

### 函数
在 Dart 中函数是一等公民，能赋值给变量或作为参数传递，对应类型`Function`。一个最基本的函数需要包含以下要素：
```dart
/* 基本函数
[修饰符、返回类型] 函数名([参数]){
  //函数体
}
*/
int add(int a, int b) {
  return a + b;
}

/* 如果一个函数只有一个表达式可以用箭头语法简化
[修饰符、返回类型] 函数名([参数]) => 单句表达式
*/
int add(int a, int b) => a + b;
```

缺省函数名的函数为匿名函数，多作为一次性函数类型入参：

```dart
<int>[1, 2, 3].sort((int a, int b) => b - a);
```

可选参数分为位置参数和命名参数，声明可选参数**需要放在参数列表的最后面**：

- **中括号包裹**参数表示可选位置参数
- **大括号包裹**参数表示可选命名参数

使用`=`给可选参数指定默认参数；命名参数可以使用`required`关键字表示必选，传命名参数时使用`:`指定

```dart
// 命名参数
void fun1(int a, {required int b, int? c}) {}
fun1(1, b : 2);
// 位置参数
void fun2(int a, [int b=2]) {};
fun2(1, 3);
```
和 JS 一样 Dart 使用**词法作用域**，即变量的作用域在编写时就已经固定，以所在大括号为作用域；而闭包使用方式同 JS

使用`typedef`关键字来定义类型别名，如`typedef Sort<T> = bool Function(T a, T b)`，该类型别名可以作为类型来声明变量

### 控制流集合

在数组中可以使用`if/else`语句来控制元素的有无，使用`for`来遍历元素（对标 React 的 JSX 中的三元表达式和 map），在编写 Flutter 时是非常方便的

```dart
Column(children:[
  if(renderWidget1)
  	Widget1(),
  else Widget2(),
  for(int i in [1,2,3]) Text('$i'),
]);
```

### 异常处理

Dart 提供了`Exception` 和`Error`两种类型的异常以及它们一系列的子类，可以基于两者编写自己的异常类

使用`throw`来抛出异常和错误；使用`on`（能指定异常类别）或者`catch`都能捕获异常；使用关键字`rethrow`将异常重新抛出；使用`finally`指定最终执行的代码（`catch`可以缺省，此时会在处理完`finally`后抛出异常）
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

> Dart 没有访问修饰符，只区分公共成员和私有成员（通过命名）
>
> **以下划线开始来命名来定义私有属性和方法**

类的构造函数的名字可以是`ClassName`（只能一个）或者`ClassName.identifier`（命名构造函数，可多个）；除非变量名和入参冲突，方法中引用实例变量时建议省略`this`
```dart
class Point {
	num x, y;
  Point(int x, int y) {
    this.x = x;
    this.y = y;
  }
  //上面可以化简为（推荐使用）
  //Point(this.x, this.y);
  Point.zero() {
    x = y = 0;
  }
}

Point p1 = new Point(1,1);
Point p2 = new Point.zero();
```
一个跟构造函数有密切关系的符号是冒号`:`，作用有三：

- 在继承场景下调用父构造函数（用`super`引用父类）
- 初始化列表：在构造函数执行前初始化实例变量**（无法访问`this`）**
- 重定向构造函数（如`Point.zero(): this(0, 0);`，这里的 this 指向本类）

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
构造函数可用 `const`修饰，表示可创建常量实例，相同的入参创建的实例是相同的。使用条件如下：

- 常量构造函数的所有成员必须是`final`的
- 常量构造函数不能有方法体
- 创建常量实例的入参必须是常量

使用`static`关键字来定义静态属性和方法，**静态方法中只能访问静态成员**

使用`factory`关键字来定义工厂构造函数（执行构造函数并不总是创建这个类的一个新实例），**工厂构造函数只能访问静态属性**，常用于：

- 将创建的实例缓存起来，避免重复创建实例（特别的有单例模式）
- 工厂模式-调用子类的构造函数返回子类

```dart
// 工厂构造函数将要创建的实例缓存起来
class Logger {
  final String name;
  bool mute = false;

  // _cache 是私有属性
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
使用`operator`关键字来重写操作符（`==`重载必须必须重写 `hashCode` 方法）

```dart
class Vector {
  final int x, y;

  Vector(this.x, this.y);

  Vector operator +(Vector v) => Vector(x + v.x, y + v.y);
  Vector operator -(Vector v) => Vector(x - v.x, y - v.y);
}
```

使用`get`/`set`关键字来定义属性的 getter 和 setter，其使用方法同 JS

```dart
class Rectangle {
  num left, top, width, height;

  Rectangle(this.left, this.top, this.width, this.height);

  // 定义两个计算属性： right 和 bottom
  get right => left + width;
  set right(num value) => left = value - width;
  get bottom => top + height;
  set bottom(num value) => top = value - height;
}
```

___

> 继承是一个 "是不是"的关系（适用于代码复用），而接口实现则是 "有没有"的关系（适用于行为约束）

使用`extends`关键字来继承父类**（只能单继承）**，使用`implements`关键字来实现接口（**可多实现**）。在继承和实现的场景下如果需要重写或实现方法时建议使用`@override`来注释方法

使用`abstract`关键词来定义抽象类，抽象类中的**抽象方法只有定义没有实现**，必须交由子类实现；但是**抽象类也可以有具体的实现**

**接口没有显式的定义，每个类都隐式的定义了一个接口**，接口包含了该类所有的实例成员及其实现的接口，接口中的方法必须交由子类实现，**建议使用抽象类来作为接口**

```dart
abstract class Dog {
  void eat();
  void sleep() {
    print('睡大觉');
  }
}

abstract class IFurry {
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

使用 `enum` 关键字定义一个枚举类型，枚举中的每个值都有一个`index`属性，代表该枚举值定义时的位置（从零开始，**无法手动指定枚举值的 index**）。另外可通过`enum.values`来获取枚举值的集合

```dart
enum Color { red, green, blue };
assert(Color.red.index == 0);
assert(Color.values[0] == Color.red);
```

使用`mixin`关键字来定义一个混入，使用`on`关键字限制 mixin 的使用范围（可多个）；mixin 用于任意类之间的代码复用，其功能跟 Vue 中的 mixins 类似。类声明时使用`with`关键字来使用 mixin（可多个）

```dart
// 定义一个 Musical mixin，只能供 Musician 使用
mixin Musical on Musician {
  bool canPlayPiano = false;
  bool canCompose = false;
  bool canConduct = false;
}
class Musician with Musical {
  // ...
}
```

Dart 的泛型使用与 TS 类似，但 Dart 的泛型是**固化的**，即运行时可使用`is`判断泛型

### 模块化

> 默认一个文件就是一个库

使用`import`后接一串 URI 来导入一个内置库：对于内置库 URI 以`dart:`开头；对于自定义和第三方库则使用文件路径或以`package:`开头

**默认的导入动作是全量导入**，相当于库的文件代码复制到导入位置。如果只需导入部分，可以使用**`show`（只导入指定的部分）**或**`hide`（导入除指定部分外的所有部分）**

如果导入的库与当前代码存在变量冲突，可以**使用`as`给库起名**，创建一个命名空间

如果要在一个文件中集中导出多个库（实现导一即导多的功能），使用`export`，他也可以配合`show`或`hide`使用

```dart
import 'dart:math' // 导入 math 库的所有
import 'dart:io' show File; // 只导入 File
import 'package:libs/Validator.dart' as Val;

var validator = new Val.Validator();

export 'package:libs/Watcher.dart'
```

如果需要将一个库拆分成多个文件，需要使用`part`和`part of` 关联两个文件，同时这个库需要使用`library`命名。**被关联文件将完全共享作用域**，可以相互访问

```dart
// part1.dart
library 'myLib'; // 命名库
import 'dart:math';
part 'part2.dart';
C c = new C(); // 访问 part2.dart

// part2.dart
class C {}
part of 'part1.dart';
print(sin(pi)); // 访问 part1.dart
```



## Dart 进阶

### 核心库

> 核心库 dart:core 无需导入即可使用

`int`和`double`实现了`num`，和 JS 不一样，像取绝对值、取整、四舍五入之类的简单变换（除了三角函数、对数等复杂变换）已经成为`num`的实例方法，具体可查看 IDE 的提示

int 相比 num 多了`gcd()`、`toRadixString()`、`modPow()`等方法；double 相比 num 多了`remainder()`方法

若需要对数、三角函数、随机数的支持，则需要导入`dart:math`库

------

除了 pad/trim/split/subString()和大小写转换等纯`String`方法，`String`类还支持大量支持字符串/正则的匹配方法：

- `startWith/endWith/contains()`-略
- `indexOf/lastIndexOf()`-索引
- `replaceFirst[Mapped]/replaceAll[Mapped]()`-首次替换/全局替换
  - 含 Mapped 的方法可传入一个`String replace(Match match)`方法来构造 replace 字符串

`StringBuffer`相当于 String 的可变类，可以动态高效的构造字符串，避免字符串拼接带来的新临时变量。使用`write/writeAll/writeln/clear()`方法来执行写入/清空操作

------

使用`RegExp`默认构造器来创建正则表达式，表达式语法同 JS。有`allMatches()`（获取所有匹配），`firstMatch()`（获取首个匹配）`hasMatch()`（js-reg.test）/`stringMatch()`（获取首个匹配字符串）四个实例方法

这里的“匹配”对应`RegExpMatch`类型的对象，其实例属性方法如下

- `start/end/groupCount`-匹配起点/匹配终点/命中分组数
- `group/groups()`根据分组索引获取分组的匹配值**（索引 0 对应匹配的字符串，从 1 开始对应各分组）**
- `namedGroup()`根据分组名获取分组的匹配值

除了使用 group()方法获取分组的匹配值，match 对象还可以直接索引来获取匹配值**（索引 0 对应匹配的字符串，从 1 开始对应各分组）**如`match[1]`

------

`DateTime`类处理日期时间，共有 5 个构造函数

- `DateTime`-根据参数获取 datetime（本地时区）
- `DateTime.utc`-根据参数获取 datetime（UTC 时区）
- `DateTime.fromMicrosecondsSinceEpoch`-根据微秒时间戳获取 datetime(默认本地时区)
- `DateTime.fromSecondsSinceEpoch`-根据秒时间戳获取 datetime(默认本地时区)
- `DateTime.now`-获取当前 datetime（本地时区）

另外还有`DateTime.parse()`和`DateTime.tyrParse()`两个静态方法能通过解析时间字符串来获取 datetime；**前者解析不了时抛异常，后者解析不了时返回 null**

datetime 的实例属性与 JS 类似在此略，下面介绍几个 datetime 偏移和比较的实例方法

| 实例方法                   | 描述                   | 返回类型 |
| -------------------------- | ---------------------- | -------- |
| add/subtract(duration)     | 对 datetime 进行偏移     | DateTime |
| isAfter/isBefore(datetime) | 比较两个 datetime 的先后 | bool     |
| difference(datetime)       | 计算两个 datetime 的时差 | Duration |

`Duration`是对一段时间的描述，通过向默认构造函数传参来获得一个 duration，duration 之间可比较

------

`Iterable<E>`即可迭代的集合，常用的`List`和`Set`就是 Iterable 的实现。Iterable 拥有`first/last`、`isEmpty/isNotEmpty`和`length`几个属性，和以下方法

- `forEach/map/fold/reduce()`-遍历/映射/js-reduce/无初始值版 reduce
- `where/any/every()`-相当于 JS 数组的 filter/some/every
- `elementAt/firstWhere/lastWhere()`-索引/正向查找/反向查找
- `expand/followedBy/join/contains()`-展开/拼接/join/包含
- `toList/toSet()`-略

List 相对于 Iterable 多了**索引、范围操作**；Set 相对于 Iterable 多了**交并差操作**

| 类型                                                         | 属性       | 方法                                                         |
| ------------------------------------------------------------ | ---------- | ------------------------------------------------------------ |
| List&Set                                                     | Iterable.* | Iterable.*<br/>add/addAll, remove/removeWhere, clear, retainWhere |
| [List](https://api.dart.dev/stable/2.14.3/dart-core/List-class.html) | reversed   | insert/insertAll,<br/>removeAt/removeLast, [last]indexOf/[last]indexWhere,<br/>shuffle/sort, sublist,<br/>getRange/fillRange/removeRange/setRange |
| [Set](https://api.dart.dev/stable/2.14.3/dart-core/Set-class.html) | --         | contains/containsAll<br/>intersection/union/difference<br/>retainAll/retainWhere(修改原对象) |

------

`Map<K,V>`描述键值对的集合，其中的键值对使用`MapEntry<K,V>`描述。Map 的属性和方法如下

| 类型                                                         | 属性                                    | 方法                                                         |
| ------------------------------------------------------------ | --------------------------------------- | ------------------------------------------------------------ |
| [Map](https://api.dart.dev/stable/2.10.5/dart-core/Map-class.html) | isEmpty/isNotEmpty, keys/values/entries | forEach/map<br/>containsKey/containsValue, putIfAbsent<br/>addAll/addEntries, remove/removeWhere, update/updateAll, clear |

`Set`和`Map`的遍历默认按插入序的，如果对次序有其他要求，可以使用他们的子类

- `SplayTreeMap/Set`-按key/element排序的顺序
- `HashMap/Set`-无序

------

[Uri](https://api.dart.dev/stable/2.14.3/dart-core/Uri-class.html)用于解析 URI 地址，详情可查看文档

### 编解码

> import 'dart:convert';

convert 库提供了常用的编解码器，你也可以实现 Codec 和 Converter 来编写自己的编解码器。以 Json 为例，该库提供 Json 的编码器、解码器和编解码器

- `JsonEncoder`-json 编码器，常用于流
- `JsonDecoder`-json 解码器，常用于流
- `JsonCodec`-json 编解码器，提供 encode 和 decode 两个方法

为了方便使用，Dart 提供了一个常量 JsonCodec 实例`json`和两个开箱即用的编解码方法`jsonDecode()`和`jsonEncode()`

```dart
const json = JsonCodec();
dynamic jsonDecode(String source, {Object? reviver(Object? key, Object? value)?}) => json.decode(source, reviver: reviver);
String jsonEncode(Object? object, {Object? toEncodable(Object? nonEncodable)?}) => json.encode(object, toEncodable: toEncodable);
```

当然除了 Json 外，convert 库还提供了 base64、utf8 等格式的编解码方案，其使用方法基本同上

### 异步编程

> import 'dart:async';

`async`和`await`使用方法和 JS 一致，不过**`async`是放在括号之后**的，而 JS 中的`Promise`在 Dart 中叫`Future`，可以通过 Promise 来认识 Future，见下表

| Dart-Future                               | JS-Promise                                                   | 描述                                                         |
| ----------------------------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| Future.value()                            | Promise.resolve()                                            | --                                                           |
| Future.error()                            | Promise.reject()                                             | --                                                           |
| Future.delay()                            | --                                                           | 创建函数体延迟执行的 Future                                   |
| Future.microtask()                        | --                                                           | 创建函数体微任务执行的 Future                                 |
| Future.any()                              | Promise.race()                                               | --                                                           |
| Future.wait()                             | eagerError=false: Promise.allSettled()<br/>eagerError=true: Promise.all() | --                                                           |
| f.then()/f.catchError()/f.whenCompleted() | p.then()/p.catch()/p.finally()                               | --                                                           |
| f.timeout()                               | --                                                           | 对传入的 future 进行倒计时，若传入的 future 未按时结束，使用 onTimeout 回调的值代替该 future |

------

**什么是流：**

`Stream`即流，是一种异步数据源，与此相对 Iterable 是一种同步数据源（因此两者有大量同名方法）。想要从流中获取数据需要成为流的订阅者，流会**异步地推送（microtask 级别）**数据给订阅者，当所有数据都被推送完则发送一个**“done”**事件

流可分为单播流和多播流，**单播流只允许一个订阅者，多播流允许多个订阅者**

**如何创建流**：

除了 Stream 命名构造函数和一些内置方法，创建一个流常用的方式是声明并调用一个`async*`函数：

```dart
// async* 函数返回一个 Stream
Stream<int> foo() async* {
  for (int i = 0; i < 10; i++) {
    // async*中可正常使用 await
    await Future.delayed(const Duration(seconds: 1));
    // async*中通过 yield 来推送一次数据
    yield i;
  }
}

void main async () {
  // 使用 await for 来遍历等待流的推送
  await for (int i in foo())
    print(i);
  	// 0,1...9
}
```

另外一个创建流的方式是使用[StreamController](https://api.dart.dev/stable/2.14.3/dart-async/StreamController/addStream.html)，controller 可以允许开发者创建受控流，**编程式的推送数据/结束推送，并响应订阅者的暂停/恢复/取消订阅行为**。其实例属性如下表所示

| 实例属性                           | 类型     | 描述                                                 |
| ---------------------------------- | -------- | ---------------------------------------------------- |
| done                               | Future   | 当流进入 done 状态，返回的 future 进入完成状态           |
| hasListener                        | bool     | 略                                                   |
| isClosed/isPause                   | bool     | **isPause 只对单播流有意义**                          |
| onListen/onCancel/onPause/onResume | Function | 订阅者行为回调，**onPause/onResume 只对单播流有意义** |
| stream                             | Stream   | controller 控制的 stream                               |

StreamController 的实例方法可以通过下面的代码来了解，实例化一个 StreamController 后通过其`stream`实例属性访问创建的流：

```dart
StreamController<String> streamController = StreamController.broadcast();
// 通过 stream 属性访问创建的流
streamController.stream.listen((data)=>print(data));

// 推送数据
streamController.add(data);
// 推送错误
streamController.addError(error);
// 转发另外一个流的推送，当被转发的流进入 done 状态，返回的 future 进入完成状态（在转发结束前不可再调用推送/转发方法）
var future1 = streamController.addStream(otherStream);
// 结束流推送，返回的 future 同 controller.done
var future2 = streamController.close()
```

**流有什么方法：**

Stream 和 Iterable 同是数据集合的描述（同步异步之分），因此有**Iterable 有的方法 Stream 基本都有**。但这其中据**收敛型的方法（如`any()`）会返回 Future，数据转换或过滤的方法（如`where()`）会返回 Stream**。真正 Stream 独有的实例方法如下表所示

| 实例方法            | 描述                                                         | 返回值                                                       |
| ------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| asBroadcastStream() | 将单播流变为多播流                                           | Stream                                                       |
| asyncMap()          | map 中的转换函数需要异步返回，Stream 会等待异步的完成才会进行一次推送 | Stream                                                       |
| distinct()          | 流去重：若当前推送数据与上一次推送数据一致，跳过当前推送     | Stream                                                       |
| drain()             | 放弃所有数据，返回 Future 表示流的结果（完成或出错）           | Future                                                       |
| handleError()       | 对标 Future.error()，返回对当前流的封装，该流可以拦截被封装流的错误 | Stream                                                       |
| listen()            | 订阅流                                                       | [StreamSubscription](https://api.dart.dev/stable/2.14.3/dart-async/StreamSubscription-class.html) |
| pipe()              | 将流喂到某个 streamConsumer 中                                 | Future                                                       |
| transform()         | 使用一个转换器将流转换为另一个流                             | Stream                                                       |

**订阅流：**

默认创建的 Stream 是单播的，即只能调用一次`listen()`，调用后返回一个订阅对象 streamSubscription。订阅对象可调用`onData/onDone/onError()`来更新回调；可调用`pause/resume()`来请求 Stream 暂停/恢复推送；可调用`cancel()`来取消对 Stream 的订阅（**若取消对单播流的订阅，会让单播流提前进入 done 状态**）

注：**如果不再使用流，务必取消对流的订阅**

------

使用`Timer(duration, callback)`来创建执行一次的定时器（相当于 JS 中的`setTimeout`）；使用`Timer.periodic(duration, callback)`来创建间隔执行定时器（相当于 JS 中的`setInterval`）。若要取消定时器，执行`timer.cancel()`

------

Dart 是单线程的，其异步通过事件循环实现，原理与 JS 中的事件循环一致，Dart 的事件循环维护一个**事件队列**和**微任务队列**（其中**微任务队列优先级更高**）。

所有外部事件任务都会被推入事件队列，如 Future、IO、计时器、点击、以及绘制事件；原生的微任务比较少（已完成的 Future 的 then()的函数体会被推入微任务队列），但可以使用`scheduleMicrotask(task)`方法创建一个微任务



## Dart 扩展(TODO)

### 并发编程

> import 'dart:isolate';

### IO编程

> import 'dart:io';