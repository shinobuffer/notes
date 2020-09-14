环境配置略

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

- url-请求地址

res对象（[`http.ServerResponse`](http://nodejs.cn/api/http.html#http_class_http_serverresponse)）相关属性方法

- 

### url

url模块

```js

```

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

### path

path模块用于文件系统路径解析

