#### 水平垂直居中方案

万能方案（单独水平/垂直都可，行内/块级元素都可）

- 万能的 flex 布局（grid 布局也行）
- 先相对/绝对 50% 定位，后 translate 纠偏（知道宽高的话，也可以使用 margin 来纠偏，或者用 calc 一步解决）

其他水平居中方案

- 对行内元素的父容器使用`text-align: center`
- 对块级元素使用`margin: auto`

其他垂直居中方案

- 对行内元素使用`vertical-align: center`

#### 多栏布局方案

> 万能的 flex 布局

双栏布局

- 一列浮动/绝对定位，一列 margin 让位
- 一列浮动，一列创建 BFC

三栏布局（双侧定宽，中间自适应）

- 两侧浮动/绝对定位，中间 margin 让位

#### Sticky-footer 方案

footer 固定高度方案A

- html、body 去除内外边距，设高度 100%
- 容器设置`min-height: 100%`，至少撑满屏幕
- 内容区通过`padding-bottom: footerHeight`让位给 footer
- footer 通过绝对定位贴容器底

```html
<div class="container">
  <div class="header"></div>
  <div class="content">内容区</div>
  <div class="footer">我要贴底</div>
</div>
```

footer 固定高度方案B

- 前三步同方案A
- footer 相对定位，通过`top/margin-top: -footerHeight`偏移到上去

```html
<div class="container">
  <div class="header"></div>
  <div class="content">内容区</div>
</div>
<div class="footer">我要贴底</div>
```

footer 不固定方案

- html、body 去除内外边距，设高度 100%
- 容器 flex-column 布局，设置`min-height: 100%`，至少撑满屏幕
- 内容区设置`flex: 1`尽可能扩大

```html
<div class="container">
  <div class="header"></div>
  <div class="content">内容区</div>
	<div class="footer">我要贴底</div>  
</div>
```

#### 隐藏元素方案

- `display: none`，不占据空间（直接在 render-tree 上消失）、不响应事件、触发**回流和重绘**
- `visibility: hidden`，占据空间、不响应事件、只会触发**重绘**
- `opacity: 0`，占据空间、响应事件、只会触发**重绘**
- 绝对定位离屏（不推荐）

#### CSS 优化

- 关键 css 内联、css 压缩 
- 避免选择器嵌套过深、避免使用通配符/属性选择器
- 充分利用继承特性，避免属性重复编写
- 考虑适当减少图片的请求（雪碧图、base64编码）
- 减少重排和不必要的重绘，参考[浏览器渲染流程](./browser.md#浏览器渲染流程)

#### BFC

> BFC（Block Formatting Context 块级格式化上下文）可以理解为元素的一个标识，带 BFC 标识的元素将创建**独立块级渲染区域**，使得该区域**内外的元素渲染不会相互影响**

触发 BFC 的情况

- display 的值为 inline-block、flex、flow-root 等
- float 的值不是 none
- position 的值不是 static 或者 relative
- overflow 的值不是 visible

BFC 规定了内部的块级盒子如何布局

- 从顶部开始，盒子在垂直方向按序排列，盒子间的垂直 margin 重叠
- BFC 中，每一个盒子的左外边距与包含块的左边界相接触，即使是存在浮动元素也是如此，除非该盒子建立了一个新的 BFC
- BFC 高度计算包含浮动子元素
- BFC 区域不与浮动元素重叠

BFC 常见用法

- 避免垂直方向的 margin 重叠（让两个兄弟处在不同的 BFC 中）
- 避免子元素浮动导致父容器高度坍塌（让父容器创建 BFC）
- 阻止元素被浮动元素覆盖（让被覆盖的非浮动元素创建 BFC）
