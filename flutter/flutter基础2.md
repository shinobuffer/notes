> 本文为 Flutter 系列的第二弹，意在继续介绍入门 Flutter 时常用基础 Widget（**交互 Widget 篇**）及其常用属性，如果对某个 Widget 有深入研究需要或需要了解更多 Widget，可以参考[老孟的文章](http://laomengit.com/flutter/widgets/widgets_structure.html)

## 按钮 Widget

### 普通按钮家族

**按钮都无法设置宽高，可以通过外层嵌套 Container 实现**

- `RaisedButton`-突起按钮
- `FlatButton`-扁平按钮
- `OutlineButton`-线框按钮

以 Raisedbutton 为例（另外两个用法一样）

| 属性                        | 说明                     | 参数               |
| --------------------------- | ------------------------ | ------------------ |
| child                       | 按钮体                   | Widget             |
| onPressed                   | 点击回调，**为空则禁用** | Function           |
| color/disabledColor         | 常态/禁用态按钮颜色      | Color              |
| textColor/disabledTextColor | 常态/禁用态按钮文字颜色  | Color              |
| splashColor                 | 涟漪颜色                 | Color              |
| highlightColor              | 长按颜色                 | Color              |
| elevation                   | 阴影范围                 | double             |
| shape                       | 按钮形状（圆角等）       | **OutlinedBorder** |

`RaisedButton.icon`命名构造函数用于在突起按钮的最前添加一个图标

`OutlinedBorder`用于表示形状，原生提供`CircleBorder`、`RoundedRectangleBorder`等

### 其他按钮

`IconButton`图标按钮

- `icon: Icon`-图标
- `onPressed: Function`-点击回调

`FloatingActionButton`浮动按钮，通常用在 Scaffold 的`floatingActionButton`属性上

| 属性            | 说明     | 参数           |
| --------------- | -------- | -------------- |
| child           | 按钮体   | Widget(Icon)   |
| backgroundColor | 背景色   | Color          |
| onPressed       | 点击回调 | Function       |
| elevation       | 阴影范围 | double         |
| shape           | 按钮形状 | OutlinedBorder |

`InkWell`和`Ink`为涟漪按钮，区别为前者涟漪绘制在容器上（超出子圆角边框），后者涟漪绘制在子上（不超出子圆边框）

| 属性           | 说明     | 参数     |
| -------------- | -------- | -------- |
| child          | 按钮体   | Widget   |
| onTap          | 点击回调 | Function |
| splashColor    | 涟漪颜色 | Color    |
| highlightColor | 长按颜色 | Color    |

### Flutter2 按钮变动

废弃`RaisedButton`，推荐`ElevatedButton`代替

| 属性      | 说明                     | 参数            |
| --------- | ------------------------ | --------------- |
| child     | 按钮体                   | Widget          |
| onPressed | 点击回调，**为空则禁用** | Function        |
| style     | 按钮样式                 | **ButtonStyle** |

ButtonStyle 的部分属性

- `backgroundColor: MaterialStateProperty<Color>`-背景色
- `foregroundColor: MaterialStateProperty<Color>`-前景色（字色）
- `elevation: MaterialStateProperty<double>`-阴影范围
- `shape: MaterialStateProperty<OutlinedBorder>`-按钮形状

这里的 MaterialStateProperty 指点击事件所有状态下的样式，ButtonStyle 下的所有属性都需要用 MaterialStateProperty 进行包裹

废弃`FlatButton`，推荐`TextButton`代替

废弃`OutlineButton`，推荐`OutlinedButton`代替



## 表单 Widget

### TextField

**文本框，[超详细解析](https://blog.csdn.net/yuzhiqiang_1993/article/details/88204031)**

| 属性               | 说明               | 参数                  |
| ------------------ | ------------------ | --------------------- |
| maxLines/maxLength | 最大行/字数        | int                   |
| onChanged          | 改变回调           | Function              |
| obscureText        | 是否密码           | bool                  |
| decoration         | 文本框装饰         | **InputDecoration**   |
| keyboardType       | 键盘类型           | enum KeyboardType     |
| controller         | 控制器（数据绑定） | TextEditingController |

`InputDecoration`的属性

| 属性       | 说明        | 参数                   |
| ---------- | ----------- | ---------------------- |
| icon       | 图标        | Icon                   |
| hintText   | placeholder | String                 |
| border     | 边框        | **OutlineInputBorder** |
| labelText  | label 文案   | String                 |
| labelStyle | label 样式   | TextStyle              |

**控制器用来进行数据绑定**（设置 TextField 的 controller 属性），通过`controller.text`读写绑定值，使用`controller.addListener()`来监听被绑文本框的变动 

```dart
initState() {
	super.initState();
  _content = TextEditingController()
    ..addListener((){
      // 通过 controller.text 读写文本内容
      _content.text = _content.text.toUpperCase();
    });
   
}

xxx(
	child: Colum(
  	children: [
      TextField(
      	controller: _content,
      ),
    ],
  ),
)
```

### CheckBox

**多选框**

| 属性        | 说明     | 参数     |
| ----------- | -------- | -------- |
| value       | 选中状态 | bool     |
| onChanged   | 改变回调 | Function |
| activeColor | 选中颜色 | Color    |
| checkColor  | ✔颜色    | Color    |

还有一个`CheckBoxListTile`，相当于 ListTile 版 checkbox，除了上面的属性还增加了如下属性

| 属性     | 说明                   | 参数   |
| -------- | ---------------------- | ------ |
| title    | 标题                   | Widget |
| subTitle | 二级标题               | Widget |
| selected | 选中后文字颜色是否跟随 | bool   |

### Radio

**单选框，拥有用一个 groupValue 的单选框组成一个组，同组单选框只能选中一个**

| 属性        | 说明                           | 参数       |
| ----------- | ------------------------------ | ---------- |
| value       | 唯一 id                         | int/String |
| onChanged   | 选中回调                       | Function   |
| groupValue  | 绑定一个变量，用于表示选中的 id | var        |
| activeColor | 选中颜色                       | Color      |

同样的有`RadioListTile`，相当于相当于 ListTile 版 radio，其新增属性可参考 CheckBoxListTile

### 时间日期选择

Flutter 提供了两个异步方法调起时间和日期的弹窗供用户选择

日期选择弹窗`showDatePicker()`，返回`Future<DateTime|null>`，参数如下

- `context: BuilderContext`-上下文
- `initialDate: DateTime`-默认日期
- `firstDate: DateTime`-日期范围起点
- `lastDate: DateTime`-日期范围终点

时间选择弹窗`showTimePicker()`，返回`Future<TimeOfDay|null>`，参数如下

- `context: BuilderContext`-上下文
- `initialDate: TimeOfDay`-默认时间



## 弹窗

> 调起弹窗将往路由栈中推入一个弹窗，因此需要**调用`Navigation.pop(context, value)`收起弹窗/向外传递数据**

调用`showDialog()`异步方法来调起一个**悬浮弹窗**，返回`Future<value>`，其参数如下

- `context: BuilderContext`-上下文
- `builder: Function`-弹窗构造器，Flutter 提供`AlertDialog`和`SipleDialog`两种现成弹窗类型，详见下面

`AlertDialog`普通弹窗

| 属性            | 说明     | 参数         |
| --------------- | -------- | ------------ |
| title           | 弹窗标题 | Widget(Text) |
| content         | 弹窗内容 | Widget(Text) |
| actions         | 弹窗按钮 | Widget[]     |
| backgroundColor | 背景颜色 | Color        |
| elevation       | 阴影     | double       |

`SimpleDialog`多选一弹窗

| 属性            | 说明     | 参数         |
| --------------- | -------- | ------------ |
| title           | 弹窗标题 | Widget(Text) |
| children        | 弹窗体   | Widget[]     |
| backgroundColor | 背景颜色 | Color        |
| elevation       | 阴影     | double       |

调用`showModalBottomSheet()`方法来调起一个**贴底弹窗**，使用方法同`showDialog()`

___

可以继承`Dialog`类并实现`build()`实现自定义弹窗，供`showDialog()`方法使用

```dart
class MyDialog extends Dialog {
	@override
  Widget build(BuildContext context) {
    return Material(
    	type: MaterialType.transparency,
      child: DialogBody() // 自定义弹窗体
    );
  }
}
```

