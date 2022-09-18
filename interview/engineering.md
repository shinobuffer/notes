> 笔记需要优化

#### Webpack loader 和 Plugin

> loader: 将不同类型的文件转换为 webpack 可识别的模块
>
> plugin: 扩展 webpack，加入自定义的构建行为，使 webpack 可以执行更广泛的任务，拥有更强的构建能力。

loader、plugin 区别

- plugins 在整个编译周期都起作用
- loader 只在构建模块前被调用，完成模块的转换

loader 本质上是一个函数，该函数对接收到的内容进行转换，返回转换后的结果（同步或异步）。webpack 处理一个模块时，根据配置中的规则选择对应的 loader 来处理资源

```typescript
export default function SynLoader(content, map, meta) {
  // 还可以使用上下文提供的一些方法属性实现高级功能
  // this.getOptions() // 获取 loader 配置
  // this.addDependency() // 加入一个文件，作为 loader 产生的结果的依赖
  
  return someSyncOperation(content);
  // 如果想要返回内容以外的东西，使用 this.callback()
  // this.callback(null, content, map, meta);
  // return;
}

export default function AsyncLoader(content, map, meta) {
  // 以回调的方式异步返回
  const asyncCallback = this.async();
  asyncWork(content, map, meta)
    .then(({content, map, meta})=>asyncCallback(null, content, map, meta))
}
```

plugin 本质上是一个类，且必须包含`apply`方法才能接受到 webpack 注入的 compiler 对象

`webpack`基于发布订阅模式，在运行的生命周期中会广播出许多事件，插件通过监听这些事件，就可以在特定的阶段执行自己的插件任务。在 `emit` 事件发生时，代表源文件的转换和组装已经完成，可以读取到最终将输出的资源、代码块、模块及其依赖，并且可以修改输出资源的内容

```typescript
class MyPlugin {
  // Webpack 会调用 MyPlugin 实例的 apply 方法给插件实例传入 compiler 对象
  apply (compiler) {
    // 找到合适的事件钩子，实现自己的插件功能
    compiler.hooks.emit.tap('MyPlugin', compilation => {
      // compilation: 当前打包构建流程的上下文
      console.log(compilation);
    })
  }
}
```

#### Webpack 打包优化

Tree Shaking

- 业务包提供 esm 规范，代码优先使用解构导入
- 启用`optimization.useExports: true`
- 在`npm.sideEffects`标注副作用代码，避免被 tree shaking 干掉

JS代码压缩和分割（按需/并行加载）

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
      // JS 代码压缩
      new TerserWebpackPlugin({
        cache: true, // 开启缓存
        parallel: true, // 多线程打包
        sourceMap: false, // 是否sourceMap
      })
    ]
  }
}
```

CSS 代码压缩和分离

```js
module.exports = {
  module: {
    rules: [
      {
        test: /\.css$/,
        use: [
          MiniCssExtractPlugin.loader,
          'css-loader',
          postcssLoader,
        ]
      }
    ]
  }
  
  optimization: {
    minimizer: [
      new CssMinimizerPlugin(), // CSS 代码压缩
    ]
  }
}
```

#### Webpack 构建速度优化

loader 优化：`test`单一匹配、`include/exclude`限制生效范围

resolve 优化：`extension`种类不要过多、`modules`指定三方某块路径、`alias`路径别名

合理使用`sourceMap`

DLLPlugin 将不常变动的三方代码独立打包，在业务配置中引用

```js
// webpack.dll.js
module.exports = {
  entry: {
    venders: ["react"]
  },
  output: {
    filename: "[name].js",
    path: resolve(__dirname, "dll"),
    library: "[name]_[hash]" //打包出来的库对外暴露出来的内容叫什么名字
  },
  plugins:[
    new webpack.DllPlugin({
      name: '[name]_[hash]',
      path: path.resolve(__dirname, "dll/mainfest.json")
    })
  ]
}

// webpack.prod.js
module.exports = {
  plugins: [
    // 引用DLL，告诉webpack哪些库不参与打包
    new webpack.DllReferencePlugin({
      manifest: path.resolve(__dirname, "dll/manifest.json")
    }),
    new AddAssetHtmlWebpackPlugin([
      {
        filepath: path.resolve(__dirname, "dll/venders.js"),
        publicPath: "./"
      }
    ])
  ]
}
```

