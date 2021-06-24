> webpack4安装`webpack、webpack-cli`
> webpack5安装`webpack@next、webpack-cli`
> 其中webpack5实现更为强大的tree-shaking，能自动实现无用代码的去除，无需额外配置

webpack作用：根据配置文件，通过项目入口，递归地收集依赖关系，将项目打包，顺便做下代码转化和文件优化。


## entry & output & resolve
使用`entry、output`字段指定webpack的项目入口和打包输出
```javascript
module.exports = {
  // 开发环境
  mode: 'developement',
  //入口
  entry: './src/index.js',
  output: {
    //输出文件名，附加八位hash
    filename: ' build.[contenthash:8].js',
    // 非入口chunk输出文件名
    chunkFilename: '[name]_chunk.js',
    //输出路径（需绝对路径）
    path: path.resolve(__dirname, 'dist'),
    //publicPath: '/', // 资源路径追加前缀，多用于生产环境
    // library: '[name]', // 向外暴露为，多用于库打包DLL
    // libraryTarget: 'window', // 向外暴露到，多用于库打包DLL
  }
}
```
若多入口多输出，`enrty`字段应为一对象，描述「name=>entry file」的映射

```js
module.exports = {
  entry: {
    // 「name=>entry file」的映射
    home: './src/index.js',
    other: './src/other.js'
  },
  output:{
    // 使用[name]来引用enrty对象中的键
    filename: '[name].js',
    path: path.resolve(__dirname,'dist')
  }
}
```

在`resolve`字段配置解析规则

```javascript
module.exports = {
  resolve: {
    //指定解析的模块
    modules: [resolve('node_modules')],
    //省略后缀，import 'index' 如果不存在依次检查 index.js, index.css
    extensions: ['.js','.css']
    //路径别名
    alias: {
      '@': './src' // 导入'@/index.js' 等价于'./src/index.js' 
    }
  }
}
```

## 常用loader

对代码进行转换的部分称为loader，在配置中对应`module`字段，值是一个对象。`module.rules`数组描述了各类文件的处理方法和顺序。`loader`是有处理顺序的，默认**从左到右，从下到上**。

### 样式处理

以sass为例，安装`sass-loader, node-sass, css-loader, postcss, postcss-loader, postcss-preset-env, mini-css-extract-plugin`，其中`postcss-loader`用于添加浏览器前缀，`css-loader`用于处理@import语法，`mini-css-extract-plugin`插件用于将css抽离成独立文件供引用。安装完后进行如下配置

```js
const MiniCssExtractPlugin = require("mini-css-extract-plugin");
const postcssLoader = {
  loader: 'postcss-loader',
  options: {
  	postcssOption: {
  		plugins: [
        // require('autoprefixer'),
        require('postcss-preset-env')(),
      ]
  	}
  }
};

module.exports = {
  module: {
    rules: [ // 下面的每个rule对应一类文件的处理
      { // 处理css文件：「兼容性处理=>@import处理=>抽离处理」
        test: /\.css$/,
        use: [
          MiniCssExtractPlugin.loader,
          'css-loader',
          postcssLoader,
        ]
      },
      { // 处理scss文件
        test: /\.scss$/,
        use: [
          MiniCssExtractPlugin.loader,
          'css-loader',
          postcssLoader,
          'sass-loader',
        ]
      },
    ]
  },
  plugins: [new MiniCssExtractPlugin({
    filename: 'css/main.css' //抽离输出路径
  })]
}
```

最后在`package.json`文件中追加`browserslist`字段用于描述需要兼容的浏览器

```json
{
  "browserslist": {
    "development": [
      "last 1 chrome version",
      "last 1 firefox version",
      "last 1 safari version"
    ],
    "production": [
      "id > 6",
      ">1%"
    ]
  }
}
```

css压缩需要用到插件`optimize-css-assets-webpack-plugin`，使用时new入`plugins`字段即可。

### 代码处理

以TS为例，其处理流程是「TS=>JS=>JS兼容」，安装`babel-loader, @babel/core, @babel/preset-env, core-js, `

```js
module.exports = {
  module: {
    rules: [
      {
        test: /\.ts$/,
        exclude: /node_modules/,
        use: [
          { // js兼容处理
            loader: 'babel-loader',
            options: {
              cacheDirectory: true, // 开启缓存
              presets: [
                '@babel/preset-env',
                {
                  targets: { // 兼容目标
                    chrome: '60',
                    ie: '9'
                  },
                  corejs: 3, // corejs版本
                  useBuiltIns: 'usage', // 按需加载
                }
              ]
            }
          },
          'ts-loader',
        ]
      }
    ]
  }
}
```

### 其他处理

若要处理css中`url(./img.jpg)`这类文件资源的导入，需要使用`url-loader、file-loader`，前者处理`url(xxx)`，后者处理文件。若要处理html中`src=./img.jpg`这类文件资源的导入，需要使用`html-loader`。

```js
module.exports = {
	module: {
		rules: [
			{
				test: /\.(jpe?g|png|gif)$/,
        loader: 'url-loader',
        options: {
          limit: 1000, // 低于1000kb的文件直接base64编码
          name:'[name].[ext]', // 输出文件名，维持原名
          outputPath: 'img', // 输出目录，相对于ouput路径
          //publicPath: 'https://oshinonya.com' //引用头加上域名
          // url-loader默认es6解析，html-loader默认commonjs解析，需统一
          esModule: false, 
        }
			},
			{
				test: /\.html$/,
        loader: 'html-loader',
			},
		]
	}
}
```

