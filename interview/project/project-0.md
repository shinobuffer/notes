### 小说新书架

#### 概念

书架的两个场景：主书架（展示用户整体收藏）、分组书架（展示分组构成）

场景显示/操作对象称为格子 Case，格子承载的内容主要有两类，分别是书籍和分组，书籍和分组又有各自的子分类：

- 书籍
  - 普通书籍
  - 盗版书籍
  - 本地书籍
- 分组（由若干本书籍组成）
  - 普通分组
  - 盗版分组
  - 本地分组
  - 书单分组

场景的两个状态

- 常态（默认）：点击对象执行相应的跳转操作，长按对象呼出菜单
- 编辑态（点击编辑）：点击对象执行选中/取消选中操作，由菜单提供批量操作功能

菜单提供的操作

- 置顶
- 删除
- 移动（移回主书架、移动到新分组、移动到现有分组）

#### 基于二进制的编辑态模型

编辑态中菜单提供的操作功能不是一直都是可用的，某些特殊情况下某些操作会被禁用置灰并做相应提示，比如移动操作在用户选没有任何选中/选中数量过多时会禁用，在用户选中了本地分组也会被禁用。

> **问题**：不同的操作对应的「特殊情况」不尽相同，即存在交叉部分，又存在各自独有的部分。如何为两个不同的场景的编辑态提供统一的操作行为控制？
>
> **排除方案**：不可能为每个操作都硬编码 if-else 逻辑来实现可用性判断
>
> **解决方案**：引入「特殊状态」和「状态集」的概念，参考 React lane 模型，使用二进制数表示「特殊状态」和「状态集」，方便进行目标状态的检查和优先级的计算

- 特殊状态（人为事先定义）：描述特定场景下，特定选中情况，使用某个位为1，其余位为0的二进制数表示，且1的位置越靠右，对应的优先级越高。比如在主书架内选中了本地分组，可以定义为一个特殊状态`0b1`；选中数量超过100本，也可以定义为一个特殊状态`0b10`。
- 状态集：因为支持多选，用户可能处于若干个特殊状态的叠加的状态，这些特殊状态的或结果就是状态集。在用户操作的过程中，会实时根据场景和选中情况更新这个二进制数

有了状态集这个二进制数，就可以基于位运算方便的实现状态检查和优先级计算：

为每种操作都定义一个 bitMask，表示各个操作需要关注的一些特殊状态。在计算状态可用性的时候，只需将状态集对应的二进制数和 bitMask 进行与运算，检查结果是否为 0 即可得知是否存在导致当前操作禁用的特殊状态。同时，从结果中提取最右方的 1 即可计算出最高优先级的特殊状态，用于获取相应提示

#### 手势事件差异屏蔽层

> **目标**：在用户长按位置附近弹出菜单，顺带解决安卓子组件超出父容器部分无法响应手势事件的问题
>
> **问题1**：Hippy 长按事件没有坐标信息
>
> **解决方案1**：通过两个手势事件的组合解决，利用 onTouchDown 获取触点坐标，利用 onLongClick 处理长按
>
> **问题2**：不同场景、不同操作系统下，手势事件中取到的坐标其参考系不尽相同
>
> **解决方案2**：在前端层屏蔽原生差异，统一坐标系，使得触点位置严格相对于页面的根容器

书架两种场景下页面渲染方式是不同的：

在主书架场景，页面是直接内嵌在 HippyFrame 容器中渲染的，容器的上下方是终端原生绘制区域，页面从容器上边界开始布局。在分组书架场景，页面是通过 Modal 渲染实现全屏效果的，页面从手机屏幕顶部开始布局

- 经过真机测试发现，安卓上通过 Frame 容器渲染的页面，其手势事件触点参照系不符合预期（预期是以容器为参照系，实际上却以手机屏幕为参照系）
- 针对这种特殊情况编写一个 formatter，对传入的原生手势事件进行参照系修正，返回修正后的事件
- 封装一个`TouchPointRecorder`插槽组件，内部绑定原生 touchDown 并利用 formatter 修正原生手势事件，重新对外暴露修正后的 touchDown 回调。并把修正后的触点坐标写到内存中，供菜单组件访问
- 使用的时候只需要用`TouchPointRecorder`组件包裹目标组件即可正确记录目标组件的点击位置

#### 弹窗逻辑抽象、复用、集中管理

项目涉及十余种业务弹窗，且部分弹窗间存在关联，如何复用和管理？

> 目标：尽可能优雅的编写弹窗组件代码，尽可能优雅方式管理弹窗

首先是复用，抽象所有弹窗的共性，一个最基础的弹窗由以下部分构成

- 背景蒙层（封装为一个支持位置定义的插槽组件，真正的弹窗包裹在其中）
- 标题、可选标题（支持 string/render props）
- 主题内容，可以是文字、选项列表、表单等（children 插槽）
- 可选的取消/确认按钮（支持 string/render props）

