# feflow教程

> tnpm install feflow-cli -g
>
> 第一次安装后执行`feflow`初始化

如果在mac/linux下feflow安装失败无法启动，尝试修改全局安装路径到权限区（用户目录下面）下并配置环境变量（我之前就算加了sudo也死活提示无权限，用这方法解决了）

## 自定义脚手架

feflow的脚手架是基于yeoman的，详情参考[此处](https://yowebapp.github.io/authoring/index.html)。下面介绍如何基于feflow开发自己的脚手架。

脚手架的本质是nodejs模块，开始之前为脚手架想个名字并接在`generator-`后面作为脚手架名称，下面使用`generator-test`为例（脚手架的名字叫test）。

创建目录`generator-test`，进入目录执行`npm init`生成npm配置文件，**其中description字段必填**（feflow要用到），并安装如下包`yeoman-generator、yosay、chalk、shelljs`

通常来说，一个脚手架有以下目录结构（先不理node_modules等文件）

```sh
generator-test/
  └───generators/
        └───app/
         		└──index.js
        		└───templates/
```

`generators/app/templates`文件夹描述了项目的目录结构，在于用户完成交互后，里面的文件将被拷贝到新建项目里

`generators/app/index.js` 集合了和用户交互的逻辑，它在执行`feflow init`并选中脚手架后被执行。通常`generators/app/index.js` 内容如下，它导出一个继承Generator的类，内有多个钩子函数，他们按序执行。

```js
const Generator = require('yeoman-generator');

module.exports = class extends Generator {
  	constructor () {}
    // 初始化阶段
    initializing () { /* code */ },
    // 接收用户输入阶段
    prompting () { /* code */ },
    // 保存配置信息和文件
    configuring () { /* code */ },
    // 执行自定义函数阶段
    default () { /* code */ },
    // 生成项目目录阶段
    writing () { /* code */ },
    // 统一处理冲突，如要生成的文件已经存在是否覆盖等处理
    conflicts () { /* code */ },
    // 安装依赖阶段
    install () { /* code */ },
    // 结束阶段
    end () { /* code */ }
}
```

除以上函数名外的每种方法被认为是一个任务，它们都在default阶段执行，你可以通过定义私有类方法（下划线开头）或实例方法来避免该方法被当成任务自动执行。

initializing阶段，初始化阶段，打印些脚手架相关的信息等



prompting阶段，主要是在命令行和用户交互，获取用户偏好设置。该阶段调用`generator.prompt`方法来获取用户输入，`prompt`方法是异步的，你需要在互动结束后收集用户的输入（answer）

```js
  prompting() {
    return this.prompt([
      {
        type: 'input', //文本输入
        name: 'name',
        message: '请输入项目名称',
        default: 'my-project'
      },
      {
        type: 'confirm', //是否确认
        name: 'tsEnable',
        message: '是否启用ts',
      },
      {
        type: 'input',
        name: 'version',
        message: '请输入版本 (1.0.0):',
        default: '1.0.0'
      }
    ]).then((answers) => {
      this.answers = answers;
    });
  }
```

writing阶段，主要在生成项目文件，将文件从源（模板）搬运到目标（项目），涉及以下方法

`generator.destinationRoot([path])`获取目标路径（项目位置）或设定目标路径

`generator.destinationPath(path)`拼接到目标路径并返回

`generator.sourceRoot([path])`获取源路径（模板位置）或设定源路径

`generator.templatePaht(path)`拼接到源路径并返回

`generator.fs`暴露了文件操作的方法（全程在内存中）

- `fs.copy(src,dest)`复制模板文件，路径中可以使用`!*`表示否定和模糊

- `fs.copytpl(src,dest[,data])`复制并处理模板文件，data将作为模板的数据，模板内使用ejs语法获取这些数据

  

install阶段，主要进行依赖安装，你可以使用`generator.npmInstall(dependencies,options)`执行一次npm安装，亦可以以CLI方式执行安装（引入shelljs处理）

```js
install () {
  this.npmInstall(['md5'], { 'saveDev': true }); // 安装md5
  // shell.exec('tnpm install')
}
```

end阶段，项目构建完成，可以输出些使用提示。



其他：

- 使用`generators.log()`方法代替`console.log()`
- 使用`generators.option(name,{type})`来获取来自命令行的参数如`--dev`，之后可通过`this.option.dev`获取参数值

## 跑起来！

本地测试脚手架，需要将脚手架链接到npm全局目录和`.feflow/`下

```sh
cd <your-path>/generator-test
npm link
cd ~/.feflow
npm link generator-test
```

修改`~/.felfow/package.json`文件，在`dependencies`字段添加`"generator-test": "1.0.0"`，让feflow能找到脚手架，之后就可以执行`feflow init`测试执行脚手架了。

如果你还装了yeoman，执行`yo test`来测试执行脚手架

