### BOM

> BOM（浏览器对象模型）是浏览器暴露给开发者的对象，开发者通过该对象与浏览器/系统进行交互，获取相关信息

BOM 的顶级对象是`window`，也是全局对象。window 下还有几个重要对象

- 【location】提供 URL 的解析，页面的跳转/刷新
- 【navigator】提供浏览器/系统相关属性
- 【screen】提供物理显示器相关信息
- 【history】提供历史记录操作

### 本地存储方案

【Cookie】服务端寄存在客户端的 KV 字符串，会在每次请求中被携带传递给服务端，cookie 的一些控制字段如下

- `Expire/Max-Age`过期时间（后者优先级更高）
- `Domain`生效域名
- `Path`生效路径
- `Secure`是否只在 HTTPS 协议携带 cookie
- `HttpOnly`是否允许前端访问修改 cookie
- `SameSite`跨站点请求是否携带 cookie

【LocalStorage】同步读写的持久化本地存储，只能读写字符串

【SessionStorage】相当于非持久的 LocalStorage，在页面关闭时会被清理

【IndexDB】可以存储结构化数据的数据库，异步读写

### 跨域方案

> 跨域是浏览器基于**同源策略**的一种安全手段

**同源指的是页面地址和请求地址的【协议】、【主机名】、【端口】相同**，三者中任意一个不相同将导致非同源跨域，浏览器会出于安全考虑限制跨域 xhr 请求

比较安全常用的跨域方案有CORS、postMessage、websocket

- CORS，服务端只需在响应头设置`Access-Control-Allow-*`相关字段即可，浏览器在预检请求通过后会允许跨域资源的访问
  - `Access-Control-Allow-Origin` 表示允许的来源
  - `Access-Control-Allow-Methods` 表示允许的请求方法
  - `Access-Control-Allow-Headers` 表示允许的请求头
  - `Access-Control-Allow-Credentials` 表示允许携带认证信息（如cookie）
- postMessage API 不受同源策略限制，可实现父子 window 之间的通信，自然可以实现两个域下的通信
- websocket 也不受同源策略限制，但是部署起来会有一定的成本

没那么常用的跨域方案有 JSONP、反向代理、iframe 嵌套

- JSONP，利用 src 不受同源策略的特性，使用 script 标签跟非同源服务端交互，由服务端返回包含数据的 JS 代码
- 反向代理，客户端请求同源服务端，服务端作为代理将请求转发给实际处理的服务端，并将结果回传客户端
- A-B-A iframe 嵌套，iframe 之间通过 url 传递信息，同域的两个窗口再相互通信 


LocalStorage 和 SessionStorage 同样收到同源策略的限制，如果需要跨域读写需要使用`postMessage`

### 前端路由种类

> 前端路由可实现**无刷新**的页面切换，是 SPA 单页应用实现的关键
>
> 前端路由依赖前端的代码才能运行，因此页面首次加载耗时会增加，也不利于搜索引擎收录页面

根据具体实现分为 Hash 路由和 History 路由

- 【Hash 路由】使用 URL 的 hash 部分来承载 path 和 query，并通过`hashchange`事件监听 hash 的变化
  - 锚点功能冲突；seo 不友好（因为搜索引擎是忽略hash部分）
  - 服务端无法获取 hash 中的 path 和 query
- 【History 路由】利用浏览器的 History API（`pushState/replaceState/go()`）操作历史记录栈，并通过`popstate`事件监听历史记录栈的变化
  - 需要服务端配置，将所有路径指向根文件

### 地址栏输入 URL 后发生的事

1. URL 解析
2. 根据域名进行 DNS 查询，获取目标服务器 IP 地址
3. 建立TCP连接
4. 发送HTTP/HTTPS请求（TLS连接）
5. 服务器响应请求返回页面
6. 相关资源下载解析，渲染页面（详见「[浏览器渲染流程](#浏览器渲染流程)」）

### 浏览器渲染流程

浏览器解析渲染流程

- 【解析/Parse】解析 HTML 生成 DOM Tree（期间可能会进行资源拉去/JS代码执行），解析 CSS 生成 CSSOM Tree，结合两者为可见节点生成 Render Tree
  - DOM 和 CSSOM 是并行构建的，但在构造 Render Tree 阶段前这两者都必须准备好
  - CSSOM 构建会阻塞 JS 执行（因为 JS执行可能会操作样式）
- 【布局/Layout】根据渲染树计算每个节点的布局信息（位置和大小）
- 【绘制/Painting】根据渲染树和 Layout 阶段收集的信息，分层绘制（渲染层）
- 【合成/Composite】将不同层按重叠顺序合成为位图，交给 GPU 渲染上屏

![image](browser.assets/126033732-d5002255-1c88-4dee-9371-da166aacdca9.png)

#### 渲染层和合成层

**渲染层的形成条件同层叠上下文的形成条件**，如果一个元素创建了层叠上下文，他将会创建一个渲染层，条件如下

- document
- 携带 z-index 的定位 position: relative|fixed|sticky|absolute
- opacity 非 1
- transform 非 none
- filter 非 none
- overflow 非 visible
- will-change = opacity | transform | filter

【规则1】同一层叠上下文中，比较的是“内部元素层叠级别”，层叠级别大的元素显示在上，层叠级别小的元素显示在下；层叠级别相等的，在 DOM 结构上靠后的元素显示在上

【规则2】不同的层叠上下文中，比较的是“父级元素层叠级别”。以“父级层叠上下文”的层叠级别来决定显示的先后顺序，与自身的层叠级别无关

基于层叠上下文的规则，一个渲染层可以根据【规则1】来决定渲染元素的绘制顺序，完成一层的绘制；不同渲染层再根据【规则2】来进行合成

___

**一定条件下渲染层会提升为合成层**，比如 3d-translate、fixed、will-change、video 等，合成层拥有单独的图层，能利用 GPU 加速位图的合成；合成层上的 transform 和 opacity 不会导致重排重绘；合成层上的重绘也不会影响外部，能减小重绘的区域。但合成层也是相当耗内存的（与内部元素尺寸正相关），在使用时需要考虑优化和取舍

#### 重排和重绘

> 重排和重绘：重新布局和重新绘制的过程（CPU主线程上）

任何**改变节点布局信息**（位置、大小）和**访问节点布局信息**的行为都会导致重排

**触发重排肯定会触发重绘**，此外颜色的修改等行为也会触发重绘

【浏览器优化机制】：因为重排的消耗比较大，浏览器会用队列缓存重排操作，批量执行。当然要访问布局信息时，浏览器会立马上清空队列，触发重排来获取正确的值

针对重排和重绘的机制，优化的方向主要是避免频繁的重排

- 避免频繁异步操作 DOM 和异步修改内联样式
- 避免频繁交替修改布局样式和访问布局信息
- 合理使用 will-change 提示浏览器可能发生的变化，将渲染层提升为合成层利用 GPU 加速
- 优先使用 CSS transform/animation 实现动画效果，同样是尽可能提升为合成层

