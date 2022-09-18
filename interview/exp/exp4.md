> 字节一面

#### CSS 1px 问题

> 物理像素：即物理屏幕的一个像素点，是设备能控制显示的最小物理单位
>
> 逻辑像素：程序能控制使用的虚拟像素，一个逻辑像素由一个或多个物理像素组成（受系统缩放和浏览器缩放的影响）
>
> CSS 像素：即CSS中的 1px，在没有缩放的情况下等价于逻辑像素

CSS 像素受 dpr 和 ppi 的影响，同一设备 dpr 的不同，不同设备的 ppi 不同，都会导致 1px 显示效果的不同

- dpr（设备像素比）：物理像素和逻辑像素的比值，可通过`window.devicePixelRatio`获取
- ppi（每英寸像素）：物理像素的密度，越大越清晰

若想要在移动端上 CSS 实现「物理像素的1px」，可选的方案有

- 根据 dpr 调整 viewport 中的 scale 并且禁用用户缩放（全局生效，确保 dpr 和 scale 积为 1）
- 根据 dpr 进行转换运算（如 dpr=2，在 css 中写 0.5px，或者使用 0.5 的 scale）
- 使用 1px 的图片模拟

#### zIndex 生效和失效

常见失效情况：

- 当前元素没有配合 position 使用
- 当前元素设置了 float
- 父元素的层叠优先级比当前元素的层叠优先级低
- 父元素设置了 `position: relative;`

【规则1】同一层叠上下文中，比较的是“内部元素层叠级别”，层叠级别大的元素显示在上，层叠级别小的元素显示在下；层叠级别相等的，在 DOM 结构上靠后的元素显示在上

【规则2】不同的层叠上下文中，比较的是“父级元素层叠级别”。以“父级层叠上下文”的层叠级别来决定显示的先后顺序，与自身的层叠级别无关

#### RN 跨端通信原理


RN 中有三个重要的组成模块：平台层（JAVA 或者 OC 环境），中间层（ C++ ）和JS 层

- 平台层负责原生组件的渲染和提供各式各样的原生能力，由平台原生语言实现
- 中间层负责解析 JS 代码，JS 和 Java/OC 代码互调，由 C++ 语言实现
- JS 层负责跨端页面具体的业务逻辑

> 无论是新架构还是老架构，JS 和 C++ 通信都是通过引擎注入的方式来实现的


RN 使用 JS 作为上层开发语言，就需要一个 JS 引擎来执行 JS 代码，在老架构中这个引擎是 JSCore。JSCore 是中间层的核心模块，负责 JS 代码的解析执行，他提供了几个 API 来与 JS 进行环境的共享和数据交互，是实现前终端通信的核心

```typescript
JSContextGetGlobalObject // 获取JavaScript运行环境的Global对象。
JSObjectSetProperty/JSObjectGetProperty // JavaScript对象的属性操作：set和get。
JSEvaluateScript // 在JavaScript环境中执行一段JS脚本。
JSObjectCallAsFunction // 在JavaScript环境中调用一个JavaScript函数
```

通信的双方是 Native 模块和 JS 模块，Native 模块由平台代码实现运行在平台层，JS 模块由 JS 代码实现，相互为对方提供模块功能，并借助中间层完成双方的通信。JS 和 平台环境会分别维护一个对方模块的映射表（moduleId 和 methodId），用于模块功能调用

##### C++ 和 JS 通信（Bridge）

**【JS 调用 C++】**：C++ 侧通过 JSC API 将 C++ 侧的变量/函数/类经过类型转换后注入到 JS Global 对象中，形成一份映射表，供 JS 侧调用 

C++ 通过`JSObjectSetProperty`在 Global 对象中注入了 Native API 供 JS 调用

```typescript
nativeFlushQueueImmediate // 立即清空 JS 任务队列
nativeCallSyncHook // 同步调用 Native 方法
nativeRequire  // 加载 Native 模块
```

**【C++ 调用 JS】**：JS 事先在 Global 对象中注入好相关变量/函数，C++ 侧通过 JSC API 获取 JS Global 对象中的全局变量和全局函数，进行相应的调用并接受返回值

JS 会在 Global 对象中注入一些 Native 模块需要的 API，供 C++ 调用

```typescript
callFunctionReturnFlushedQueue // 让 C++ 调用 JS 模块
invokeCallbackAndReturnFlushedQueue // 让 C++ 调用 JS 回调
flushedQueue // 清空 JS 任务队列
callFunctionReturnResultAndFlushedQueue // 让 C++ 调用 JS 模块并返回结果
```

JS 调用 Native 模块是异步调用，会把参数包装成一个任务放在任务队列`MessageQueue`中，等待 Native 调用（比如事件触发），待任务满足执行条件时 Native 模块再借助中间层通过`flushedQueue`清空并执行任务队列

【PS】以上基于任务回调的通信，只能实现单次的调用。若需要实现事件监听，则需要使用发布订阅模块来代替任务队列

##### C++ 和 JS 通信（JSI）

RN 的新架构使用 JSI 代替 Bridge，它是对 JS引擎 与 Native (C++) 之间相互调用的封装，相当于一层通用的映射框架，将 JS 接口和引擎解耦，允许使用其他 JS 引擎

JSI 弃用了异步序列化的 Bridge，而是采用 HostObject 接口作为双边通信的协议（按规定实现install、get方法），实现了双边同步通信下的高效信息传输。

##### 平台 和 C++ 通信

以 JAVA 为例，Java 跟 C++ 的互相调用通过 JNI 实现，C++ 层会暴露出来一些 API 来给 Java 层调用，来让 Java 能通过 C++ 中间层跟 JS 层进行通信

```typescript
initializeBridge // 初始化：C++ 从 Java 拿到 Native 模块，作为参数传给 JS 生成 NativeModules
jniLoadScriptFromFile // 加载 JS 文件
jniCallJSFunction // 调用 JS 模块
jniCallJSCallback// 调用 JS 回调
setGlobalVariable // 编辑 global 变量
getJavaScriptContext // 获取 JS 运行环境
```

#### RN 和 Flutter 的对比和选型

RN 的开发语言和风格更贴近前端，上手门槛对前端研发人员来说更低；而 Flutter 需要额外学习 Dart 语言和 Flutter 框架

RN 属于中转型框架，借助中间层调用终端的能力，依赖原生平台实现渲染；Flutter 在跨平台和兼容方向上做得更加彻底，借助 skia 引擎完全接管渲染，可实现更大范围的跨平台和更好的一致性，理论上性能也更好

RN 在发展时间和社区上相较于 Flutter 占优，有更多的经验累积和第三方库

```typescript
interface Person {
  age: number;
  name: string;
  isMale: boolean;
}

type FilterString<T> = {
   [K in keyof T as T[K] extends string ? K : never]: T[K];
}
```

