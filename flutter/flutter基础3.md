> 本文为 Flutter 系列的第三弹，意在介绍 Flutter 中导航、路由、状态共享、动画的实现，如果对某个 Widget 有深入研究需要或需要了解更多 Widget，可以参考[老孟的文章](http://laomengit.com/flutter/widgets/widgets_structure.html)

## 导航

### MaterialApp

`MaterialApp`一般**作为 App 根 Widget 使用**，他封装了实现 Material Design 所需的 Widget

| 属性            | 说明           | 类型                              |
| --------------- | -------------- | --------------------------------- |
| home            | 主页           | Widget                            |
| title           | 标题           | String                            |
| color           | 颜色           | Color                             |
| theme           | 主题           | ThemeData                         |
| routes          | 路由           | Map\<string,(context) => Widget\> |
| initialRoute    | 初始路由       | String                            |
| onGenerateRoute | 路由跳转拦截器 | Function                          |

### Scaffold

`Scaffold`是 Material Design 页面级布局结构的基本实现（**通常一个页面以 Scaffold 为根 Widget**），他提供了用于显示 drawer、snackbar 和底部 sheet 的 API

| 属性                         | 说明         | 类型                              |
| ---------------------------- | ------------ | --------------------------------- |
| appBar                       | 顶部 Bar      | AppBar                            |
| body                         | 页面主体     | Widget                            |
| bottomNavigationBar          | 底部 Bar      | BottomNavigationBar               |
| drawer                       | 抽屉菜单     | Drawer                            |
| floatingActionButton         | 浮动按钮     | FloatingActionButton              |
| floatingActionButtonLocation | 浮动按钮位置 | enum FloatingActionButtonLocation |

`AppBar`为顶部 Bar 的实现，作为 Scaffold 中 appBar 属性的值

| 属性                | 说明                           | 类型           |
| ------------------- | ------------------------------ | -------------- |
| title               | 标题                           | Widget         |
| backgroundColor     | 背景色                         | Color          |
| leading             | 左按钮（用于替换默认返回）     | IconButton     |
| actions             | 右按钮（可多个）               | Widget[]       |
| bottom              | AppBar 下方区域，通常显示 TabBar | Widget(TabBar) |
| centerTitle         | 标题是否居中                   | bool           |
| textTheme/iconTheme |                                |                |

`BottomNavigationBar`为底部导航栏的实现，作为 Scaffold 的 bottomNavigationBar 属性的值，其属性如下

- `currentIndex: int`-当前选中的 index，可用来控制显示的页面
- `onTap: Function`-点击回调，函数接收点击的 index
- `fixedColor: Color`-选中颜色
- `items: Widget[]`-底 bar 项，Flutter 提供`BottomNavigationBarItem`
- `type: BottomNavigationBarType `-当底 tab 数大于三时，要设置`BottomNavigationBarType.fixed`

```dart
bottomNavigationBar: BottomNavigationBar(
	currentIndex: this.curIndex,
  opTap: (int index) {
    this.setState((){this.curIndex = index;});
  },
  items: [
    BottomNavigationBarItem(
    	icon: Icon(Icons.home),
      title: Text('首页'),
    ),
  ]
)
```

`TabBar`为顶部 TabBar 的实现，**通常作为 AppBar 中 title 或 bottom 属性的值**，并配合`TabControler`和`TabBarView`控制 Tab 的显示内容和切换

| 属性         | 说明         | 类型             |
| ------------ | ------------ | ---------------- |
| tabs         | tab 项        | Widget[] (Tab[]) |
| controller   | 自定义控制器 | TabController    |
| isScrollable | 是否可滚动   | bool             |
| indicator... | 指示器相关   | --               |
| label...     | 标签相关     | --               |

```dart
home: defaultTabController( //默认的控制器包裹 Scaffold
  length: 2, // tab 数量
  child: Scaffold(
    appBar: AppBar(
      title: Text('顶部 Bar'),
      bottom: TabBar(
      	tabs: <Widget>[
          Tab(text: 'tab1'),
          Tab(text: 'tab2'),
        ]
      )
    ),
    body: TabBarView( // 设置 tab 对应的视图
    	children: <Widget>[
        Page1(),
        Page2(),
      ]
    ),
  )
)
```

使用默认的控制器`DefalutTabController`能实现简单的 TabBar 功能但是**不能监听 Tab 相关事件**，如果需要监听 Tab 事件，需要创建自定义控制器`TabController`并在 TabBar 和 TabBarView 中使用该控制器

```dart
class TestPage extends StatefulWidget {
  TestPage({Key key}) : super(key: key);

  @override
  _TestPageState createState() => _TestPageState();
}

class _TestPageState extends State<TestPage>
    with SingleTickerProviderStateMixin {
  TabController _tabController; // 自定义控制器

  @override
  void initState() {
    super.initState();
    _tabController = new TabController(length: 2, vsync: this); // 创建控制器
    _tabController.addListener(() {
      // 监听 tab 变化
      print(_tabController.index);
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: TabBar(
          controller: this._tabController, // 使用控制器
          tabs: [Tab(text: 'tab1'), Tab(text: 'tab2')],
        ),
      ),
      body: TabBarView(
        controller: this._tabController, // 使用控制器
        children: [
          Center(child: Text('tab1 page')),
          Center(child: Text('tab2 page')),
        ],
      ),
    );
  }
}
```



___
`Drawer`为抽屉菜单的实现，作为 Scaffold 中 drawer 或 endDrawer 的值

| 属性  | 说明   | 类型   |
| ----- | ------ | ------ |
| child | 菜单体 | Widget |

PS1：Flutter 提供现成的菜单头`DrawerHeader`和`UserAcccountDrawerHeader`来快捷开发

**PS2：点击菜单跳转后通过`Navigation.pop(context`收起菜单**



## 路由

**路由包括基本路由和命名路由**，通过`Navigator`实现路由跳转，**调用导航相关方法前需要先获取上下文**，调用格式有两种，分别是`Navigator.of(context).xxx(...)`和`Navigator.xxx(context, ...)`，xxx 对应的方法如下

| xxx                | 说明                               | 参数                  |
| ------------------ | ---------------------------------- | --------------------- |
| push               | 路由入栈                           | Route                 |
| pushReplacement    | 栈顶路由替换                       | Route                 |
| pushAndRemoveUntil | 路由入栈，清除栈底到指定路由间路由 | ModalRoute.withName() |
| pop                | 路由出栈                           |                       |
| canPop             | 返回布尔，当前路由能否出栈         |                       |
| mayPop             | 当前路由能出栈就出栈               |                       |

通常一个路由就是一个页面，要想把一个页面封装成路由，可以使用`MaterialPageRoute`，他实现与平台页面切换动画风格一致的路由切换动画

```dart
Route route = MaterialPageRoute({
  WidgetBuilder builder, // (context) => Widget，可以传参
  RouteSettings settings, // 配置信息
  bool maintainState = true, // 是否缓存
  bool fullscreenDialog = false, // 是否全屏的模态对话框
});
```



___

命名路由需要在`MaterialApp`的`routes`参数中提前配置，该参数接收类型为`Map<string,(context) => Widget>`。相关方法如下，大多对应普通路由的方法

| xxx                     | 说明           | 参数                          |
| ----------------------- | -------------- | ----------------------------- |
| pushNamed               | 命名路由入栈   | String                        |
| pushReplacementNamed    | 参照上面       | String                        |
| pushNamedAndRemoveUntil | 参照上面       | String, ModalRoute.withName() |
| popUntil                | 出栈到指定路由 | ModalRoute.withName()         |

**命名路由的相关方法是无法实现传参**的，需要使用路由跳转拦截器`onGenerateRoute`并处理参数传递，如下

```dart
final routes = {
  // About 要有相关构造函数接收参数
	'/about': (context, {arguments}) => About(arguments: arguments),
};

@override
Widget build(BuildContext context) {
	return MaterialApp(
		home: Home(),
		onGenerateRoute: (RouteSettings settings) { 
      final Function pageBuilder = this.routes[settings.name]; // 获取要跳转的路由
      if (pageBuilder != null) {
        if (settings.arguments != null) { // 有参数就传递参数
          final Route route = MaterialPageRoute(
          	builder: (context) => pageBuilder(context, arguments: settings.arguments)
          );
          return route;
        }else {
          final Route route = MaterialPageRoute(
          	builder: (context) => pageBuilder(context)
          );
          return route;
        }
      }
    }
	);
}
// 使用
Navigation.pushNamed(context, '/about', arguments: {"id": 114});
```



## 状态共享

### 父子状态共享

`InheritedWidget`提供一种数据在 widget 树中从上到下传递、共享的方式。父 widget 通过`InheritedWidget`共享了一个数据，那么在任意子孙 widget 中来获取该共享的数据。如果子依赖了`InheritedWidget`中共享的数据，则钩子函数`didChangeDependencies()`会在共享数据发生变动时被调用。

通过继承`InheritedWidget`，创建一个 Widget 专门存放需要共享的数据，这样他的子孙都能共享其中的数据

```dart
class ShareDataWidget extends InheritedWidget {
  ShareDataWidget({
    @required this.data,
    Widget child
  }) :super(child: child);

  final int data; //需要在子树中共享的数据，保存点击次数

  //定义一个便捷方法，方便子树中的 widget 获取共享数据  
  static ShareDataWidget of(BuildContext context) {
    return context.dependOnInheritedWidgetOfExactType<ShareDataWidget>();
  }

  //该回调决定当 data 发生变化时，是否通知子树中依赖 data 的 Widget  
  @override
  bool updateShouldNotify(ShareDataWidget old) {
    //如果返回 true，则子树中依赖(build 函数中有调用)本 widget
    //的子 widget 的`state.didChangeDependencies`会被调用
    return old.data != data;
  }
}
```

然后把父子组装起来，在子 Widget 中获取共享数据

```dart
class _TestWidget extends StatefulWidget {
  @override
  __TestWidgetState createState() => new __TestWidgetState();
}

class __TestWidgetState extends State<_TestWidget> {
  @override
  Widget build(BuildContext context) {
    //使用 InheritedWidget 中的共享数据
    return Text(ShareDataWidget
        .of(context)
        .data
        .toString());
  }

  @override
  void didChangeDependencies() {
    super.didChangeDependencies();
    //父或祖先 widget 中的 InheritedWidget 改变(updateShouldNotify 返回 true)时会被调用。
    //如果 build 中没有依赖 InheritedWidget，则此回调不会被调用。
    print("Dependencies change");
  }
}

class InheritedWidgetTestRoute extends StatefulWidget {
  @override
  _InheritedWidgetTestRouteState createState() => new _InheritedWidgetTestRouteState();
}

class _InheritedWidgetTestRouteState extends State<InheritedWidgetTestRoute> {
  int count = 0;

  @override
  Widget build(BuildContext context) {
    return  Center(
      child: ShareDataWidget( //使用 ShareDataWidget
        data: count,
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            Padding(
              padding: const EdgeInsets.only(bottom: 20.0),
              child: _TestWidget(),//子 widget 中依赖 ShareDataWidget
            ),
            RaisedButton(
              child: Text("Increment"),
              //每点击一次，将 count 自增，然后重新 build,ShareDataWidget 的 data 将被更新  
              onPressed: () => setState(() => ++count),
            )
          ],
        ),
      ),
    );
  }
}
```

### 全局状态共享

> 使用第三方库 provider 来实现全局状态共享

`Provider`是对`InheritedWidget`的封装，有以下类型的`Provider`，具体 API 可移步[此处](https://pub.flutter-io.cn/documentation/provider/latest/provider/provider-library.html)，下面会以`ChangeNotifierProvider`为例

| 类型                   | 描述                                                         |
| ---------------------- | ------------------------------------------------------------ |
| Provider               | 基础 provider，携带并暴露一个值                               |
| ListenableProvider     | `ListenableProvider`会监听对象的变化，只要`ListenableProvider`的 listner 被调用，`ListenableProvider`就会重新构建依赖于该 provider 的控件。 |
| ChangeNotifierProvider | 一种特殊的`ListenableProvider`，它基于`ChangeNotifier`，并且在有需要的时候，它会自动调用`ChangeNotifier.dispose`。 |

使用默认构造方法创建一个 provider，默认情况下 provider 是懒加载的，即共享的数据至少要被请求一次，`create`方法才会被调用，但你可以设置`lazy: false`来禁用懒加载

```dart
ChangeNotifierProvider(
  create: (_) => MyChangeNotifier(), // 共享的数据
  child: Son(),
  // lazy: false,
)
```

然后在子孙 Widget 中可以使用`BuildContext`的扩展方法来读取共享数据（下面的`T`对应`create`方法返回的共享数据类型）

- `context.watch<T>()`, 该方法会使用对应的控件**监听 T 的变化**。
- `context.read<T>()`, 该方法直接返回 T，并**不会监听的变化**。

___

下面来看使用代码，首先创建共享数据的容器（`ChangeNotifier`）

```dart
class MyChangeNotifier with ChangeNotifier {
  int _counter = 0;

  int get counter => _counter;

  incrementCounter() {
    _counter++;
    notifyListeners();//处理完业务后调用这个方法通知 Provider 更新依赖他的子孙 Widget
  }
}
```

然后使用`ChangeNotifierProvider`作为顶层 Widget，设置共享的数据并绑定子 Widget，在子 widget 中就可以获取共享数据和调用更新共享数据的方法了

```dart
class MyHomePage extends StatelessWidget {
  final String title;

  MyHomePage({Key key, this.title}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    MyChangeNotifier notifier =
        Provider.of<MyChangeNotifier>(context); //通过 Provider.of(context)获取 MyChangeNotifier
    return Scaffold(
      appBar: AppBar(
        title: Text(title),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            Text('You have pushed the button this many times:'),
            Text(
              '${context.watch<MyChangeNotifier>().counter}', // 获取共享的数据（监听型）
              style: Theme.of(context).textTheme.headline4,
            ),
          ],
        ),
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: notifier.incrementCounter, //调用 notifer 中更新数据的方法
        tooltip: 'Increment',
        child: Icon(Icons.add),
      ),
    );
  }
}

class MyApp extends StatelessWidget {
 @override
 Widget build(BuildContext context) {
   return MaterialApp(
     title: 'Flutter Demo',
     theme: ThemeData(
       primarySwatch: Colors.blue,
       visualDensity: VisualDensity.adaptivePlatformDensity,
     ),
     home: ChangeNotifierProvider(
         create: (_) => MyChangeNotifier(),
         child: MyHomePage(title: 'Flutter Demo Home Page')),
   );
   // 注意这里 Provider 直接包裹 Homepage
   // 这样 Homepage 才能通过 context 获取到 Notifier 实例
 }
}
```

___

上面的代码，如果你要把 Provider 也移动到 Homepage 里，会因为失去直接包裹关系而无法正确获取`Provider<T>`，导致 22 行代码出错。为解决该问题需要引入`Consumer`，他能从先祖获取`Provider<T>`然后传递给`builder`中的组件使用，其作用有二

1. context 中没有 Provider 时，`Consumer`允许我们从`Provider`中的获取数据
2. 更细粒度的重构来帮助性能优化。

`Consumer`接收`child`和`builder`属性，

- `builder`为一个函数，接收三个参数`(BuildContext context, T model, Widget child)`，返回 Widget，其中的 model 即为共享数据类型。
- `child`为一个 Widget，这个 Widget 不应该依赖 Provider 的共享数据，因为该`child`会作为第三个参数传给`builder`，共享数据变化时不会重绘

改动后的代码如下

```dart
class MyHomePage extends StatelessWidget {
  final String title;
  final MyChangeNotifier notifier = MyChangeNotifier();

  MyHomePage({Key key, this.title}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return ChangeNotifierProvider(
      create: (_) => notifier,
      child: Scaffold(
        appBar: AppBar(
          title: Text(title),
        ),
        body: Consumer<MyChangeNotifier>( // 使用 Consumer
          builder: (_, localNotifier, child) => Center(
            child: Column(
              mainAxisAlignment: MainAxisAlignment.center,
              children: <Widget>[
                Text('You have pushed the button this many times:'),
                Text(
                  '${localNotifier.counter}',
                  style: Theme.of(context).textTheme.headline4,
                ),
                child, // 这个 child，即传入的 Text('我不会重绘')，不会被重绘
              ],
            ),
          ),
          child: Text('我不会重绘'), // 作为 child 传给 builder
        ),
        floatingActionButton: FloatingActionButton(
          onPressed: notifier.incrementCounter,
          tooltip: 'Increment',
          child: Icon(Icons.add),
        ),
      ),
    );
  }
}
```

___

`Consumer`的监听粒度是 Provider 级别的，即 Provider 的任意属性一旦发生改变（任意 notifyListeners 执行），就会触发依赖者的重绘。如果只监听 Provider 的部分属性，需要使用`Selector<A,S>`（这里`A`为 Provider，`S`为 Provider 中要监听的属性的类型或类型集合），使用方法类似于`Consumer`但是多了一个`selector`属性用于选取要监听的属性。

- `selector`为一个函数，接收两个参数`(BuildContext context, A model)`，返回要监听的具体属性，并且作为第二参数传给`builder`
- `builder`为一个函数，接收三个参数`(BuildContext context, S data, Widget child)`，返回 Widget，其中的 data 为`selector`的返回值
- `child`同`Consumer`

```dart
Selector<MyChangeNotifier, int>(
  selector: (context, notifier) => notifier.counter, // 监听单个属性
  builder: (context, data, child) => ...,
  child: ...,
)
```

如果要监听多个属性，使用`TupleX`（X=1,2...）来包装多个属性类型

```dart
Selector<MyChangeNotifier, Tuple2<int, String>>(
  selector: (context, notifier) => Tuple2(notifier.counter, notifier.name), // 监听多个属性
  builder: (context, data, child) => Text('${data.item1}, ${data.item2}'),
  child: ...,
)
```

___

要共享多个的复杂的数据，使用`MultiProvider`，用于包装多个 Provider

```dart
MultiProvider(
  providers: [
    Provider<Something>(create: (_) => Something()),
    Provider<SomethingElse>(create: (_) => SomethingElse()),
    Provider<AnotherThing>(create: (_) => AnotherThing()),
  ],
  child: someWidget,
)
```



## 动画

Flutter 中对动画进行了抽象，主要涉及 Animation、Curve、Controller、Tween 这四个类，它们一起配合来完成一个完整动画。

先来句总结：**Controller 确定动画时长=>Curve 确定运动曲线（可选）=>Tween 确定动画值区间**

### Animation

`Animation`是一个抽象类，主要的功能是**保存动画的插值和状态**，是一个在一段时间内依次生成一个区间(Tween)之间值的类。动画的曲线函数由`Curve`来决定

`Animation`有如下两个方法：

1. `addListener()`-用于给`Animation`添加帧监听器，在每一帧都会被调用。帧监听器中最常见的行为是改变状态后调用`setState()`来触发 UI 重建。
2. `addStatusListener()`-用于给`Animation`添加“动画状态改变”监听器；动画开始、结束、正向或反向（见下`AnimationStatus`定义）时会调用状态改变的监听器。
   1. `AnimationStatus.dismissed`-动画仍未开始
   2. `AnimationStatus.forward`-动画正向执行中
   3. `AnimationStatus.reverse`-动画反向执行中
   4. `AnimationStatus.completed`-动画完成



### Curve

`Curve`用于描**述动画的过程**，即曲线函数，通过`CurvedAnimation`来指定动画的曲线，他接收`parent`和`curve`两个参数

```dart
final CurvedAnimation curve =
  new CurvedAnimation(parent: controller, curve: Curves.easeIn);
```

`CurvedAnimation`可以通过包装`AnimationController`和`Curve`生成一个新的动画对象 ，通过这种方式来将动画和动画执行的曲线关联起来的。`Curves`枚举类提供了常用的曲线函数，但你也可以继承`Curve`类并实现`transform`方法来自定义一个曲线函数

```dart
class ShakeCurve extends Curve {
  @override
  double transform(double t) {
    return math.sin(t * math.PI * 2);
  }
}
```



### AnimationController

`AnimationController`用于**控制动画**，它包含动画的启动`forward()`、停止`stop()` 、反向播放 `reverse()`等方法；在动画的每一帧，生成一个新的值（`Animation.value`）用于重绘 UI。

`AnimationController`派生自`Animation<double>`，默认情况下，他在给定的时间段内线性的生成从 0.0 到 1.0（默认区间）的数字，但也可以通过`lowerBound`和`upperBound`来指定区间。

```dart
final AnimationController controller = new AnimationController( 
  duration: const Duration(milliseconds: 2000), 
  lowerBound: 10.0,
  upperBound: 20.0,
  vsync: this
);
```

这里的`vsync`参数，它接收一个`TickerProvider`类型的对象，通常我们会将`SingleTickerProviderStateMixin`添加到`State`的定义中，然后将 State 对象作为`vsync`的值，就上上面一样

### Tween

`Tween`派生自`Animatable<T>`，当动画值要使用不同的范围或不同的数据类型时（double 以外），可以使用`Tween`来添加映射以生成不同的范围或数据类型的值

```dart
final Tween doubleTween = new Tween<double>(begin: -200.0, end: 0.0);
final Tween colorTween =
  new ColorTween(begin: Colors.transparent, end: Colors.black54);
```

要使用 Tween 对象，需要调用其`animate()`方法，然后传入一个控制器对象。所以最终创建一个动画的流程为`controller->[curve]->tween->animation`如下

```dart
final AnimationController controller = new AnimationController(
  duration: const Duration(milliseconds: 500), vsync: this);
final Animation curve =
  new CurvedAnimation(parent: controller, curve: Curves.easeOut);
Animation<int> alpha = new IntTween(begin: 0, end: 255).animate(curve);
```



### 单个动画

```dart
class ScaleAnimationRoute extends StatefulWidget {
  @override
  _ScaleAnimationRouteState createState() => new _ScaleAnimationRouteState();
}

//需要继承 TickerProvider，如果有多个 AnimationController，则应该使用 TickerProviderStateMixin。
class _ScaleAnimationRouteState extends State<ScaleAnimationRoute>  with SingleTickerProviderStateMixin{ 

  Animation<double> animation;
  AnimationController controller;

  initState() {
    super.initState();
    controller = new AnimationController(
        duration: const Duration(seconds: 3), vsync: this);
    //图片宽高从 0 变到 300
    animation = new Tween(begin: 0.0, end: 300.0).animate(controller)
      ..addListener(() {
        //触发重绘
        setState(()=>{});
      });
    //启动动画(正向执行)
    controller.forward();
  }

  @override
  Widget build(BuildContext context) {
    return new Center(
       child: Image.asset("imgs/avatar.png",
          width: animation.value,
          height: animation.value
      ),
    );
  }

  dispose() {
    //路由销毁时需要释放动画资源
    controller.dispose();
    super.dispose();
  }
}
```

**PS：动画不再使用的时候记得释放控制器防止内存泄漏**

上面例子中，动画执行后会不断触发监听器中的`setState`，触发重绘来实现动画效果。可以看到`addListener()`和`setState()` 是一个必须的结构，但这个结构可以使用`AnimatedBuilder`重构来去掉，上面的 build 方法可以改为

```dart
initState() {
  super.initState();
  controller = new AnimationController(
    duration: const Duration(seconds: 3), vsync: this);
  //图片宽高从 0 变到 300
  animation = new Tween(begin: 0.0, end: 300.0).animate(controller);
  //启动动画(正向执行)
  controller.forward();
}
@override
Widget build(BuildContext context) {
    return AnimatedBuilder(
      animation: animation,
      child: Image.asset("images/avatar.png"),
      builder: (BuildContext ctx, Widget child) {
        return new Center(
          child: Container(
              height: animation.value, 
              width: animation.value, 
              child: child,
          ),
        );
      },
    );
}
```

上面的重构看起来出现了两个`child`，实际上外层`child`会传递给`AnimatedBuilder`的匿名构造器，供构造器将动画化的容器和非动画化的内容结合，最终的结果是构造器返回的对象插入到 widget 树中。

通过这种方法可以封装过渡效果来复用动画如下

```dart
class GrowTransition extends StatelessWidget {
  GrowTransition({this.child, this.animation});
	// 接收内容 child 和动画效果 animation（也可以接收 controller 再在内部构件 animation）
  final Widget child;
  final Animation<double> animation;

  Widget build(BuildContext context) {
    return new Center(
      child: new AnimatedBuilder(
          animation: animation,
          builder: (BuildContext context, Widget child) {
            return new Container(
                height: animation.value, 
                width: animation.value,
                child: child
            );
          },
          child: child
      ),
    );
  }
}
```

### 多个动画

多个动画按序或重叠执行需要创建多个动画对象，但只靠一个`AnimationController`控制所有的动画对象，即`controller(1)-curve(n)->tween(n)->animation(n)`。通过使用`Interval`确定每个动画的运动时段

```dart
color = ColorTween(
  begin:Colors.green,
  end:Colors.red,
).animate(
  CurvedAnimation(
    parent: controller, //所欲动画公用一个 controller
    curve: Interval(
      0.0, 0.6, //间隔，前 60%的动画时间
      curve: Curves.ease,
    ),
  ),
);

padding = Tween<EdgeInsets>(
  begin:EdgeInsets.only(left: .0),
  end:EdgeInsets.only(left: 100.0),
).animate(
  CurvedAnimation(
    parent: controller,
    curve: Interval(
      0.6, 1.0, //间隔，后 40%的动画时间
      curve: Curves.ease,
    ),
  ),
);
```

