> koa，使用更新的语法，编写更优雅的代码

调用`koa()`创建一个简单服务，下面的所有操作都将围绕app这个变量完成

```typescript
import koa from 'koa';
const app = koa();
// ...
app.listen(3000);
```

koa调用`app.use(middleware)`来使用中间件，中间件是一个`async (ctx, next)=>{}`的异步函数。**koa的应用级中间件总是优先于路由中间件，与定义顺序无关**

中间件的执行顺序遵循“洋葱模型”：每个中间件以`await next()`为分界线分割为“上部”和“下部”，当一个中间件执行完“上部”并执行到`next`时就会暂停当前中间件的执行，进入下一个中间件的“上部”执行；当所有“上部”都完成执行，程序开始倒序执行中间件的“下部”

![clipboard.png](./koa.assets/bV6D5Z.png)



## 上下文

koa的`context`对node中的request和response对象进行了封装，并在上面提供了许多常用属性和方法。`app.context`是`app.ctx`的原型，可以通过在`context`上添加属性或方法来扩展`ctx`，下面介绍koa的`ctx`

| 属性/方法                                        | 说明                         |
| ------------------------------------------------ | :--------------------------- |
| ctx.req                                          | node的Request对象            |
| ctx.res                                          | node的Response对象           |
| ctx.request                                      | koa的Request对象             |
| ctx.response                                     | koa的Response对象            |
| ctx.app                                          | 应用程序实例引用             |
| ctx.cookies.get(name, [options])                 | 获取cookie                   |
| ctx.cookies.set(name, value, [options])          | 设置cookie                   |
| ctx.throw([status], [msg], [properties])         | 抛出异常statusCode           |
| ctx.assert(value, [status], [msg], [properties]) | 表达式非真抛出异常statusCode |

### Koa Request

`ctx.request`是对原生请求对象封装，重写并扩展了许多有用的属性和方法

- `request.header`-请求node headers
- `request.headers`-解析后的请求头对象
- `request.method`-请求方法
- `request.length`-请求Content-Length
- `request.url/origin/href/path`-请求url/origin/完整url/路径
- `request.querystring/query`-rawQuery/objQuery
- `request.host/protocol`-请求host/协议
- `request.type`-根据Content-Type返回mine-type
- `request.fresh/stale`-缓存新鲜/过期
- `request.secure`-是否https协议
- `request.ip`-请求远程地址
- `request.socket`-请求套接字
- `request.is(types)`-判断请求Content-type是否符合types（支持mime或extname）
- `request.get(field)`-获取请求头某个字段

大部分属性和方法在`ctx`上都有对应的别名，如「ctx.query==ctx.request.query」，请多多善用

### Koa Response

`ctx.response`是对原生响应对象封装，重写并扩展了许多有用的属性和方法

- `response.headers`-响应请求头对象
- `response.socket`-响应套接字
- `response.status/message=`-响应状态码/状态消息
- `response.length=`-响应Content-Length
- `response.body=`-响应体（Content-Type会根据响应体自适应）
- `response.type=`-响应Content-Type（可设置为mime或extname）
- `response.lastModified=`-以Date的形式读写Last-Modified
- `response.etag=`-设置ETag
- `response.headerSent`-响应头是否已发送
- `response.is(types)`-判断响应Content-type是否符合types（支持mime或extname）
- `response.get/has/remove(field)`-获取/是否含有/移除请求头某字段
- `response.set(field,value)`-设置请求头字段
- `response.set(obj)`-设置请求头字段
- `response.redirect(url,[alt])`-302重定向（可修改ctx.status来改为301）
- `response.attachment([filename],[options])`-设置Content-Disposition

大部分属性和方法在`ctx`上都有对应的别名，如「ctx.body==ctx.request.body」，请多多善用



## 路由

在koa中多了一个“路由器Router”的概念

- 路由器支持使用中间件
- 路由器上可挂载路由（支持命名路由）
- 调用`router.routes()`获取合并好的中间件
- 路由器可以嵌套（`routerA.use(routerB.routes())`）
- `router.redirect(source, destination, [code])`路由重定向

```typescript
import Router from 'koa-router';
const router = new Router({
  prefix: '/api' // 路由前缀作为根url
});

router.use(middleware); // 对所有请求使用中间件
router.use('/login', middleware);  // 针对指定url使用中间件
router.use(['/login', '/register'], middleware); // 针对一组url使用中间件

router.get('/:uid', async (ctx, next)=>{
  const params = ctx.params; // 命名路由参数对象
});
// 其他路由

router.redirect('/stat', 'statistics'); // 将/stat重定向至/statistics

app.use(router.routes());
```



## 常用三方中间件

`koa-bodyparser`可以新增`ctx.request.body`，将rawBody转换为objBody

```typescript
import bodyParser from 'koa-bodyparser'
app.use(bodyParser());
```

`koa-static`提供静态服务，`app.use(koaStatic(targetDir))`开启「/=>targetDir」的静态托管服务