## 开发必备

如果要使用开发服务器，npm安装`webpack-dev-server`，使用`devServer`字段进行配置，启动命令为`webpack-dev-server`

```javascript
module.exports = {
  devServer:{
    port: 8080, // 端口
    progress: true, 
    contentBase: './dist', // 代码运行目录
    compress: true, // gzip压缩
    hot: true, // 开启热更新

    proxy: { //代理转发
      '/root': { // /root开头的请求代理到target
        target: 'http://localhost:1919',
        changeOrigin: true,
        pathRewrite: { // 代理后重写url，去除/root
          '^/root' : ''
        }
      }
    }
  }
  devtool: 'source-map',
}
```

如果需要打包html模板，npm安装插件`html-webpack-plugin`，创建插件实例并使用

```javascript
const HtmlWebpackPlugin = require("html-webpack-plugin");
module.exports = {
  plugins:[
    new HtmlWebpackPlugin({
      template: './src/index.html', //模板路径
      filename: 'index.html', //输出文件名
      minify: {
        collapseWhitespace: true, //消除空行
        removeComments: true, //消除注释
      },
      hash: true
    })
  ]
}
```

## 构建优化

每个文件都会尝试去跟所有loader匹配，但通常来说只会命中一个，可以通过`oneOf`将不相干扰的loader放在一起，这样一旦命中其中的一个loader就会终止之后无用的匹配

```js
rules: [
	{
    oneOf: [
      // js loader
      // css loader
    ],
    // other loader
  }
]
```

在`optimization`字段中配置一些webpack优化项

```js
module.exports = {
  optimization: {
    splitChunks: {
      chunks: 'all',
      minSize: 30000,//触发分割的最低体积
      minChunks: 1,//触发分割的引用次数
      automaticNameDelimiter: '~', //名称链接符
      cacheGroups: {
        vendors: {//第三方模块
          test: /[\\/]node_modules[\\/]/,
          priority: -10 //优先级更高
        },
        default: {//自己的代码
          minChunks: 2，
          test: /[\\/]src[\\/]/,
          priority: -20，
          reuseExistingChunk: true,
        }
      }
    },
    // 将当前模块记录其他模块的hash单独打包为一个runtime文件，避免缓存失效
    runtimeChunk: {
      name: entrypoint => `runtime-${entrypoint.name}`,
    },
    minimizer: [
      // 配置生产环境的压缩方案：js、css
      new TerserWebpackPlugin({
        cache: true, // 开启缓存
        parallel: true, // 多线程打包
        sourceMap: false, // 是否sourceMap
    	})
    ]
  }
}
```

多进程打包，安装`thead-loader`，通常放在babel-loader之上

```js
use: [
  {
    loader: 'thread-loader',
    options: {worker: 2}
  },
  // babel-loader
]
```

某些库通过CDN加载，通过`external`字段禁止某些库被打包

```js
module.exports = {
  externals: {
    jquery: '$' // 库名=>全局变量
  }
}
```

使用dll对某些不变库进行单独打包，这需要另起一份配置文件专门用于这些库的打包，最终生成一份映射文件。在开发生产环境中导入该映射文件告知不打包这些库。

```js
// webpack.dll.js
module.exports = {
  mode: 'production',
  entry: {
    vue: ['vue', 'vuex'], // 对vue全家桶单独打包 
  },
  output: {
    filename: '[name].js',
    path: resolve(__dirname, 'dll'),
    library: '[name]_[hash]', // 向外暴露的名字，用于建立到filename的映射
  },
  plugins: [
    new webpack.DllPlugin({
      name: '[name]_[hash]', // 对应 output.library
      path: resolve(__dirname, 'dll/manifest.json') // 该文件提供library到outputFile的映射文件
    })
  ]
}
// webpack.config.js
module.exports = {
  plugins: [
    new webpack.DllReferencePlugin({
      manifest: resolve(__dirname, 'dll/manifest.json'), // 加载隐射文件，里面的库将不参与打包
    }),
    // 若不想手动在html中导入打包的库，可以使用下面的插件辅助
    // new AddAssetHtmlWebpackPlugin([filepath: resolve(__dirname, 'dll/vue.js')])
  ]
}
```

## 其他

### 变量引入

如果你在大部分文件都要导入一个模块，可以考虑在每个文件中注入该模块
```javascript
let webpack =require('webpack');
module.exports = {
  plugins:[
    new webpack.ProvidePlugin({
      $: 'jquery' //注入jquery为$
    })
  ]
}
```
### 环境区分&全局常量
创建多个webpack配置文件供不同环境使用：
- `webpack.base.js`，公共配置
- `webpack.dev.js`，开发配置
- `webpack.prod.js`，生产配置

并在开发/生产配置中整合公共配置
```javascript
// webpack.prod.js
let {smart} = require('webpack-merge')
let base = require('./webpack.base.js')

module.exports = smart(base,{
  mode:'production'
  //其他配置
})
```
使用插件`webpack.DefinePlugin`定义全局常量，可以在模块中直接使用
```javascript
let webpack = require('webpack');
module.exports = {
  plugins:[
    new webpack.DefinePlugin({
      BASE_URL: JSON.stringify('https://oshinonya.com'),
      IS_PROD: 'true'
    })
  ]
})
```
