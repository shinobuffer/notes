> 版本5.x，安装和更多使用参考[官网](https://reactnavigation.org/docs/getting-started)

## 试水（以StackNavigator为例）
### 基本使用
`createStackNavigator()`返回一个包含`Screen`和`Navigator`的对象，用于配置导航，`Screen`（用来配置路由）需要包含在`Navigator`中。
`NavigationContainer`组件用于组织所有导航结构，通常该组件用在app的最顶层。
```jsx
import { NavigationContainer } from '@react-navigation/native';
import { createStackNavigator } from '@react-navigation/stack';

function Home({ navigation }) {
  return (
    <View style={{ flex: 1, alignItems: 'center', justifyContent: 'center' }}>
      <Text>Home Screen</Text>
      <Button
        title="Go to Details"
        onPress={() => navigation.navigate('Details')}
      />
    </View>
  );
}
function Details() {
  return (
    <View style={{ flex: 1, alignItems: 'center', justifyContent: 'center' }}>
      <Text>Details Screen</Text>
    </View>
  );
}

const Stack = createStackNavigator();

function App() {
  return (
    <NavigationContainer>
      <Stack.Navigator initialRouteName="Home">
        <Stack.Screen name="Home" component={Home} options={{ title: 'Overview' }} />
        <Stack.Screen name="Details" component={Details} options={{ title: 'Details' }} />
        {/*往Screen传props来配置路由*/}
      </Stack.Navigator>
    </NavigationContainer>
  );
}

export default App;
```
被路由绑定的页面组件可以通过`props.navigation`取得`navigation`对象，通过`navigation.navigate(name,params?)`方法进行跳转，通过`navigation.goBack()`方法返回。
注：如果已经在目标路由，多次`navigate()`到该路由是无效的，如果需要忽略该限制可以使用`navigation.push(name,params?)`方法

`navigation.navigate(name,params?)`中的params对象用于跳转传参，在目标组件中通过`props.route.params`取得参数，通过`navigation.setParams()`来修改参数。`Screen`组件可以接收`initialParams`props来指定路由初始化参数。

### options
`Screen`组件接收`options`props来进行一些额外的配置，如头栏的配置。`options`接收一个对象，如果`options`需要依赖params/navigation，也可以接收一个返回对象的函数。
```jsx
return (
  <Stack.Navigator>
    <Stack.Screen
      name="Home"
      component={Home}
      options={{ title: 'My home' }}
      {/*options={({ route,navigation }) => ({ title: route.params.name })}*/}
    />
  </Stack.Navigator>
);
```
如果需要修改当前路由的`options`，可以使用`navigation.setOptions()`方法
```jsx
<Button
  title="Update the title"
  onPress={() => navigation.setOptions({ title: 'Updated!' })}
/>
```
一些修改头栏样式的`options`
- `headerStyle`: 头栏样式
- `headerTintColor`: 头栏标题和返回按钮的色调
- `headerTitleStyle`: 头栏标题样式（字体相关）
- `headerShown`：是否显示头栏

如果多个路由需要共享`options`，可以把这部分`options`上提到`Navigator`组件的`screenOptions`props。
```jsx
<Stack.Navigator
  screenOptions={{
    headerStyle: {
      backgroundColor: '#f4511e',
    },
    headerTintColor: '#fff',
  }}
>
  <Stack.Screen
    name="Home"
    component={Home}
    options={{ title: 'My home' }}
  />
</Stack.Navigator>
```
如果你想使用自定义组件代替头栏标题，可以使用`headerTitle`这个options，接收形如`props => <MyCom {...props} />`的函数。如果想在头栏两侧加自定义按钮，可以使用`header(Left|Right)`这个options，接收一个返回component的函数
```jsx
<Stack.Navigator>
  <Stack.Screen
    name="Home"
    component={Home}
    options={{
      headerTitle: props => <MyCom {...props} />,
      headerRight: () => (
        <Button
          onPress={() => alert('This is a button!')}
          title="Info"
          color="#fff"
        />
      ),
    }}
  />
</Stack.Navigator>
```
注：`options`中的`this`指向并不是被绑定的组件，如果需要在这些按钮中与组件互动，按钮应该在组件内通过`navigation.setOptions()`进行设置
```jsx
function Home({ navigation }) {
  const [count, setCount] = React.useState(0);

  React.useLayoutEffect(() => {
    navigation.setOptions({
      headerRight: () => (
        <Button onPress={() => setCount(c => c + 1)} title="Update count" />
      ),
    });
  }, [navigation]);

  return <Text>Count: {count}</Text>;
}
```

默认情况下`HeaderLeft`预留给了默认的返回按钮，显示为一个左箭头和上一个路由的标题（当标题过长时以‘back’代替），当然你可以重写`HeaderLeft`。

### 嵌套导航
一个导航组件作为另一个导航的`Screen`，这就是嵌套导航，如下
```jsx
function Home() {
  return (
    <Tab.Navigator>
      <Tab.Screen name="Feed" component={Feed} />
      <Tab.Screen name="Messages" component={Messages} />
    </Tab.Navigator>
  );
}

function App() {
  return (
    <NavigationContainer>
      <Stack.Navigator>
        <Stack.Screen name="Home" component={Home} />
        <Stack.Screen name="Profile" component={Profile} />
        <Stack.Screen name="Settings" component={Settings} />
      </Stack.Navigator>
    </NavigationContainer>
  );
}
```
Home组件包含了一个tab导航，但同时在App组件中它也作为stack导航的Screen，形成如下导航结构
- `App`(Stack.Navigator)
  - `Home` (Tab.Navigator)
    - `Feed` (Screen)
    - `Messages` (Screen)
  - `Profile` (Screen)
  - `Settings` (Screen)

注意以下的点：
- 精良减少嵌套层数，避免错误嵌套
- 每个导航维护着自己的历史
- 每个导航维护着自己的`options`，不会相互干扰
- 每个Screen维护着自己的`params`
- 如果当前导航无法处理跳转操作，传递给父导航处理（如跨级跳转）
- 导航特定的方法如`openDrawer, closeDrawer, toggleDrawer`可以穿透
- 子导航默认不接收父导航事件，如果需要请使用`navigation.dangerouslyGetParent().addEventListener()`
- 默认情况下父导航的UI渲染在子导航的UI上（请考虑嵌套顺序，常用的有drawer包含stack，stack包含tab，tab包含stack）

有时嵌套的stack导航需要让子导航的头栏渲染而父导航的头栏不渲染，这时可以在父导航的`options`设置`headerShown:false`来改变UI渲染规则。有时父子的头栏都要渲染，你可以给子导航的`options`设置`headerShown:true`


跳转可以是`Screen`到`Navigator`（跨层）或`Screen`到`Screen`（同层）。以上面代码为例，如果你在Profile编程式导航到Home（跨层），则默认跳转到Feed（`Navigator`的第一个`Screen`）。如果需要在跨层时指定要跳转的`Screen`，`navigate()`方法接收第二个参数指定`Screen`，第二参数也可以同时指定跳转传参
```js
navigation.navigate('Home', { 
  screen: 'Messages',
  params: { user: 'jane' }
});
```
如果导航嵌套得很深，跳转时可以使用嵌套的`Screen`
```jsx
navigation.navigate('Root', {
  screen: 'Settings',
  params: {
    screen: 'Sound',
    params: {
      screen: 'Media',
    },
  },
});
//跳到Root->Settings->Sound->Media
```
### 生命周期
stack 导航的声明周期遵循堆栈的特性，进入A（A入栈），由A跳到B（B入栈），B返回A（B出栈，还剩A在栈内）。一个`Screen`如果在栈内，就会维持mounted状态，只有在出栈时才会unMount。

既然未出栈的`Screen`不会被销毁，那它就可以监听离开/返回该`Screen`的事件。这可以通过`useFocusEffect`这个Hook实现
```jsx
import { useFocusEffect } from '@react-navigation/native';

function Profile() {
  useFocusEffect(
    React.useCallback(() => {
      // Do something when the screen is focused

      return () => {
        // Do something when the screen is unfocused
        // Useful for cleanup functions
      };
    }, [])
  );

  return <ProfileContent />;
}
```
## StackNavigator
>yarn add @react-navigation/stack

除了`navigation`上的`navigate()`，StackNavigator也有自己的跳转方法
- `push(name,params)`，跳转到（入栈，即便已经在目标路由）
- `pop(n)`，返回n步（出栈n个）
- `popToTop()`，返回若干步到最初的路由（出栈到只剩1个）

### props
`props`作用于`Navigator`组件
- `mode`-跳转过渡方式，接收enum(card,modal)
- `headerMode`-导航头栏渲染方式，接收enum(float,screen,none)

### options
`options`作用于`Screen`组件
- `title`-兜底导航项标题
- `header`-自定义导航头栏，需要设置`headerMode="screen"`，接收`props=>component`（props为`{ scene, previous, navigation }`）
- `headerShown`-是否显示头栏，接收bool
- `headerTitle`-自定义头栏标题，接收string、`({allowFontScaling,style,children}) => component`，children包含标题string
- `headerTitleAlign`-头栏标题对齐，接收enum(center,left)
- `headerBackImage`-自定义头栏返回键，接收component
- `headerBackTitle`-头栏返回键标题，接收string，默认是上一个路由的标题
- `headerTruncatedBackTitle`-屏幕容不下`headerBackTitle`时启用
- `header(Left|Right)`-自定义头栏左右部分，接收component
- `headerBackground`-自定义头栏背景，接收component
- `header*Style`

- 可以自定义options（下同）
- [更多options](https://reactnavigation.org/docs/stack-navigator#options)

## DrawerNavigator
>yarn add @react-navigation/drawer

抽屉导航栏的开关切换有特别的方法，而且能穿透给子导航调用
- `navigation.openDrawer()`
- `navigation.closeDrawer()`
- `navigation.toggleDrawer()`

提供一个`useIsDrawerOpen`Hook来获取一个变量判断抽屉是否开启。
```jsx
import { useIsDrawerOpen } from '@react-navigation/drawer';
// ...
const isDrawerOpen = useIsDrawerOpen();
```
### props
`props`作用于`Navigator`组件
- `sceneContainerStyle`- 包裹`Screen`组件容器的样式
- `drawerStyle`-抽屉样式
- `drawerContent`-自定义抽屉内容，接收`props=>component`
- `drawerContentOptions`-接收object，配置默认的drawer
  - `activeTintColor`：选中时导航项标题、图标的颜色
  - `activeBackgroundColor`：选中时导航项背景色
  - `inactiveTintColor/inactiveBackgroundColor`：非选中时
  - `itemStyle`：导航项容器样式
  - `labelStyle`：导航项图标容器样式
  - `contentContainerStyle`：`DrawerContentScrollView`样式
  - `style`：`DrawerContentScrollView`容器样式

### options
`options`作用于`Screen`组件
- `title`-兜底导航项标题
- `drawerLabel`-导航项标题，接收接收string或`({focused, color}) => component`
- `drawerIcon`-导航项图标，接收 `({focused, color, size}) => component`
- [更多options](https://reactnavigation.org/docs/drawer-navigator)

### 自定义DrawerContent
觉得默认抽屉导航丑可以自定义抽屉内容，加些头像什么的。这要用到`drawerContent`options。完全重写默认抽屉要用到`DrawerContentScrollView`、`DrawerItem`组件。自定义的抽屉组件会收到一个`{state,navigation,descriptors,progress}`的props（下同），通过 `descriptors[state.routes[state.index].key].options`可以获取到当前选中项的options
```jsx
const DrawerContent = (props) => {
  const { state: { routes }, navigation } = props;
  return (
    <DrawerContentScrollView {...props}>
    {/*这里写一些额外的组件如头像什么的*/}
      {
        routes.map(route => {
          return (
            <DrawerItem key={route.key}
              label={({ focused, color }) => <Text>{route.name}</Text>}
              onPress={() => navigation.navigate(route.name)}
            />
          );
        })
      }
    </DrawerContentScrollView>
  );
};

const RootDrawer = () => {
  return (
    <NavigationContainer>
      <Drawer.Navigator
        initialRouteName="首页"
        drawerContent={DrawerContent}
      >
        <Drawer.Screen name="首页" component={Home} />
        <Drawer.Screen name="设置" component={Setting} />
      </Drawer.Navigator>
    </NavigationContainer>
  );
};
```
`DrawerContentScrollView`用于包裹导航项，`DrawerItem`组件用于重写导航项组件，`DrawerItem`接收以下props
- `label`-自定义导航项标题，接收string、`({ focused, color }) => component`
- `icon`-自定义导航项图标，接收`({ focused, color, size }) => component`
- `focused`-指示当前导航项是否选中
- `onPress`-点击回调，通常要跳转
- `activeTintColor/activeBackgroundColor`
- `inactiveTintColor/inactiveBackgroundColor`
- `labelStyle`
- `style`- 导航项样式

注：这些props和`Navigator`组件`drawerContentOptions`props中有重合部分，但是一旦你选择重写导航项，后者的配置是不生效的

其实如果还是不满意的话你甚至可以自己实现`DrawerItem`，彻底的重写

## TabNavigator
>yarn add @react-navigation/bottom-tabs

### props
`props`作用于`Navigator`组件
- `tabBar`-自定义tabBar，接收`props=>component`
- `tabBarOptions`-接收object，配置默认tabBar
  - `activeTintColor/activeBackgroundColor`
  - `inactiveTintColor/inactiveBackgroundColor`
  - `tabStyle`：导航项样式
  - `showLabel`：是否显示导航项标题，接收bool
  - `labelStyle`：导航项标题样式
  - `labelPosition`：导航项标题位置，接收enum(below-icon,beside-icon)
  - `style`：tabBar样式

### options
`options`作用于`Screen`组件
- `tabBarVisible`-是否显示tabBar，接收bool
- `title`-兜底导航项标题，没设置`headerTitle`或`\*Label`时启用
- `tabBarLabel`-导航项标题，接收string或`({focused, color}) => component`
- `tabBarIcon`-导航项图标，接收`({focused, color, size}) => component`
- `tabBarButton`-导航项按钮，它包含了标题和图标，需要实现onPress实现跳转，接收`props =>component`
- `tabBarBadge`-tab图标右上角气泡显示，接收num/string
- [更多options](https://reactnavigation.org/docs/bottom-tab-navigator/)

### 自定义TabBar
同样自定义TabBar会收到一个`{state,navigation,descriptors,progress}`的props，使用方法参考Drawer
```jsx
const tabBar = (props) => {
  const { state: { index: curIndex, routeNames, routes }, navigation, descriptors } = props;

  return (
    <View style={style.tabBar}>
      {
        routes.map(route => {
          const focused = routeNames[curIndex] === route.name;
          const { title, iconName } = descriptors[route.key].options;
          return (
            <TouchableOpacity key={route.key}
              activeOpacity={0.8}
              style={style.tabItem}
              onPress={() => navigation.jumpTo(route.name)}
            >
              <Icon name={iconName} color={focused ? '#5abebc' : '#666'} size={18} />
              <Text style={[style.tabItemTitle, { color: focused ? '#5abebc' : '#666' }]}>{title}</Text>
            </TouchableOpacity>
          );
        })
      }
    </View>
  );
};
```

## TopTabNavigator
>yarn add @react-navigation/material-top-tabs react-native-tab-view

### props
`props`作用于`Navigator`组件
- `tabBar`-自定义tabBar，接收`props=>component`
- `tabBarPosition`：tabBar位置，默认顶部，接收enum(top,bottom)
- `sceneContainerStyle`- 包裹`Screen`组件容器的样式
- `style`-tabBar样式
- `tabBarOptions`-接收object，配置默认tabBar
  - `activeTintColor/activeBackgroundColor`
  - `tabStyle`：导航项样式
  - `showIcon`：是否显示导航项图标，默认不显示，接收bool
  - `showLabel`：是否显示导航项标题，默认显示，接收bool
  - `pressColor/pressOpacity`：导航项按下时的颜色和透明度
  - `scrollEnabled`：tabBar是否可滚，接收bool
  - `indicatorStyle`：指示器（导航项下面的线条）样式
  - `labelStyle`：导航项标题样式，会覆盖`activeTintColor/activeBackgroundColor`
  - `iconStyle`：导航项图标样式
  - `style`：tabBar样式
  - `renderIndicator`：自定义指示器
- [更多props](https://reactnavigation.org/docs/material-top-tab-navigator#props)

### options
`options`作用于`Screen`组件
- `title`-兜底导航项标题，没设置`headerTitle`或`\*Label`时启用
- `tabBarLabel`-导航项标题，接收string或`({focused, color}) => component`
- `tabBarIcon`-导航项图标，接收`({focused, color, size}) => component`


## Hook
```js
import {
  useNavigation,
  useRoute,
  useNavigationState,
  useFocusEffect,
  useIsFocused
} from '@react-navigation/native';
//...
useFocusEffect(
  React.useCallback(() => {
    const unsubscribe = API.subscribe(userId, user => setUser(user));

    return () => unsubscribe();
  }, [userId])
);

const linkTo = useLinkTo();
linkTo('/settings/storage')

```
- `useNavigation`：获取navigation对象，仅限在Screen里面获取
- `useRoute`：获取当前路由，用来获取当前路由参数
- `useNavigationState`：获取导航的state/state中的某些数据，`const index = useNavigationState(state => state.index);`
- `useFocusEffect`：当Screen focus时执行副作用
- `useIsFocused`：获取当前Screen是否focused
- `useLinkTo`：获取一个使用path跳转的方法
