创建一个简单服务，下面的所有操作都将围绕app这个变量完成

```typescript
import express from 'express';
const app = express();
// ...
app.listen(3000);
```

## 中间件

服务器对每个请求的处理可分解为多次操作，中间件就是这些操作的封装，**中间件的匹配及执行顺序同其定义顺序**。

通过`app.use(middleware)`来使用一个中间件，中间件是一个接受三个参数的函数

1. `req`请求对象
2. `res`响应对象
3. `next`通过调用该方法来继续向下匹配

```typescript
app.use((res, req, next)=>{
  // ...
	next();
})
```

express提供了实现静态服务的中间件，通常会作为第一个中间件使用
- `app.use(express.static(targetDir))`开启「/=>targetDir」的静态托管服务
- `app.use(path, express.static(targetDir))`开启「path=>targetDir」的静态托管服务

而在最后的通常会定义一个中间件处理非法请求

```typescript
app.use((res, req)=>{
  res.status(404).send(); // 返回404等操作
})
```

## 路由

路由是一种特殊的中间件，通过`app.get(path, (req,res,next)=>{}`注册一个路由（get可以替换唯post/put等方法），支持动态路由（类似于vue-router）。**路由匹配的顺序同定义的顺序，默认情况下一旦命中就不会继续向下匹配**，但可以通过调用next方法来强制继续向下匹配

```typescript
app.get('/:id', (req, res, next)=>{
  const query = req.query; // query对象
  const params = req.params; // 动态路由参数对象
  res.send('hello');
  // next(); // 调用next方法来继续向下匹配
})
```

express支持将多个提供同类服务的路由封装为一个中间件供app.use使用

```typescript
// user.js
const router = express.Router();
router.get('/login', (req,res)=>{});
router.get('/register', (req,res)=>{});
export default router;
// app.js
import userRouter from './router/user';
app.use('/user', userRouter);
```

## 常用三方中间件

`body-parser`可以新增`req.body`，将rawBody转换为objBody

```typescript
import bodyParser = 'body-parser';
// 解析 application/x-www-form-urlencoded
app.use(bodyParser.urlencoded({ extended: false }));
// 解析 application/json
app.use(bodyParser.json());
```

`cookie-parser`可以通过解析`req.headers`来获取objCookies并绑定在`req.cookies`上，并提供`res.cookie()`方法来在服务端设置cookie

```typescript
import cookieParser = 'cookie-parser';
app.use(cookieParser('secretKey'));
```

`express-winston, winston`

`multer`用于上传文件的接受，通常配合post、put方法使用，可以在req对象上新增如下三个属性

1. `req.body`请求文本域信息
2. `req.file`上传文件信息
3. `req.files`上传

通过`multer.single/array/field()`方法来获取multer中间件并使用（具体参数详见以下代码）

```typescript
import multer from 'multer';
const storage = multer.diskStorage({
  // 指定存储路径
  destination: function (req, file, cb) {
    cb(null, '/uploads')
  },
  // 指定存储文件名
  filename: function (req, file, cb) {
    cb(null, file.fieldname + '-' + Date.now())
  }
});
const limit = { // 上传限制
  fileSize: 1024*1024*10, // 文件最大10M
  files: 9， // 最多同时上传9个文件
};
const fileFilter = (req, res, cb) => { // 文件过滤器
  // ...
  cb(null, false); // 拒绝文件
  cb(null, true); // 接受文件
}
const upload = multer({storage, limit, fileFilter});

app.post('/upload', upload.single('file'), (req, res)=>{}); // 接受单个文件
app.post('/upload', upload.array('files', 9), (req, res)=>{}); // 接受文件，最大上传数量为9
app.post('/upload', upload.fields([ // 接受混合文件
  {name: 'file', maxCount: 1},
  {name: 'files', maxCount: 9},
]), (req, res)=>{});
```

