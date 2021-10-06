> 本文为 Flutter 系列的第一弹，意在介绍入门 Flutter 时常用基础 Widget（**展示 Widget 篇**）及其常用属性，如果对某个 Widget 有深入研究需要或需要了解更多 Widget，可以参考[老孟的文章](http://laomengit.com/flutter/widgets/widgets_structure.html)

## 起步

Flutter 项目的源码放在`lib`目录中，默认的入口文件为`lib/main.dart`，入口函数为`main`函数。调用`runApp(widget)`方法启动一个 App，该方法接受一个`Widget`作为根 Widget

在 Flutter 中一个重要的概念叫`Widget`，他是**功能的抽象描述**。对于学过 RN 的同学，你可以粗略地认为组件在 Flutter 中就是一种 Widget（实际上 Widget 的范围更广，不一定要对应可视化的 UI，继续深入学习后会懂的）

Flutter 提供 Material 和 IOS 两种风格设计，分别导入`package:flutter/material.dart`和`package:flutter/cupertino.dart`即可



## 自定义 Widget

了解内置 Widget 之前来了解下自定义 Widget，自定义 Widget 分无状态和有状态两种，**无状态 Widget 中的状态（属性）无法改变**，对应初始化后不会变动的 UI；**有状态 Widget 中的状态（属性）可以在 Widget 生命周期中变化并响应式更新视图**

- 继承`StatelessWidget`并实现`build()`方法来创建一个无状态 Widget，该方法返回你要封装的 Widget

- 继承`StatefulWidget`并实现`createState()`方法来创建一个有状态 Widget。`createState()`方法需要返回一个 State，因此还需要指定 Widget 的 State，通过继承`State<YourWidget>`并实现`build()`方法来创建这个 State，该方法返回你要封装的 Widget。在 State 中你可以初始化状态并调用`setState()`方法更新状态

```dart
// 无状态 Widget
class Footer extends StatelessWidget {
  @override
  Widget build(BuildContext ctx) {
    return Center(
    	child: Text('Footer'),
    );
  }
}

// 有状态 Widget
class Page extends StatefulWidget {
	_PageState createState() => _PageState();
}

class _PageState extends State<Page> {
  int countNum = 0; // 这是一个状态
  @override
  Widget build(BuildContext ctx) {
    return Column(
      children: <Widget>[
        SizedBox(height: 20),
        Text('${this.countNum}'),
        SizedBox(height: 20),
        RaisedButton(
          child: Text('按钮'),
          onPressed: (){
             setState(() {
               // 更新状态，触发渲染
               this.countNum++;
             });
          },
        )
      ],
    );
  }
}
```



## 基础 Widget

###  Center

用于居中布局，可以**将子 Widget 水平垂直居中**

| 属性  | 说明     | 类型   |
| ----- | -------- | ------ |
| child | 子 Widget | Widget |

### Container

一个**容器**，只能容纳一个子 Widget。**能够设置宽高**，对于不能设置宽高的 Widget，可以外套一个 Container 来设置宽高

| 属性       | 说明               | 类型                           |
| ---------- | ------------------ | ------------------------------ |
| alignment  | 子 Widget 对齐方式   | enum Alignment                 |
| decoration | 背景、边框相关设置 | **BoxDecoration**              |
| child      | 子 Widget           | Widget                         |
| height     | 高                 | double                         |
| width      | 宽                 | double                         |
| margin     | 外边距             | EdgeInsets.all/fromLTRB/only() |
| padding    | 内边距             | EdgeInsets.all/fromLTRB/only() |
| transform  | 变换               | Matrix4.xxx()                  |

BoxDecoration 的属性

- `color: Color`-背景色
- `image: DecorationImage`-背景图
- `border: Border`-边框的颜色、粗细、样式
  - **Border.all(color: Color, width: int, style: BorderStyle)**-设置所有边框
  - **Border(top: BorderSide(color: Color, width: int, style: BorderStyle))**-分别设置边框
- `borderRadius: BorderRadius`-边框弧度
  - **BorderRadius.circular(16)**-四周圆角
  - **BorderRadius.only(topLeft: Radius.circular(64))**-分别设置圆角
- `boxShoadow: <BoxShadow>[]`-容器阴影
  - **BoxShadow(offset: Offset(x,y), color: Color, blurRadius: double, spreadRadius: double)**

### Text

**一行文本**

| 属性（有序号为位置参数，下同） | 说明     | 类型               |
| ------------------------------ | -------- | ------------------ |
| ①content                       | 文本内容 | String             |
| textAlign                      | 对齐方式 | enum TextAlign     |
| textDirection                  | 文本方向 | enum TextDirection |
| overflow                       | 超出处理 | enum TextOverflow  |
| textScaleFactor                | 放大倍率 | double             |
| maxLines                       | 最大行数 | int                |
| style                          | 字体样式 | **TextStyle**      |

TextStyle 的属性

- `decoration: TextDecoration`-装饰线类型
- `decorationColor: Color`-装饰线颜色
- `decorationStyle: TextDecorationStyle`-装饰线风格
- `wordSpacing/letterSpacing: double`-字/词间距
- `fontStyle: FontStyle`-字体
- `fontSize: double`-字大小
- `height: double`-行高（乘以字大小得到）
- `color: Color`-字颜色
- `fontWeight: FontWeight`-字粗细

### Image

**一张图片**，`Image.asset`用于显示本地图片，`Image.network`用于显示网络图片

| 属性         | 说明     | 类型             |
| ------------ | -------- | ---------------- |
| ①uri         | 资源地址 | String           |
| alignment    | 对齐方式 | enum Alignment   |
| height/width | 高宽     | double           |
| fit          | 填充方式 | enum BoxFit      |
| repeat       | 重复方式 | enum ImageRepeat |

加载本地图片需要配置资源目录，Flutter**建议为每张图片准备 3 种规格**，以让 Flutter 能根据设备选择最好图片

1. 创建一个目录`images`（可其他名字）作为资源根目录
2. 创建目录`2.0x`和`3.0x`，放入对应规格的图片，原图放在跟`2.0x`/`3.0x`目录同级位置，三张图片同名
3. 编辑`pubspec.ymal`在`flutter.assets`字段设置资源根目录位置如`images`
4. 通过`'images/.../img.png'`引用图片

### ClipOval

**将子 Widget 裁剪成椭圆**（默认根据子的宽高裁剪），性能较差不推荐大量使用，**优先考虑子 Widget 的 border/shape 支持**

| 属性         | 说明     | 类型        |
| ------------ | -------- | ----------- |
| child        | 子 Widget | Widget      |
| fit          | 填充方式 | enum BoxFit |
| height/width | 高宽     | double      |

他的兄弟 Widget：`ClipRRect`圆角矩形裁剪、`ClipRect`矩形裁剪

### Chip

**一张标签**，默认两边圆角

| 属性            | 说明           | 类型         |
| --------------- | -------------- | ------------ |
| avatar          | 标签头         | Widget(Icon) |
| lebel           | 标签内容       | Widget(Text) |
| labelStyle      | 标签文字样式   | TextStyle    |
| labelPadding    | 标签内容内间距 | EdgeInsets   |
| deleteIcon      | 标签尾         | Widget(Icon) |
| onDeleted       | 标签尾点击回调 | Function     |
| deleteIconColor | 标签尾颜色     | Color        |
| backgroundColor | 背景颜色       | Color        |
| padding         | 内间距         | EdgeInsets   |
| elevation       | 阴影大小       | double       |
| shadowColor     | 阴影颜色       | Color        |

如果想让多个标签统一样式，使用`ChipTheme`包装多个`Chip`

```dart
ChipTheme(
	//统一设置 Chip 的属性
  data: ChipThemeData(
    backgroundColor:Colors.grey,
    labelPadding:EdgeInsets.fromLTRB(10, 0, 10, 0),
    //...
  ),
  child: Wrap(
    spacing: 8.0,
    runSpacing: 10.0, 
    children: <Widget>[
      Chip(label: Text('label1')),
      Chip(label: Text('label2')),
    ],
  ), 
),
```

### Card

**一张卡片**，默认圆角加阴影

| 属性        | 说明     | 类型            |
| ----------- | -------- | --------------- |
| child       | 子 Widget | Widget          |
| margin      | 外边距   | EdgeInsets      |
| shape       | 卡片形状 | **ShapeBorder** |
| shadowColor | 阴影颜色 | Color           |
| elevation   | 阴影大小 | double          |

这里的 shape 属性可以是`ShapeBorder`的任一实现如圆角`RoundedRectangleBorder(borderRadius: BorderRadius.circular(14.0))`



## 可滚动 Widget

### ListView

**一个列表**，在列表中列表项的宽占满容器，高度默认自适应。**列表不能嵌套列表**

| 属性            | 说明     | 类型       |
| --------------- | -------- | ---------- |
| children        | 列表项   | Widget[]   |
| padding         | 内边距   | EdgeInsets |
| resolve         | 是否反序 | bool       |
| scrollDirection | 滚动方向 | enum Axis  |

`ListTile`是列表项的一个实现，通常作为 ListView 的子使用，但也能单独使用，接收属性如下

| 属性              | 说明          | 类型         |
| ----------------- | ------------- | ------------ |
| leading/trailing  | 项头尾        | Widget       |
| title/subTitle    | 主次标题      | Widget(Text) |
| onTap/onLongPress | 点击/长按回调 | Function     |
| enable            | 是否可点击    | bool         |

`ListView.builder`命名构造函数用于迭代生成列表，值得注意的是该方法并**不是直接对数据源进行迭代，而是生成一系列下标供索引**

- `physics: ScrollPhysics`-滚动物理特性，原生提供的 physics 包括
  - **AlwaysScrollableScrollPhysics()**-总是可以滑动
  - **NeverScrollableScrollPhysics()**-禁止滚动
  - **BouncingScrollPhysics()**-内容超过一屏 上拉有回弹效果
  - **ClampingScrollPhysics()**-包裹内容 不会有回弹

- `itemCount: int`-列表项数量，决定了`itemBuilder`中下标的范围
- `itemExtend: double`-列表项高度，不设置则自适应
- `itemBuilder: Function`-该函数传入两个参数 context（上下文）和 index（下标），并返回一个 Widget 对应列表一个项
- `controller: ScrollController`-滚动控制器，用于监听滚动

若想在列表项之间引入分隔项，可以使用`ListView.separated`命名构造函数，使用方法同 builder，但额外提供`separatorBuilder`参数作为分隔项的构造器

注意：`ListView`默认构造函数为一次性渲染，**建议数据量大的时候使用其命名构造函数**，利用其懒加载特性优化性能

### GridView

**一个网格列表**，使用`GridView.count`命名构造函数来创建网格布局，在网格布局中**网格项的宽高设置是无效的**，其宽平分容器，其高默认自适应或由`crossAspectRatio`（宽高比）算得

| 属性             | 说明                 | 类型       |
| ---------------- | -------------------- | ---------- |
| children         | 网格项               | Widget[]   |
| crossAxisSpacing | 水平项间距           | double     |
| mainAxisSpacing  | 垂直项间距           | double     |
| crossAxisCount   | 一行多少项（多少列） | int        |
| crossAspectRatio | 项宽高比             | double     |
| padding          | 内边距               | EdgeInsets |
| resolve          | 是否反序             | bool       |
| scrollDirection  | 滚动方向             | enum Axis  |

`GridView.builder`使用方法和同`ListView.builder`。但是**使用 builder 会使 axisCount 之类的顶级布局失效**，为了修复这个问题需要对布局进行封装并赋值给`gridDelegate`属性

```dart
gridDelegate: SilverGridDelegateWithFixedCrossAxisCount(
	crossAxisSpacing: 10.0,
	crossAxisCount: 3,
)
```

###  滚动监听

使用`ScrollController`创建一个滚动控制器对象，作为`ListView.builder`的`controller`属性的值，构造时接收两个参数

- `initialScrollOffset: int`-初始化滑动距离
- `keepScrollOffset: bool`-是否保存滑动距离，默认 true

滚动控制器对象常用方法

| 方法                            | 含义                       |
| ------------------------------- | -------------------------- |
| addListener(callback: Function) | 监听滑动                   |
| jumpTo/animateTo(offset: int)   | 滚动到指定位置（有无动画） |
| dispose()                       | 记得释放控制器             |

控制器上有一个`ScrollPosition`类型的`position`属性，用于在回调中获取当前滚动的详细信息

- `position.pixels`-滑动距离
- `position.maxScrollExtent`-最大可滑动距离，即滑动内容长度
- `position.viewportDimension`-可视区域长度

___

除了使用控制器，滚动也可以通过**捕捉滚动事件**实现，该事件的类型为`ScrollNotification`，使用`NotificationListener`Widget 来捕捉事件，滚动事件的回调参数类型为`ScrollNotification`，它包括一个`metrics`属性，它的类型是`ScrollMetrics`，内含当前滚动的信息，具体属性如下

- `metrics.pixels`-滑动距离
- `metrics.maxScrollExtent`-最大可滑动距离
- `metrics.extentBefore`-可视区顶部距离内容顶部的距离
- `metrics.extentInside`-可视区域长度
- `metrics.extentAfter`-可视区底部距离内容底部的距离
- `metrics.atEdge`-是否滑到了容器边界（顶或底）

```dart
@override
Widget build(BuildContext context) {
  return Scrollbar( //进度条
    // 监听滚动通知
    child: NotificationListener<ScrollNotification>(
      onNotification: (ScrollNotification notification) {
        double progress = notification.metrics.pixels /
          notification.metrics.maxScrollExtent;
        //重新构建
        setState(() {
          _progress = "${(progress * 100).toInt()}%";
        });
        print("BottomEdge: ${notification.metrics.extentAfter == 0}");
        //return true; //放开此行注释后，进度条将失效
      },
      child: Stack(
        alignment: Alignment.center,
        children: <Widget>[
          ListView.builder(
            itemCount: 100,
            itemExtent: 50.0,
            itemBuilder: (context, index) {
              return ListTile(title: Text("$index"));
            }
          ),
          CircleAvatar(  //显示进度百分比
            radius: 30.0,
            child: Text(_progress),
            backgroundColor: Colors.black54,
          )
        ],
      ),
    ),
  );
}
```



## 布局 Widget

Flutter 中的布局 Widget 赋予子 Widget 以布局规则

### Padding

**内边距**

| 属性    | 说明     | 类型       |
| ------- | -------- | ---------- |
| child   | 子 Widget | Widget     |
| padding | 内边距   | EdgeInsets |

另：Widget 间的间距还能使用`SizedBox`简单实现

### Row&Column

这两个布局**对应 Flex 布局中的两种 flex-direction**，其属性如下

| 属性               | 说明           | 类型                    |
| ------------------ | -------------- | ----------------------- |
| children           | 子 Widget       | Widget[]                |
| mainAxisAlignment  | 主轴对齐方式   | enum MainAxisAlignment  |
| crossAxisAlignment | 交叉轴对齐方式 | enum crossAxisAlignment |

跟这两个 Widget 配合使用的还有 `Expanded`（作为 Row 和 Column 的子 Widget），**对应 Flex 布局中的 flex-grow**，其属性如下

| 属性  | 说明        | 类型   |
| ----- | ----------- | ------ |
| child | 子 Widget    | Widget |
| flex  | 同 flex-grow | int    |

### Stack

**让子 Widget 相对于父容器进行堆叠布局**，即绝对定位布局（后面的 Widget 覆盖前面的 Widget）

| 属性      | 说明                   | 类型                           |
| --------- | ---------------------- | ------------------------------ |
| children  | 子 Widget               | Widget[]                       |
| alignment | 对齐方式(作用于全部子) | Alignment(x,y), enum Alignment |

在 Stack 中使用`Align`和`Position`（作为 Stack 的子 Widget）来为每个子 Widget 单独指定位置

**`Align`：比例定位/四角四边定位**

| 属性      | 说明     | 类型                           |
| --------- | -------- | ------------------------------ |
| child     | 子 Widget | Widget                         |
| alignment | 对齐方式 | Alignment(x,y), enum Alignment |

上面的`Alignment(x,y)`的 x 和 y 范围在[-1,1]，(0,0)是绝对居中

**`Positioned`：精确定位**

| 属性                  | 说明     | 类型   |
| --------------------- | -------- | ------ |
| child                 | 子 Widget | Widget |
| top/left/right/bottom | 定位     | int    |

### Wrap

**让子 Widget 进行流式布局**（空间不足时自动换行）

| 属性                   | 说明                | 类型           |
| ---------------------- | ------------------- | -------------- |
| children               | 子 Widget            | Widget[]       |
| direction              | 主轴方向            | enum Axis      |
| alignment/runAlignment | 主轴/交叉轴对齐方式 | enum Alignment |
| spacing/runSpacing     | 主轴/交叉轴间距     | int            |

### AspectRatio

**规定子 Widget 的宽高比**，**一方（高/宽）固定或占满容器，另一方（宽/高）由宽高比算得**，只有两个属性

- `child: Widget`-子 Widget

- `aspectRatio: double`-宽高比
