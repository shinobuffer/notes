## BuildContext

在Flutter中，Everything is Widget，但Widget并不是真正要显示在屏幕上的东西，而只是一个配置信息，真正显示在屏幕上的视图树是Element Tree。

在构建视图（生成Element Tree）时，Widget会作为配置信息用于生成Element（因此Element保留了Widget的引用）。

```dart
abstract class StatelessWidget extends Widget {
  const StatelessWidget({ Key key }) : super(key: key);
  @override
  StatelessElement createElement() => StatelessElement(this);
}
```

Element在构建时会传入自身并调用Widget的build方法（从这里看出BuildContext就是Element，或Element实现了BuildContext）

```dart
class StatelessElement extends ComponentElement {
  /// Creates an element that uses the given widget as its configuration.
  StatelessElement(StatelessWidget widget) : super(widget);

  @override
  StatelessWidget get widget => super.widget;

  @override
  Widget build() => widget.build(this);

  @override
  void update(StatelessWidget newWidget) {
    super.update(newWidget);
    assert(widget == newWidget);
    _dirty = true;
    rebuild();
  }
}
```

以StatelessWidget为例，其装载过程如下

- 调用StatelessWidget的 createElement 方法，并根据这个widget生成StatelesseElement对象。

- 将这个StatelesseElement对象挂载到element树上。

- StatelesseElement对象调用widget的build方法，并将element自身作为BuildContext传入。

## 其他

- pageView页面缓存 https://juejin.cn/post/6844903660816695309

