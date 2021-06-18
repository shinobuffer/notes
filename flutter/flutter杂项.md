## 不生效问题

`Divider`和`VerticalDivider`生效需要求父容器有设置`width`/`height`

`GestureDetector`默认交给子处理事件，即子以外`GestureDetector`以内的空白区域是不会触发事件的，如果要空白区域也能触发事件，设置`behavior: HitTestBehavior.opaque`

`TabBar`（继承于`PreferredSize`）默认的高度是和背景色无法修改。可以外套一个`PreferredSize`来修改高度并使用`Container`设置背景，然后记得再外套一个`Material`。

```dart
PreferredSize(
  preferredSize: Size.fromHeight(30), // 修改高度
  child: Container(
    width: double.maxFinite,
    alignment: Alignment.center,
    color: Colors.white, // 修改背景色
    child: Material(
      color: Colors.transparent,
      child: TabBar(
        tabs: [
          Container(
            alignment: Alignment.center,
            height: 30,
            child: Text('tab1'),
          ),
        ],
      ),
    ),
  ),
)
```

## Other

可滚动组件如`SingleChildScrollView`或`ListView`在滚动边界自带波纹，如果需要去掉波纹得外套一个`ScrollConfiguration`并配置behavior属性，

```dart
ScrollConfiguration(
	behavior: NoSplashScrollBehavior(),
  child: ScrollableWidget(),
)

class NoSplashScrollBehavior extends ScrollBehavior {
  @override
  Widget buildViewportChrome(
      BuildContext context, Widget child, AxisDirection axisDirection) {
    if (getPlatform(context) == TargetPlatform.android ||
        getPlatform(context) == TargetPlatform.fuchsia) {
      return GlowingOverscrollIndicator(
        child: child,
        showLeading: false,
        showTrailing: false,
        axisDirection: axisDirection,
        color: Theme.of(context).accentColor,
      );
    } else {
      return child;
    }
  }
}

```

