## Vue + Ts

npm依赖

```sh
npm install vue vue-class-component vue-property-decorator vuex vuex-class -S
npm install vue-loader vue-template-compiler typescript ts-loader tslint tslint-loader tslint-config-standard -D
```

webpack配置

```js
// plugins添加配置
plugins: [
  new VueLoaderPlugin()
]
// loader添加配置
module: {
  rules:[{
    test: /\.vue$/,
    loader: 'vue-loader'
  }, {
    test: /\.tsx?$/,
    loader: 'ts-loader',
    options: { transpileOnly: true, appendTsSuffixTo: [/\.vue$/] },
    exclude: /node_modules/
  }]
}
```



## Sass 相关

npm依赖，如果css要单独打包可以不装 style-loader，转而采用 mini-css-extract-plugin

```sh
npm install style-loader css-loader node-sass sass-loader postcss-loader autoprefixer@9.8.6 -D
# 下面用于css抽取和css压缩
npm install mini-css-extract-plugin optimize-css-assets-webpack-plugin -D
```

webpack配置

```js
const MiniCssExtractPlugin = require("mini-css-extract-plugin");
// plugins添加配置
plugins: [
  new MiniCssExtractPlugin({
    filename: 'css/main.css' //css抽离输出路径
  }),
  new OptimizeCSSPlugin()
]

// loader添加配置
module: {
  rules: [{
    test: /\.css$/,
    use: [
      MiniCssExtractPlugin.loader,
      {
        loader: 'css-loader',
        options: { importLoaders: 1 }
      },
      {
        loader: 'postcss-loader',
        options: {
          postcssOptions: {
            plugins: [require('autoprefixer')]
          }
        }
      }
    ]
  }, {
    test: /\.scss$/,
    use: [
      MiniCssExtractPlugin.loader,
      {
        loader: 'css-loader',
        options: { importLoaders: 2 }
      },
      {
        loader: 'postcss-loader',
        options: {
          postcssOptions: {
            plugins: [require('autoprefixer')]
          }
        }
      },
      'sass-loader'
    ]
  }]
}
```

## 其他

```sh
npm install webpack-dev-server html-webpack-plugin clean-webpack-plugin -D
```

