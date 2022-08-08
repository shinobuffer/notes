##### for in 和 for of的区别

`for in` 可以遍历到对象上的所有属性名和方法名，**包括其原型链上定义的**属性名和方法名

`Object.keys()`可以得到对象上的属性名和方法名集合，**不包括原型链上的**

`for of`只能用于实现了`Iterator`的对象，可以遍历对象上的属性值，**不包括原型链上的**

##### useEffect 和 userLayoutEffect 的区别

`useEffect`和`useLayoutEffect`的执行时机不一样

- `useEffect`前者被异步调度，当页面渲染完成后再去执行，不会阻塞页面渲染。
- `useLayoutEffect`在 commit-layout 阶段新的DOM准备完成，但还未渲染到屏幕之前，同步执行。

##### React 和 Vue 的区别

- Vue 默认使用模板语法：模板、样式、逻辑；React 默认使用 JSX，更贴近原生 JS，对 TS 友好
- Vue 一个组件就是一个文件，而 React 中的组件实质上是一个类/函数，可以以变量的形式存在于同一文件内
- Vue 是推模型，数据变动时界面会自动更新；React 是拉模型，数据变动时需要手动触发更新（函数式编程`view = f(state)`）

##### 浏览器的线程和进程

浏览器是进程的

- Browser 进程（唯一）
- GPU 进程（唯一）
- 插件进程（一个插件对应一个）
- Renderer 进程（一个 tab 对应一个）

Renderer 进程是多线程的：

- GUI 线程，负责页面的解析、布局和绘制
- JS 引擎线程，负责解析执行 JS 代码
- 事件触发线程，负责将满足触发条件的任务（Promise等）加入到任务队列中等待处理
- 定时器线程，单独负责计时，把到期的定时任务加入到任务队列中等待处理
- 网络请求线程，一个请求对应一个线程，请求状态变更后将回调任务加入到任务队列中等待处理

##### 错误捕获和上报

错误类型：JS 执行错误（同步/异步）、资源加载错误（图片/script）、语法错误、普通异步错误（setTimeout）

错误捕获

- `try-catch`
- `window.onerror`
- `window.addEventListener('error')`
- `window.addEventListener('unhandledrejection')`捕获未 catch 的异步错误

![img](pre-exp.assets/v2-9e5ae1919c008d706f50bd5307aa065c_1440w.jpg)

数据上报

- HTTP 请求——受同源限制，会被页面 unload 取消
- Image.src——可跨域，但受限于 get 方法数据量有上限
- `navigator.sendBeacon()`——可跨域，甚至不受 unload 影响