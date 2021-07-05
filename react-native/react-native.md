## 常用组件
### StyleSheet
一种类似 CSS 样式表的抽象，将样式统一管理。使用时将对应的样式放入组件的style属性。组件的style属性可以接收样式对象，也可以接收样式对象的数组，React会自动将样式合成（靠右的样式优先级高）。**RN的样式不会继承**
```jsx
import React from "react";
import { StyleSheet, View } from "react-native";

const App = () => (
  <View style={styles.container}>
    <View style={[styles.container,{margin:10}]}></View>
  </View>
);
const styles = StyleSheet.create({
  container: {
    flex: 1,
    padding: 10,
    backgroundColor: "#eaeaea"
  }
});

export default App;
```
### View
[触摸机制](https://www.jianshu.com/p/98e0b21473be)  
不可滚动的视图，类似于web中的`div`。样式接受：布局、变换、边框、阴影、背景、透明度。  
View上可以申请并绑定触摸事件（未申请默认是不响应触摸事件的），RN会传入一个合成触摸事件给事件处理器。一个合成触摸事件具有以下格式
- nativeEvent
    - `changedTouches`：从上一次事件以来的触摸事件数组
    - `identifier`：触摸事件ID，对应于手指
    - `locationX、locationY` ：触点相对组件的位置
    - `pageX、pageY`：触点相对屏幕的位置
    - `target`：接收当前触摸事件的组件ID
    - `timestamp`：触摸事件时间戳
    - `touches`：屏幕上所有当前触摸事件的数组

触摸相关的事件props有：
- `hitSlop`：扩大视图触摸响应范围，接收`{top: 10, bottom: 10, left: 0, right: 0}`
- `pointerEvents`：参照css，接收num(auto,none,box-only,box-none)
- `onStartShouldSetResponder`：是否申请处理 touch start 事件，接收`(event)=>bool`（下同）
- `onStartShouldSetResponderCapture`：如果父视图要阻止子视图响应touch start，应返回true
- `onMoveShouldSetResponder`：是否申请处理 touch move 事件
- `onMoveShouldSetResponderCapture`：如果父视图要阻止子视图响应touch move，应返回true
- `onResponderTerminationRequest`：如果要放弃当前处理的事件，让给别的申请处理事件，应返回true
- `onResponderGrant`：申请成功回调，准备开始处理事件接收`(event)=>{}`（下同）
- `onResponderReject`：申请失败回调
- `onResponderStart`：事件响应开始回调
- `onResponderMove`：触摸移动回调
- `onResponderRelease`：触摸结束（touchup）回调
- `onResponderEnd`：事件响应结束回调
- `onResponderTerminate`：事件处理被终止回调
- `onLayout`：当元素挂载或者布局改变的时候调用，参数为`{nativeEvent:{layout: {x, y, width, height}}}`
```jsx
const App = () => (
  <View style={styles.container}>
    <View style={[styles.container,{margin:10}]}></View>
  </View>
);
```

### Text
用于显示文本。样式接受：text\*、font\*、颜色、行高。  
相关props
- `selectable`：接收bool（默认false），是否可以选中
- `numberOfLines`：接收num，限定最大行数
- `ellipsizeMode`：接收enum(head, middle, tail, clip)，超出行数限制后省略的方式
- `onPress/onLongPress`：按/长按事件回调
- `onLayout`

### Image
用于显示图片。样式接受：布局、变换、边框、阴影、背景、透明度、resizeMode  
相关props
- `source`：接收`{ uri: string }`，图源
- `defaultSource`：接收`{ uri: string }`，占位图
- `capInsets`：
- `onLayout`
- `onLoad`：加载成功回调
- `onError`：加载失败回调，参数为`{nativeEvent: {error}}`
- `onLoadStart`：加载成功回调
- `onLoadEnd`：加载结束回调（无论成功与否）
- `onProgress`：在加载过程中不断调用，参数为`{nativeEvent: {loaded, total}}`

### TextInput
用于文本输入。不支持分别指定边框颜色/宽度样式  
相关props
- `value`：接收string，文本内容
- `defaultValue`：接收string，初始文本内容
- `editable`：接收bool（默认true），是否可以编辑
- `maxLength`：接收num，限制文本字数
- `multiline`：接收bool（默认false），是否多行
- `numberOfLines`：接收num，多行模式下的最大行数
- `secureTextEntry`：接收bool（默认false），是否隐藏输入
- `keyboardType`：接收enum(default, numeric, password, email, phone-pad)，弹出软盘类型
- `returnKeyType`：接收enum(done, go, next, search, send)，软盘回车显示样式
- `placeholder`：接收string，略
- `placeholderColor`：略
- `autoFocus`：接收bool（默认false），是否自动获得焦点
- `onBlur`：失去焦点回调
- `onChangeText`：文本内容变化回调，参数为当前文本内容
- `onSelectionChange`：文本选中范围变化回调，参数为`{nativeEvent: {selection: {start, end}}`
- `onLayout`
- `onEndEditing`

### ScrollView
可滚动视图。样式接受：布局、变换、边框、阴影、背景、透明度。该组件需要**父容器有确定的尺寸**才能工作。  
相关props
- `contentContainerStyle`：滚动视图内容器样式，不要在此固定高度，否则无法正常滚动
- `horizontal`：接收bool（默认false），子元素是否水平排列滚动
- `showsHorizontalScrollIndicator`：接收bool，是否显示水平滚动条（也有竖直的）
- `scrollEnabled`：接收bool（默认true），是否允许滚动
- `scrollEventThrottle`：接收num，滚动回调频率
- `onScroll`：滚动过程不断调用
- `onMomentumScrollBegin`：滚动开始回调
- `onMomentumScrollEnd`：滚动结束回调

### TouchableOpacity
将视图封装使其可以响应触摸操作。当视图被按下，透明度降低。样式接受：边框
相关props
- `activeOpacity`：接收num（默认0.2），按下时透明度
- `onPress`：点击回调
- `onPressIn`：点击开始回调
- `onPressOut`：点击结束回调
- `onLongPress`：长按回调

同系列的组件还有以下几个，都能提供按钮封装功能，只是点击特效不一样
- `TouchableHighlight`-按下降低透明度的同时能让一个底层颜色（通过`underlayColor`props设置）透过，该组件只允许一个子节点
- `TouchableNativeFeedback`-按下涟漪扩散特效，该组件只允许一个子节点
- `TouchableWithoutFeedback`-无特效，该组件只允许一个子节点

### FlatList
列表组件，兼容`ScrollView`的props和样式，特点是只渲染可视区域数据。
注：子项离开渲染区不会保留内部状态；该组件继承自`PureComponent`使用浅层比较决定是否重渲染，注意引用类型的props数据
相关props
- `data`：接收Array，要渲染的数据集
- `extraData`：接收Array，额外的数据集
- `renderItem`：接收Function，将数据项映射为组件的函数，形状：`(item,index)=>component`
- `ListEmptyComponent`：接收component，数据集空时显示的组件
- `List(Header|Footer)Component`：接收component，头部/尾部列表项组件
- `horizontal`：接收bool（默认false），是否水平排布
- `getItemLayout`：接收Function，可选优化，提前告知每个子元素的偏移信息（前提时每个子元素高度固定已知），形状`(data, index) => {length: number, offset: number, index: number}`
- `initialNumToRender`：接收num，首次渲染的子项个数
- `initialScrollIndex`：接收num，首次渲染从第n个子项开始
- `keyExtractor`：接收Function，从数据项中提取key，形状`(item: object, index: number) => string;`
- `onEndReachedThreshold`：接收num（0-1），触底阈值，是个比值，当离底部还有`t*容器高度`是触发触底
- `onEndReached`：触底回调
- `onRefresh`：刷新回调，如果设置了该props，可以下拉刷新

### SectionList
分组列表组件，兼容`ScrollView`的props和样式，特点数据分组。
注：子项离开渲染区不会保留内部状态；该组件继承自`PureComponent`使用浅层比较决定是否重渲染，注意引用类型的props数据
相关props
- `sections`：接收Array，形状`[{data:Array,...}]`，每个数据项对应一个组
- `renderSection(Header|Footer)`：接收Function，每个组的头尾组件渲染函数，形状：`(section,index)=>component`
- 其他props基本与FlatList一致

### Modal
自定义弹窗容器，里面放自定义组件
相关props
- `visible`：接收bool，是否显示弹窗。
- `transparent`：接收bool，背景是否透明（默认白色）
- `animationType`：接收enum('none', 'slide', 'fade')，弹窗动画类型
- `onShow`：弹窗显示回调
- `onDismiss`：弹窗消失回调
## 常用API
### Alert
提示对话框。主要一个方法`Alert.alert(title,message?,buttons?,options?)`，参数如下
- `title`：接收string，对话框标题
- `message`：接收string，对话框内容
- `buttons`：最多设置三个按钮，取消按钮需特定，如`[{text:'取消',onPress,style:'cancel'},{text:'OK',onPress}]`

## 动画
动画的本质就是属性值的渐变，在RN中我们称这类渐变的属性值为动画值。RN提供两个方法创建一个动画值
- `Animate.Value(num)`创建一个标量动画值
- `Animate.ValueXY({x,y})`创建一个矢量动画值

动画值的修改（渐变）有专门的方法，这里介绍最常用的`Animated.timing(value,config)`方法，执行该方法创建一个动画
- `value`-要修改的动画值
- `config`-动画配置，该对象包含以下
  - `duration`：持续时间单位ms，默认500
  - `easing`：缓动函数，默认`Easing.inOut(Easing.ease)`
  - `delay`：延迟，单位ms，默认0
  - `isInteraction`：指定本动画是否在InteractionManager的队列中注册以影响其任务调度，默认是
  - `useNativeDriver`：是否原生动画驱动，默认否

通过在动画上调用`start()`来启动动画。`start()`可以传入一个回调函数，以便在动画完成时得到通知调用，回调函数会收到一个`{finished}`的值表示动画函数是否正常完成。

动画值需要配套动画组件使用，`Animated`中默认提供了`View`、`Text`、`Image`、`ScrollView`、`FlatList`、`SectionList`六种直接使用的动画组件。如果需要其他动画组件可以使用`createAnimatedComponent()`自己封装。下面是一个简单例子
```js
class Box extend Component{
  state = {
    boxHeight:new Animated.Value(0)
  }

  addHeight = ()=>{
    Animated.timing(this.state.boxHeight, {
      toValue: 100,
      duration: 5000
    }).start();
  }

  render(){
    return (
      <>
        <Animated.View style={[style.view,{height:this.state.boxHeight}]} />
        <Button onPress={this.addHeight}/>
      </>
    )
  }
}
const style = StyleSheet.create({
  view:{
    backgroundColor:'black',
  }
})
```


## 路由导航
>yarn add react-navigation

主要有以下navigator
```jsx
import {
  StackNavigator,
  TabNavigator,
  DrawerNavigator,
} from 'react-navigation';
```
`StackNavigator`提供了一种页面切换的方法，每次切换时，新的页面会放置在堆栈的顶部。
`StackNavigator`接收一个对象，每个key对应一个页面，详细属性如下。当组件被注册到navigator后，组件的props会有一个`navigation`对象，使用`navigation.navigate(name,params?)`来编程式导航，`params`用于向路由传参，接收方通过`navigation.state.params`获取传入的参数
```jsx
//假设有Home和Details两个组件
const Home = ({ navigation }) => (
  <View style={{ flex: 1, alignItems: 'center', justifyContent: 'center' }}>
    <Text>Home Page</Text>
    <Button
      onPress={() => navigation.navigate('Details')}
      title="Go to details"
    />
  </View>
);

const RootNavigator = StackNavigator({
  Home: {//Home页面
    screen: Home,//对应组件
    navigationOptions: {
      headerTitle: 'Home',//导航栏标题
    },
  },
  Details: {
    screen: Details,
    navigationOptions: {
      headerTitle: 'Details',
    },
  },
});
export default RootNavigator;
```
完整API为`StackNavigator(RouteConfigs, StackNavigatorConfig?)`

`StackNavigatorConfig`用于配置路由行为，有以下配置：
- `router`选项
    - `initialRouteName` - 设置堆栈的默认页面。对应RouteConfigs中的一个key。
    - `initialRouteParams` - 初始路由参数
    - `navigationOptions` - 用于页面的默认导航选项
    - `paths` - 用于覆盖RouteConfigs中设置的path的一个映射
- `mode` - 页面切换模式：标准`'card'`，底部滑入`'modal'`（IOS）
- `headerMode` - 标题切换模式：切换时动画`'float'`、随页面淡入淡出`'screen'`、无标题栏`'none'`
- `onTransition(Start|End)`：切换动画开始/结束回调

___
`TabNavigator`提供顶部或底部tab导航栏，类似的`TabNavigator`接收一个对象
```jsx
//假设有Home组件
const RootTabs = TabNavigator({
  Home: {//Home页面
    screen: Home,//对应组件
    navigationOptions: {
      tabBarLabel: 'Home',//导航项标题
      tabBarIcon: //({tintColor, focused})=>component，导航栏图标组件
    },
  }
});

export default RootTabs;
```
完整API为`TabNavigator(RouteConfigs, TabNavigatorConfig?)`
[TabNavigatorConfig](https://www.reactnavigation.org.cn/docs/tabnavigator#tabnavigatorconfig)
___
`DrawerNavigator`提供侧边抽屉式导航栏，类似的`DrawerNavigator`接收一个对象。使用`navigation.navigate('Drawer(Open|Close|Toggle)')`打开、关闭、切换抽屉
```jsx
//假设有Home组件
const RootDrawer = DrawerNavigator({
  Home: {//Home页面
    screen: Home,//对应组件
    navigationOptions: {
      drawerLabel: 'Home',//导航项标题
      drawerIcon: //({tintColor, focused})=>component，导航栏图标组件
    },
  }
});

export default RootDrawer;
```
完整API为`DrawerNavigator(RouteConfigs, DrawerNavigatorConfig?)`
[DrawerNavigatorConfig](https://www.reactnavigation.org.cn/docs/drawernavigator#drawernavigatorconfig)

