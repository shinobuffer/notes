> PDD 一面、TapTap 一面、Construct 一面

#### TS 中 Interface 和 Type 的区别

相同点：都能描述对象/函数；都可以被类实现；都可扩展（`type`用交叉类型，`interface`用`extends`）

不同点

- `type`可以给基本类型、联合类型、元组类型起别名，但`interface`不行
- `type`中可以使用`in`关键字来实现动态属性，但`interface`不行
- `interface`可以重复定义来合并声明，但`type`不行

```typescript
type Partial<T> = {
  [P in keyof T]?: T[P];
}

type Required<T> = {
  [P in keyof T]-?: T[P];
};

type Pick<T, K extends keyof T> = {
  [P in K]: T[P];
};

type ReturnType<T extends (...args: any) => any> = T extends (
  ...args: any
) => infer R
  ? R
  : any;
```

#### Loader 和 Plugin 的区别
Webpack 本身只能处理 JS 和 JSON 文件，**loader 为 webpack 添加了处理其他类型文件的能力**，loader 本质上是一个转换函数，将其他类型文件转换为 webpack 可识别的模块（md-loader/webp-loader）

**Plugin 用于扩展 webpack 的功能**。webpack 运行的生命周期中会广播出许多事件，Plugin 可以监听这些事件在合适的时机通过 webpack 提供的 API 来改变输出结果（upload-plugin）

#### Webpack 工作原理

> 从入口文件开始，分析依赖的模块，并使用相应的 loader 对依赖的模块进行加载，将加载结果转换为 AST 用于分析和收集当前模块依赖的子模块，就这样不断递归的加载模块和收集依赖模块，直至所有依赖的模块都完成加载，即完成整个项目的打包

初始化阶段：

- 执行 webpack 命令，将命令行参数和配置文件进行整合，得到完整的配置项
- 校验配置项并配置项初始化`compiler`对象，完成内部/三方插件的注册

构建阶段：

- 执行入口函数`compiler.run()`，最终调用`compiler.compile()`开始项目的构建流程
- 构建开始创建`compilation`对象（构建过程的上下文对象），并触发`make`钩子进入`make`阶段
- `make`阶段从入口模块开始解析，找出依赖的模块并加入到依赖列表中
- 调用`buildModule()`对依赖的模块进行构建，使用相应的 loader 对模块进行加载，将加载结果转换为 AST，分析 AST 来继续收集当前模块依赖的子模块，不断重复加载模块和收集依赖模块的过程，直至所有依赖模块被处理，得到编译产物和依赖关系图

生成阶段：根据依赖关系，将多个模块封装成`chunk`，并把`chunk`转换为`asset`加入到输出列表，最终输出到文件系统

#### shuffle 实现和验证

```typescript
export const shuffle = (arr: number[]): number[] => {
  // 逆序遍历数组，随机选取当前位置前方的一个位置，和当前位置进行交换
  // 相当于无放回的抽签问题，以此为每个位置放置抽到的数
  for (let i = arr.length - 1; i >= 0; i--) {
    const randIndex = Math.floor(Math.random() * (i + 1));
    [arr[randIndex], arr[i]] = [arr[i], arr[randIndex]];
  }

  return arr;
};

// 验证方式
// 一张牌出现在任何位置的概率是相等的。
// 一个位置出现任何牌的概率是相等的。
```

#### 手写简单 useState

```typescript
// 使用数组来模拟 hook 链表（实际上存在fiber.memoizedState上）
const stateList: any[] = [];
let index = 0;

export const useState = (initialState) => {
  // 当前执行的 useState hook
  const currentIndex = index;
  // 处理首次初始化
  stateList[currentIndex] = stateList[currentIndex] ?? initialState;

  // 更新状态后调度更新
  const setState = (newState) => {
    stateList[currentIndex] = newState;
    render();
  };

  // index 后移，为下一个 hook 做准备
  index++;

  return [stateList[currentIndex], setState];
};

// 想办法触发更新，比如使用 ReactDOM.render
const render = () => {
  index = 0;
  // ReactDOM.render(<App />, document.getElementById('root'));
};
```

#### 手写简单 useEffect

```typescript
type Deps = any[];
type DestroyFun = () => void;

// 使用数组来模拟 effect 链表，存储上次的依赖和销毁函数（实际上存在hook.memoizedState上）
const depsList: (Deps | undefined)[] = [];
const destroyList: (DestroyFun | undefined)[] = [];
let index = 0;

export const useEffect = (callback: () => DestroyFun, deps?: Deps) => {
  // 当前执行的 useEffect hook
  const currentIndex = index;
  // 上次的依赖
  const prevDeps = depsList[currentIndex];
  // 首次挂载（无上次依赖）/无 deps/依赖变动，需要进行回调
  const changed = !prevDeps || !deps || deps.some((dep, i) => dep !== prevDeps[i]);

  if (changed) {
    depsList[currentIndex] = deps;
    // 先执行上次的销毁，再执行本次的创建
    destroyList[currentIndex]?.();
    destroyList[currentIndex] = callback();
  }
  // index 后移，为下一个 hook 做准备
  index++;
};

export const clearEffectIndex = () => {
  index = 0;
};
```