根据这些构成实现一个基类弹窗，然后再基于基类弹窗封装出业务所需的十余种弹窗组件实现复用

___

然后就是弹窗的管理，就我在其他项目中的见闻，少量弹窗的管理方案通常是定义几个布尔变量分别控制弹窗的显示和隐藏，但是这种做法对于涉及十余种弹窗的书架项目来说，实在是太不雅了。所以我决定基于全局状态管理，利用 rematch 创建一个新模块，对弹窗进行集中管理，并对外暴露简单易用的接口调起业务弹窗



为了实现集中管理，第一步是对弹窗进行规范化处理，所有业务弹窗的 props 都必须严格受到统一数据结构的约束，我将其定义为泛形接口`DialogCommonProps<T>`，包含两个字段：

- data 字段是泛形，表示外部向弹窗传递的数据
- onInteraction 字段是一个回调函数，函数接收 DialogInteraction 枚举类型，用于告知外部用户和弹窗发生了什么类型的交互

DialogInteraction 枚举定义了交互类型，比如 Confirm 泛指点击肯定的按钮、Cancel 泛指点击否定的按钮，Blank 泛指点击空白区域等等，外部可通过回调的入参得知用户点击了弹窗的哪个功能区域
```typescript
/**
 * 弹窗的最终有效行为，
 * 这里“最终有效”指的不是单纯的物理点击，而是经过一系列过滤和验证之后的有效行为，有点类似于上报。
 * 【比如点击确认按钮后，需要发送请求并根据响应来判断本次行为是否有效】
 */
export enum DialogInteraction {
  /**
   * 空白（泛指点击非空白非主体区域），
   * 注意使用该行为时，需要防止非空白区域的穿透
   */
  Blank,
  /** 确认（泛指点击肯定的按钮） */
  Confirm,
  /** 取消（泛指点击否定的按钮） */
  Cancel,
  /** 关闭（点击X按钮） */
  Close,
  /** 选择（泛指点击各种选项） */
  Option,
}

export interface DialogCommonProps<Data = any> {
  /** 弹窗最终有效行为回调 */
  onInteraction: (action: DialogInteraction) => void;
  /** 弹窗传参 */
  data: Data;
}
```
第二步是将弹窗实例抽象为一个数据结构`DialogMetaInfo`弹窗元信息，由唯一标识id、弹窗组件、层叠优先级、触发弹窗关闭的交互类型、弹窗类型、弹窗传参组成

多个弹窗元信息以数组的形式存储，在此基础上提供一个`DialogProvier`插槽组件来根据弹窗元信息数组渲染对应的弹窗，由`DialogProvier`来接管弹窗的渲染、参数传递和交互回调，具体如何接管由上面提到的`DialogMetaInfo`指定。这样就实现了由元信息数组驱动弹窗的渲染
```typescript
interface DialogMetaInfo {
  /** 弹窗唯一标识 */
  id: string;
  /** 弹窗组件 */
  component: React.FC<DialogCommonProps>;
    /**
   * 弹窗层叠优先级，越高越优先。
   * 若不提供，则自动为弹窗分配跟当前显示弹窗中最高的zIndex相等的值（即作为当前最高优插入）
   */
  zIndex?: number;
  /**
   * 注意：使用该属性前，请确保在弹窗组件中正确使用onAction回调埋点，否则不会生效
   * 可以导致弹窗关闭的弹窗行为。
   * 若不提供则默认Confirm/Cancel/Close/Blank行为会导致弹窗关闭
   */
  closeOnInteractions?: DialogInteraction[];
  /** 是否以Modal形式打开，默认否 */
  openWithModal?: boolean;
  /** 弹窗传参 */
  data?: Data;
}
```

到这里已经是能用的状态了，但不推荐直接使用，因为直接操作元信息数组对代码的侵入性太大了。为了解决这个问题，我对完整的弹窗管理逻辑进行了分层，提供了三个层级的 API，通过层层封装来不断简化 API 的使用：

- 在底层，提供元信息数组的更新函数：`updateDialogMetaInfos()`
- 在中层，提供通用弹窗控制函数：`openDialog()`、`closeDialog()`，对传入的弹窗配置进行校验并生成元信息，并调用底层函数来操作元信息数组，实现任意弹窗的打开和关闭
- 在上层，提供各个业务弹窗 open 函数，通过生成各自的弹窗配置并调用中层的`openDialog()`函数来具体打开某一个弹窗

开发者在实际开发需要打开某个弹窗时，只需简单调用最上层对应的 API 并传递约定的参数即可

至于弹窗的关闭，通常是不用开发者手动调用关闭的，关闭的逻辑已经以配置化的形式记录在元信息中，`DialogProvier`组件会将回调中的互动类型和元信息中的关闭逻辑进行比对，在相应互动发生时自动关闭当前弹窗，开发者在编写弹窗组件时做好相应的互动埋点即可

```typescript
const { modal : openDeleteBookDialog } = usedispatch();
openDeleteBookDialog({fromId: 'group1'})
```