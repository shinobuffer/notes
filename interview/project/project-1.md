### 小说频道性能优化

- 参照浏览器 performance API 标准实现打点工具库
- 分析项目渲染流程（小说项目是一个多 tab 页面，渲染方式分有缓存和无缓存两种），划分阶段，设计埋点方案
  - 记录缓存标识（渲染方式）、记录首屏 tab 位置（渲染页面）

  - 有缓存：*TerminalInitial->AppInitial->LoadCache->Render*

  - 无缓存：*TerminalInitial->AppInitial->LoadCache->FetchCardsPrepare->FetchCards->FetchCardsProcess->Render*


```typescript
enum Mark {
  /** hippy 引擎创建时刻 */
  HippyCreate = 'HippyCreate',
  /** 构造器开始 */
  AppConstructorStart = 'AppConstructorStart',
  /** 卡片缓存加载开始 */
  LoadCacheStart = 'LoadCacheStart',
  /** 卡片缓存加载结束 */
  LoadCacheEnd = 'LoadCacheEnd',
  /** 首屏请求开始（卡片） */
  FetchCardsStart = 'FetchCardsStart',
  /** 首屏请求结束（卡片） */
  FetchCardsEnd = 'FetchCardsEnd',
  /** 首屏请求开始（tab） */
  FetchTabsStart = 'FetchTabsStart',
  /** 首屏请求结束（tab） */
  FetchTabsEnd = 'FetchTabsEnd',
  /** 渲染开始 */
  RenderStart = 'RenderStart',
  /** 首屏第一张卡片渲染结束 */
  RenderFirstCardEnd = 'RenderFirstCardEnd',
  /** 首屏最后一张卡片渲染结束 */
  RenderEnd = 'RenderEnd',
}
```

分析收集到的性能数据，发现耗时的大头主要在以下阶段

- 终端初始化耗时（虽然不是我们能直接控制的部分，但能够通过减包来间接优化）
- 请求前准备耗时（将无依赖关系的异步进行并发处理，超时保护）
- 渲染耗时（优化层级结构，组件重构）

减包工作这块能做的优化有很多

- 删除冗余/过时代码
- 将不合理的 base64 图片硬编码改为网络图片
- 充分利用 tree-shaking（把所有业务包都改用 rollup 打包，提供 esm 规范的业务包）
- 工具库/组件懒加载（使用火焰图分析 JS 执行流程过程中的调用栈，找出首屏加载非必要组件）

