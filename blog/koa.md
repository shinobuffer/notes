> koa，使用更新的语法，编写更优雅的代码

创建一个简单服务，下面的所有操作都将围绕app这个变量完成

```typescript
import koa from 'koa';
const app = koa();
// ...
app.listen(3000);
```

koa中的中间件使用和express类似同样使用`app.use(middleware)`

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

- `request.header`请求node headers
- `request.headers`解析后的请求头对象
- `request.method`请求方法
- `request.length`请求Content-Length
- `request.url/origin/href/path`请求url/origin/完整url/路径
- `request.querystring/query`rawQuery/objQuery
- `request.host/protocol`请求host/协议
- `request.type`根据Content-Type返回mine-type
- `request.fresh/stale`缓存新鲜/过期
- `request.secure`是否https协议
- `request.ip`请求远程地址
- `request.socket`请求套接字
- `request.is(types)`判断请求Content-type是否符合types（支持mime或extname）
- `request.get(field)`获取请求头某个字段

大部分属性和方法在`ctx`上都有对应的别名，如「ctx.query==ctx.request.query」，请多多善用

### Koa Response

`ctx.response`是对原生响应对象封装，重写并扩展了许多有用的属性和方法

- `response.headers`响应请求头对象
- `response.socket`响应套接字
- `response.status/message=`响应状态码/状态消息
- `response.length=`响应Content-Length
- `response.body=`响应体（Content-Type会根据响应体自适应）
- `response.type=`响应Content-Type（可设置为mime或extname）
- `response.lastModified=`以Date的形式读写Last-Modified
- `response.etag=`设置ETag
- `response.headerSent`检查响应头是否已发送
- `response.is(types)`判断响应Content-type是否符合types（支持mime或extname）
- `response.get/has/remove(field)`获取/是否含有/移除请求头某字段
- `response.set(field,value)`设置请求头字段
- `response.set(obj)`设置请求头字段
- `response.redirect(url,[alt])`302重定向（可修改ctx.status来改为301）
- `response.attachment([filename],[options])`设置Content-Disposition

大部分属性和方法在`ctx`上都有对应的别名，如「ctx.body==ctx.request.body」，请多多善用

## 路由

```typescript
import Router from 'koa-router';

```



## 常用三方中间件

`koa-bodyparser`可以新增`ctx.request.body`，将rawBody转换为objBody
