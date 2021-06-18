> `环境配置略

## 基本模块

### http

http模块用于搭建http服务，通过监听端口来处理请求并返回。每接收一个请求，触发`request`事件并执行回调函数处理请求，该回调函数将被提供两个对象作为参数：req请求对象（含请求的详细信息）、res响应对象（用于构造返回数据、设置响应头等）。

**注意，回调在接收所有headers后执行，对于post请求，请求体还未接收，监听req的`data`事件来设置请求体接收的回调**

```js
const http = require('http')
const url = require('url')

http.createServer((req, res)=>{
  req.on('data', body=>console.log('请求已接收'))
  req.on('end',()=>console.log('请求结束'))
  res.writeHead(200, {'Content-Type': 'text/plain'})
  res.end('hello world')
}).listen(8080)
```

req对象（[`http.IncomingMessage`](http://nodejs.cn/api/http.html#http_class_http_incomingmessage)）相关属性方法

- `status(Code|Message)` 请求状态
- `headers`, `rawHeaders` 请求头
- `method` 请求方法
- `httpVersion` HTTP版本
- `url` 请求url

res对象（[`http.ServerResponse`](http://nodejs.cn/api/http.html#http_class_http_serverresponse)）相关属性方法

- `getHeaderNames()` 获取响应头名称列表
- `getHeaders()` 获取已设置的响应头的副本
- `setHeader('headername', value)` 设置响应头字段
- `getHeader('headername')` 获取响应头字段
- `removeHeader('headername')` 移除响应头字段
- `hasHeader('headername')` 判断是否存在指定响应头字段
- `headersSent` 响应头是否已被发送给客户端
- `writeHead(statusCode[,headers])`发送请求头，其中的headers会与之前已设置的headers合并
- `write(chunk[,encoding][,callback)`发送请求体，默认utf8编码
- `end(chunk[,encoding][,callback)`发送最后请求体并结束请求，默认utf8编码

### url

url模块用于url的解析。目前官方已经不推荐使用`url.parse()`，而是推荐以类的方式使用

```js
const url = new URL('https://oshinonya.com/article');
// const url = new URL('/article','https://oshinonya.com');
```
通过 url 实例属性来获取各种解析结果，这些实例属性包括`hash`、`host`、`hostname`、`href`、`origin`、`pathname`、`port`、`protocal`、`search`

通过 `url.searchParmas` 这个属性来获取查询参数对象（下简写为`sp`），`sp`有提供以下方法来操作查询参数
- `sp.get(key)`
- `sp.delete(key)`
- `sp.set(key,value)`
- `sp.append(key,value)`
- `sp.toString()`

### events

events模块提供了一个`EventEmitter`类用于处理事件，在该机制下，一个事件名对应一种事件，每个事件维护一个监听队列，并在触发时按序触发队列里的回调函数。

```js
const EventEmitter = require('events')
const emitter = new EventEmitter()
```

`EventEmitter`实例有以下方法
- `addListener/on(name,listener)`添加一个回调函数到某事件的队列尾，多次调用多次添加。返回实例本身因此可以链式调用（下同某些方法同）
- `emit(name,[,...args])`触发某事件，按序调用事件队列中的回调函数并传递参数
- `eventNames()`返回现有事件名的数组
- `listenerCount(name)`返回某事件注册的回调函数数量
- `listeners(name)`返回某事件的回调函数队列
- `removeListener/off(name,listener)`从某事件的队列中移除回调函数，每次移除只移除一个回调。
- `once(name,listener)`添加一个只触发一次的回调函数到某事件的队列尾
- `prependListener(name,listener)`添加一个回调函数到某事件的队列头
- `prependOnceListener(name,listener)`添加一个只触发一次的回调函数到某事件的队列头
- `removeAllListener([name])`清空某事件/所有事件的队列

### fs

fs模块用于访问文件系统并与文件系统进行交互，fs所有方法默认是异步的，但可以通过调用`fs.*Sync()`来转为同步
- `fs.access(path[,mode],err=>{})`测试用户对文件/目录的权限，mode为常量`fs.constants.(F|R|W)_OK`对应是否存在、可读、可写
- `fs.mkdir(path[,options],err=>{})`创建目录，options默认为`{recursive:false,mode:0o777}`
- `fs.writeFile(file,data[,options],err=>{})`创建文件（存在则覆盖）并写入数据，data可以是string或buffer，options默认为`{encoding:'utf8',mode:0o666,flag:'w'}`
- `fs.appendFile(file,data[,options],err=>{})`追加数据到文件（不能存在则创建），参数参考`writeFile()`
- `fs.readFile(path[,options],(err,data)=>{})`读取文件，options默认为`{encoding:null,flag:'r'}`，回调中data为string或buffer
- `fs.readdir(path[,options],(err,files)={})` 读取目录，options默认为`{encoding:'utf8',withFileTypes:false}`，回调中files为文件名数组
- `fs.rename(oPath,nPath,err=>{})`文件移动/重命名
- `fs.rmdir(path[,options],err=>{})`删除目录，options默认为`{recursive:false}`
- `fs.unlink(path,err=>{})`删除文件
- `fs.stat(path,(err,stats)=>{})`查询文件信息，`stats`对象提供了关于文件的信息（文件类型、创建事件等）

fs模块中的流式文件读取/写入
- `fs.createReadStream(path[,options])`创建一个读流rs，options接收`{flags,encoding,fd,mode,autoClose,emitClose,start,end,highWaterMark,fs}`。监听流上的`data`事件来获取数据，监听流上的`end`事件读取完成后回调
- `fs.createWriteStream(path[,options])`创建一个写流ws，options接收`{flags,encoding,fd,mode,autoClose,emitClose,start,fs}`。监听流上的`finish`事件写入完成后回调
- `rs.pipe(ws)`管道传输复制文件

### buffer

buffer用于处理二进制数据

使用`Buffer.from(array)`、`Buffer.alloc(size)`、`Buffer.allocUnsafe(size)`创建一个buffer，单位为字节

Buffer（字节数组）可以像数组一样通过下标被访问和修改，每个元素是一个0-255的整数，通过`buffer.length`获取buffer大小，也可以迭代

`buffer.write(string[,offset[,length]][,encoding])`将字符串写入buffer

`srcBuffer.copy(destBuffer[,start,end])`将源buffer拷贝到目标buffer

`buffer.slice(start,end)`用于buffer的切片（返回视图不是副本）

`buffer.indexOf(value[,byteOffset][,encoding])`value可以是string/buffer/uint8array/int，查找目标出现的位置

`buffer.includes(value[,byteOffset][,encoding])`value同上，判断是否含有目标

### path

path模块用于文件系统路径解析
- `path.basename(path[,ext])`返回路径的最后一部分，可选择滤掉扩展名
- `path.dirname(path)`返回路径目录部分
- `path.extname(path)`返回路径扩展名
- `path.isAbsolute(path)`判断是否绝对路径
- `path.join(...)`拼接路径的多个部分并返回
- `path.normalize(path)`去除`..`、`.`计算路径
- `path.parse(path)`解析路径返回一个`{root,dir,base,ext,name}`对象
- `path.relative(from,to)`基于当前路径返回 `from` 到 `to` 的相对路径
- `path.resolve(...)`基于当前路径和传入片段计算返回绝对路径
