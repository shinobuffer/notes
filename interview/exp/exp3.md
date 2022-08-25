> TapTap 一面、Construct 一面

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

#### 登录态实现

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

