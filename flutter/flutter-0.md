## 了解 Widget

> Everything’s a widget.

官方对`Widget`的定义是“`Element`配置信息的描述”（至于什么是`Element`后期会解释），现阶段为了方便理解，可以认为`Widget`是描述Flutter UI及UI如何交互 v的基本单元，比如`Text`这个`Widget`可以描述文本的内容、大小、颜色等样式。

通过`Widget`的组合嵌套可以组合出更复杂的`Widget`，用以描述更复杂的UI或者交互，他们在代码中呈现出来的层级结构称为【Widget Tree】

按功能划分`Widget`可以分成3类：

- 【Component Widget】——组合Widget，这类Widget继承自`StatelessWidget`或`StatefulWidget`，由若干个Widget组合而成，是业务开发主要的编写对象，也是接下来要重点说明的；
- 【Proxy Widget】——代理Widget，这类Widget会接受并代理一个子Widget，为其提供额外中间功能，比如`InheritedWidget`用于在子Widgets间传递共享信息、`ParentDataWidget`用于配置子Renderer Widget的布局信息；
- 【Renderer Widget】——渲染Widget，跟UI渲染相关的核心Widget，拥有与之一一对应的`RenderObject`，所有的组合Widget和代理Widget的叶子Widget都将会是渲染Widget。

`Widget`基类有三个方法/变量需要重点理解

- key——兄弟节点间的唯一标识
- createElement()——
- canUpdate()——

```dart
@immutable
abstract class Widget extends DiagnosticableTree {
  const Widget({ this.key });

  final Key? key;

  @protected
  @factory
  Element createElement();

  static bool canUpdate(Widget oldWidget, Widget newWidget) {
    return oldWidget.runtimeType == newWidget.runtimeType
        && oldWidget.key == newWidget.key;
  }
  // ...
}
```
