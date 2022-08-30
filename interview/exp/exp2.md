> 字节一面
>
> 常规题目：TCP、UDP、浏览器缓存、HTTPS

#### 例题

场景题目

```typescript
{
  var a = 1;
  let b = 2;
}
console.log(a) // 1
console.log(b) // Reference Error
```

```typescript
const a = 1;
let b = 2;
var c = 4;

// 块级作用域不会绑定在 window 上，window.a 为 undefined，不会走这里的逻辑
if(window.a) {
	b = 3;
}

console.log(a) // 1
console.log(b) // 2
console.log(window.c) //4
```

正则题目

```typescript
// <h1 id="XSS">XSS</h1><script>alert(1);</script>
// <h1>XSS</h1>&lt;script&gt;alert(1);&lt;/script&gt
const xss = (str: string): string => {
  // 1.将<script></script>中的尖括号进行替换
  return str.replace(/<(\/?script)>/g, '&lt;$1&gt;')
  // 2.对标签中的属性进行模除
    .replace(/(?<=<.*)( \S+="\S*")(?=.*>)/g, '');
};
```

千位数划分题目

```typescript
/** 千分位分割（支持带小数点） */
export const splitThousand = (numStr: string): string => {
  const [intArea, floatArea = ''] = numStr.split('.');
  let count = 0;
  let newIntArea = '';

  // 逆序扫描整数部分
  for (let i = intArea.length - 1; i >= 0; i--) {
    newIntArea = `${intArea[i]}${newIntArea}`;
    count++;
    // 每添加三个数字，就补充一个逗号
    if (count % 3 === 0 && i > 0) {
      newIntArea = `,${newIntArea}`;
    }
  }

  if (floatArea) return `${newIntArea}.${floatArea}`;
  return newIntArea;
};

export const splitThousandWithRegExp = (numStr: string): string => {
  const [intArea, floatArea = ''] = numStr.split('.');

  const newIntArea = intArea.replace(/(\d)(?=(\d{3})+$)/g, '$1,');

  if (floatArea) return `${newIntArea}.${floatArea}`;
  return newIntArea;
};
```

#### CSS 单位

- rem 相对于根字体大小
- em，相对于自身的字体大小，如果自身没有设置字体大小，则相对于继承字体的大小
- vh/vw，相对于视口大小，视口被平分为 100 份，1vh/1vw 就是其中的一份大小
- 百分比
  - top/left、right/bottom 相对于最近非 static 定位的父元素的高/宽
  - padding/margin 永远相对于父元素宽
  - border-radius 相对于自身宽度

#### CDN

> 内容分发网络，根据用户位置分配最近的资源，加速资源的访问

应用了 CDN 后，客户端访问资源时，本地 DNS 进行查询的时候会拿到一个指向 CDN 均衡负载系统的 CNAME，CDN 负载均衡系统综合考虑客户端地理位置和 CDN 节点的负载情况等信息返回最佳的 CDN 节点的 IP 地址

客户端拿到 IP 地址后向目标 CDN 节点请求资源，此时 CDN 节点作为一个缓存代理，如果目标资源在缓存中直接返回给客户端，否则回源拉取目标资源并缓存下来，供下次命中使用 

#### 响应式布局

> 根据屏幕尺寸动态调整样式布局

```html
<meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no”>
```

- 媒体查询
- 百分比/vh/vw
- rem 配合动态设置根字体大小

```typescript
//动态为根元素设置字体大小
function init () {
    // 获取屏幕宽度
    var width = document.documentElement.clientWidth
    // 设置根元素字体大小。此时为宽的10等分
    document.documentElement.style.fontSize = width / 10 + 'px'
}

//首次加载应用，设置一次
init()
// 监听手机旋转的事件的时机，重新设置
window.addEventListener('orientationchange', init)
// 监听手机窗口变化，重新设置
window.addEventListener('resize', init)
```

> 字节二面

#### 跨端开发的理解

对研发效率的追求，催生出了跨平台技术的出现。相比于传统的原生开发，跨平台技术能实现一套代码多端运行、动态更新，可以以更高的效率和更低的成本开发应用，在大多数场景下是一个更优的选择

跨平台技术可大致分为三类：

- web 技术，使用 webView 来
- 原生渲染，比如 RN，该方案通过执行JS代码来调用原生控件进行渲染，由C++中间层承担双方沟通的桥梁
- 自渲染，比如 Flutter，该方案选择直接自绘控件，摆脱平台带来的差异性

RN 渲染原理

- JS侧通过 React 维护 Fiber Tree，并将更新转换为一连串指令通过 bridge 发送给 Native 侧
- Native 侧根据 JS 侧的指令维护 Shadow Tree，计算节点布局信息，并生成相应的 UI 操作指令
- 终端根据 UI 操作指令执行真正的 UI 渲染逻辑

#### xhr 封装 get/post

XMLHttpRequest 对象属性

- `readyState`代理状态
  - UNSENT-0-未`open()`
  - OPENED-1-已`open()`
  - HEADERS_RECEIVED-2-已`send()`且接收到响应头
  - LOADING-3-下载中`responseText`已有部分数据
  - DONE-4-下载完成
- `status`HTTP状态码
- `responseType`响应数据类型，可以是text、json、blob、arraybuffer
- `response`响应内容，和`responseType`对应
- `responseText`文本形式返回响应内容，要求`responseType`为 text
- `timeout`超时时间
- `withCredentials`是否携带 cookie

XMLHttpRequest 对象事件

- `onreadystatechange`代理状态变更事件
- `onload`请求完成事件
- `onerror/onabort/ontimeout`请求错误/终止/超时事件

XMLHttpRequest 对象方法

- `open(method, url, isAsync)`初始化一个请求（默认异步）
- `setRequestHeader(header, value)`设置请求头
- `send(body)`发送请求，`body`可以是Blob、FormData、URLSerachParams
- `abort()`终止请求

___

封装步骤，可使用 Promise，具体实现见 [xhr-get-post.ts](https://github.com/Bersder/leetcode/blob/master/src/implementation/example/xhr-get-post.ts)

1. 创建 xhr 对象
2. 调用`open(method, url)`初始化请求
3. 设置响应类型`responseType`、调用`setRequestHeader()`设置请求头
4. 设置超时时间`timeout`和`Credentials`
5. 绑定事件回调`onreadystatechange`接收响应（`readyState`等于4并且`status`正常）
6. 调用`send(body)`发送请求
