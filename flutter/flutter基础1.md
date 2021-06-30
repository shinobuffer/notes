本文为Flutter系列的第一弹，意在介绍入门Flutter时常用基础组件（**展示组件篇**）及其常用属性，如果对某个组件有深入研究需要或需要了解更多组件，可以参考[老孟的文章](http://laomengit.com/flutter/widgets/widgets_structure.html)

## 起步

Flutter项目的源码放在`lib`目录中，默认的入口文件为`lib/main.dart`，入口函数为`main`函数。调用`runApp(widget)`方法启动一个App，该方法接受一个`Widget`作为根Widget

在Flutter中一个重要的概念叫`Widget`，他是**功能的抽象描述**。对于学过RN的同学，你可以粗略地认为组件在Flutter中就是一种Widget（实际上Widget的范围更广，不一定要对应可视化的UI，继续深入学习后会懂的）

Flutter核心库为`flutter/material.dart`，提供了丰富的Material Widget供开发者使用，以此来实现安卓和IOS风格的一致性



## 自定义Widget

了解内置Widget之前来了解下自定义Widget，自定义Widget分无状态和有状态两种，**无状态Widget中的状态（属性）无法改变**，对应初始化后不会变动的UI；**有状态Widget中的状态（属性）可以在Widget生命周期中变化并响应式更新视图。**

- 继承`StatelessWidget`并实现`build`方法来创建一个无状态Widget，该方法返回你要封装的Widget

- 继承`StatefulWidget`并实现`createState`方法来创建一个有状态Widget。`createState`方法需要返回一个State，因此还需要指定Widget的State，通过继承`State<YourWidget>`并实现`build`方法来创建这个State，该方法返回你要封装的Widget。在State中你可以设置需要的状态并调用`setState`方法更新状态。

```dart
// 无状态Widget
class Footer extends StatelessWidget {
  @override
  Widget build(BuildContext ctx) {
    return Center(
    	child: Text('Footer'),
    );
  }
}

// 有状态Widget
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



## 基础Widget

###  Center

用于居中布局，可以**将子Widget水平垂直居中**

| 属性  | 说明     | 类型   |
| ----- | -------- | ------ |
| child | 子Widget | Widget |

### Container

一个**容器**，只能容纳一个子Widget。**能够设置宽高**，对于不能设置宽高的Widget，可以外套一个Container来设置宽高

| 属性       | 说明               | 类型                           |
| ---------- | ------------------ | ------------------------------ |
| alignment  | 子Widget对齐方式   | enum Alignment                 |
| decoration | 背景、边框相关设置 | BoxDecoration                  |
| child      | 子Widget           | Widget                         |
| height     | 高                 | double                         |
| width      | 宽                 | double                         |
| margin     | 外边距             | EdgeInsets.all/fromLTRB/only() |
| padding    | 内边距             | EdgeInsets.all/fromLTRB/only() |
| transform  | 变换               | Matrix4.xxx()                  |

BoxDecoration的属性

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

| 属性（无序号为命名参数，下同） | 说明     | 类型               |
| ------------------------------ | -------- | ------------------ |
| ①content                       | 文本内容 | String             |
| textAlign                      | 对齐方式 | enum TextAlign     |
| textDirection                  | 文本方向 | enum TextDirection |
| overflow                       | 超出处理 | enum TextOverflow  |
| textScaleFactor                | 放大倍率 | double             |
| maxLines                       | 最大行数 | int                |
| style                          | 字体样式 | TextStyle          |

TextStyle的属性

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

加载本地图片需要配置资源目录，Flutter**建议为每张图片准备3种规格**，以让Flutter能根据设备选择最好图片

1. 创建一个目录`images`（可其他名字）作为资源根目录
2. 创建目录`2.0x`和`3.0x`，放入对应规格的图片，原图放在跟`2.0x`/`3.0x`目录同级位置，三张图片同名
3. 编辑`pubspec.ymal`在`flutter.assets`字段设置资源根目录位置如`images`
4. 通过`'images/.../img.png'`引用图片

### ClipOval

**将子Widget裁剪成椭圆**（默认根据子的宽高裁剪）

| 属性         | 说明     | 类型        |
| ------------ | -------- | ----------- |
| child        | 子Widget | Widget      |
| fit          | 填充方式 | enum BoxFit |
| height/width | 高宽     | double      |

他的兄弟Widget：`ClipRRect`圆角矩形裁剪、`ClipRect`矩形裁剪

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
	//统一设置Chip的属性
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

| 属性        | 说明     | 类型        |
| ----------- | -------- | ----------- |
| child       | 子Widget | Widget      |
| margin      | 外边距   | EdgeInsets  |
| shape       | 卡片形状 | ShapeBorder |
| shadowColor | 阴影颜色 | Color       |
| elevation   | 阴影大小 | double      |

这里的shape属性可以是`ShapeBorder`的任一实现如圆角`RoundedRectangleBorder(borderRadius: BorderRadius.circular(14.0))`



## 可滚动Widget

### ListView

**一个列表**，在列表中列表项的宽占满容器，高度默认自适应。**列表不能嵌套列表**

| 属性            | 说明     | 类型       |
| --------------- | -------- | ---------- |
| children        | 列表项   | Widget[]   |
| padding         | 内边距   | EdgeInsets |
| resolve         | 是否反序 | bool       |
| scrollDirection | 滚动方向 | enum Axis  |

`ListTile`是列表项的一个实现，通常作为ListView的子使用，但也能单独使用，接收属性如下

| 属性              | 说明          | 类型         |
| ----------------- | ------------- | ------------ |
| leading/trailing  | 项头尾        | Widget       |
| title/subTitle    | 主次标题      | Widget(Text) |
| onTap/onLongPress | 点击/长按回调 | Function     |
| enable            | 是否可点击    | bool         |

`ListView.builder`命名构造函数用于迭代生成列表，值得注意的是该方法并不是直接对数据源进行迭代，而是生成一系列下标供索引

- `itemCount: int`-列表项数量，决定了`itemBuilder`中下标的范围
- `itemExtend: double`-列表项高度，不设置则自适应
- `itemBuilder: Function`-该函数传入两个参数context（上下文）和index（下标），并返回一个Widget对应列表一个项
- `controller: ScrollController`-滚动控制器，用于监听滚动

### GridView

**一个网格列表**，使用`GridView.count`命名构造函数来创建网格布局，在网格布局中**网格项的宽高设置是无效的**，其宽平分容器，其高默认自适应或由`crossAspectRatio`（宽高比）算得

| 属性             | 说明       | 类型       |
| ---------------- | ---------- | ---------- |
| children         | 网格项     | Widget[]   |
| crossAxisSpacing | 水平项间距 | double     |
| mainAxisSpacing  | 垂直项间距 | double     |
| crossAxisCount   | 一行多少项 | int        |
| crossAspectRatio | 项宽高比   | double     |
| padding          | 内边距     | EdgeInsets |
| resolve          | 是否反序   | bool       |
| scrollDirection  | 滚动方向   | enum Axis  |

`GridView.builder`使用方法和同`ListView.builder`。但是**使用builder会使axisCount之类的顶级布局失效**，为了修复这个问题需要对布局进行封装并赋值给`gridDelegate`属性

```dart
gridDelegate: SilverGridDelegateWithFixedCrossAxisCount(
	crossAxisSpacing: 10.0,
	crossAxisCount: 3,
)
```

###  滚动监听

使用`ScrollController`创建一个滚动控制器对象，作为`ListView.builder`的`controller`属性的值，构造时接收两个参数

- `initialScrollOffset: int`-初始化滑动距离
- `keepScrollOffset: bool`-是否保存滑动距离，默认true

滚动控制器对象常用方法

| 方法                            | 含义                       |
| ------------------------------- | -------------------------- |
| addListener(callback: Function) | 监听滑动                   |
| jumpTo/animateTo(offset: int)   | 滚动到指定位置（有无动画） |
| dispose()                       | 记得释放控制器             |

控制器上有一个`ScrollPosition`类型的`position`属性，用于在回调中获取当前滚动的详细信息

- `position.pixels`-滑动距离
- `position.maxScrollExtent`-最大可滑动距离，即滑动组件内容长度
- `position.viewportDimension`-可视区域长度

___

除了使用控制器，滚动也可以通过**捕捉滚动事件**实现，该事件的类型为`ScrollNotification`，使用`NotificationListener`Widget来捕捉事件，滚动事件的回调参数类型为`ScrollNotification`，它包括一个`metrics`属性，它的类型是`ScrollMetrics`，内含当前滚动的信息，具体属性如下

- `pixels`：滑动距离。
- `maxScrollExtent`：最大可滑动距离
- `extentBefore`：可视区顶部距离内容顶部的距离
- `extentInside`：可视区域长度
- `extentAfter`：可视区底部距离内容底部的距离
- `atEdge`：是否滑到了可滚动组件的边界（顶或底）

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



## 布局Widget

Flutter中的布局Widget赋予子Widget以布局规则

### Padding

**内边距**

| 属性    | 说明     | 类型       |
| ------- | -------- | ---------- |
| child   | 子Widget | Widget     |
| padding | 内边距   | EdgeInsets |

另：Widget间的间距还能使用`SizedBox`简单实现

### Row&Column

这两个布局**对应Flex布局中的两种flex-direction**，其属性如下

| 属性               | 说明           | 类型                    |
| ------------------ | -------------- | ----------------------- |
| children           | 子Widget       | Widget[]                |
| mainAxisAlignment  | 主轴对齐方式   | enum MainAxisAlignment  |
| crossAxisAlignment | 交叉轴对齐方式 | enum crossAxisAlignment |

跟这两个Widget配合使用的还有 `Expanded`（作为Row和Column的子Widget），**对应Flex布局中的flex-grow**，其属性如下

| 属性  | 说明        | 类型   |
| ----- | ----------- | ------ |
| child | 子Widget    | Widget |
| flex  | 同flex-grow | int    |

### Stack

**让子Widget相对于父容器进行堆叠布局**，类似于绝对定位布局（后面的Widget覆盖前面的Widget）

| 属性      | 说明                   | 类型                           |
| --------- | ---------------------- | ------------------------------ |
| children  | 子Widget               | Widget[]                       |
| alignment | 对齐方式(作用于全部子) | Alignment(x,y), enum Alignment |

在Stack中使用`Align`和`Position`（作为Stack的子Widget）来为每个子Widget单独指定位置

**Align：比例定位/四角四边定位**

| 属性      | 说明     | 类型                           |
| --------- | -------- | ------------------------------ |
| child     | 子Widget | Widget                         |
| alignment | 对齐方式 | Alignment(x,y), enum Alignment |

上面的`Alignment(x,y)`的x和y范围在[-1,1]，(0,0)是绝对居中

**Positioned：精确定位**

| 属性                  | 说明     | 类型   |
| --------------------- | -------- | ------ |
| child                 | 子Widget | Widget |
| top/left/right/bottom | 定位     | int    |

### Wrap

**让子Widget进行浮动布局**

| 属性                   | 说明                | 类型           |
| ---------------------- | ------------------- | -------------- |
| children               | 子Widget            | Widget[]       |
| direction              | 主轴方向            | enum Axis      |
| alignment/runAlignment | 主轴/交叉轴对齐方式 | enum Alignment |
| spacing/runSpacing     | 主轴/交叉轴间距     | int            |

### AspectRatio

**规定子Widget的宽高比**，**一方（高/宽）固定或占满容器，另一方（宽/高）由宽高比算得**。只有两个属性

- `child: Widget`-子Widget

- `aspectRatio: double`-宽高比
