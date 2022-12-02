> 框架永远都不会成为标准，框架永远都只能基于标准。他们不是同一层级上的东西，不存在可比性，更别提谁会替代谁

## Web Component

> 脱离框架，原生的组件化方案

原生支持意味着可以不需要任何框架即可完成开发，开发出来的组件亦可用于所有框架（无排他、无依赖）；并一定程度提高性能表现（无VDOM）和调试便利性（DOM 树扁平化）

其实这项技术并没有想象那么遥不可及，如果你有在开发者工具中仔细观察各类页面，你会发现这货无处不在。比如 input，video，select 等等，其实他们都是标准的原生组件；又比如 Chrome 的内置页面和 Youtube，检查一下源码你会发现一堆自定义的标签；而借助 Web Component 技术，你也可以创造这些原生组件

Web Component 由三个技术规范支持实现

- `Shadow DOM`-允许开发者将一个隐藏的、**独立的** DOM 附加到一个元素上
- `Custom Element`-允许开发者定义使用自己的元素（标签），扩展 HTML 语义
- `Template & Slot`-允许开发者复用相同的标记结构（Vue 的灵感来源之一）

### Shadow DOM

Shadow DOM 允许将隐藏的 DOM 树附加到常规的 DOM 树中，它以 shadow root 节点为起始根节点，在这个根节点的下方，可以是任意元素

<img src="./web-component.assets/shadowdom.svg" alt="img" style="zoom: 50%; background: white" />

操作 Shadow DOM 的方式和常规 DOM 方式一致，他们使用同一组 API 来创建、操作节点。不同的是，Shadow DOM 内部的元素和外部的元素互不干扰。事实上，有些微前端框架就是利用 Shadow DOM 充当 CSS 沙箱

使用`Element.attachShadow()`方法将一个 shadow root 附加到任何一个元素上，`mode`参数表示是否禁止通过`element.shadowRoot`来访问 Shadow DOM

```tsx
let shadowRoot1 = elementRef1.attachShadow({mode: 'open'});
let shadowRoot2 = elementRef2.attachShadow({mode: 'closed'}); // 禁止通过 elementRef2.shadowRoot 访问 Shadow DOM，video 元素就属于这种类型
```

将 Shadow DOM 附加到一个元素之后，就可以使用 DOM APIs 对它进行操作，就和处理常规 DOM 一样

```ts
const para = shadowRoot.querySelector('#para')
const para = shadowRoot.appenChild(para.cloneNode(true))
```

当 Shadow DOM 附加的对象是我们自定义的元素，一个 Web Component 的雏形就出来了，标记结构、样式、行为被隐藏在自定义元素的内部并且与外部的代码隔离开来了

### Custom Element

自定义元素**只能**通过继承`HTMLElement`来定义，调用`customElements.define()`声明注册该元素后，就可以直接在 HTML 上使用，或者通过`document.createElement()`创建并插入到 DOM 上

你可能会想能不能继承`HTMLXxxElement`来直接继承特定元素（进而白嫖里面的一些逻辑），答案是否定的，浏览器会报错给你看。如果有这需要，可以通过`customElements.define()`的第三个参数中的`extends`属性来表明要继承的目标元素，但这样的代价是你无法再直接使用自定义元素的标签名，转而要使用继承元素的标签配合`is`属性来使用自定义元素

```html
<script>
// 规定：自定义元素的名称至少包含一个中划线，以避免和浏览器的原生标签撞车
customElements.define('wc-example', class extends HTMLElement {/* ... */});
document.body.appendChild(document.createElement('wc-example'));
// 使用第三个参数，继承 input 元素。但使用时必须使用被集成标签配合 is 属性使用
customElements.define('wc-input', class extends HTMLElement {/* ... */}, { extends: 'input'});
document.body.appendChild(document.createElement('input', { is: 'wc-input' }));
</script>
<body>
  <wc-example></wc-example>
  <input is="wc-input" />
</body>
```

要想实现「自定义组件」，需要掌握三个要点：生命周期、数据传递和事件模型

------

自定义元素的生命周期如下

| 生命周期                   | 说明                               | 类型                                                  |
| -------------------------- | ---------------------------------- | ----------------------------------------------------- |
| `constructor`              | 定义元素时调用                     | --                                                    |
| `connectedCallback`        | 元素首次插入到 DOM 时调用          | `() => void`                                          |
| `disconnectedCallback`     | 元素从 DOM 删除时调用              | `() => void`                                          |
| `adoptedCallback`          | 元素被移动到其他位置时调用         | `() => void`                                          |
| `attributeChangedCallback` | 元素「属性」增加、删除、修改时调用 | `(attr:string, oldVal:string, newVal:string) => void` |

既然是类，第一个执行并且只执行一次的肯定是构造函数，触发时机是自定义元素被`document.createElement()`的时候，在构造函数中，主要完成 shadow root 的创建和一些初始化工作

`connectedCallback`和`disconnectedCallback`就如其名所示，会在插入到文档（`appendChild`）/从文档中移除（`removeChild`）时触发一次，可以理解为 React 中的`componentDidMount`和`componentWillUnmount`

`attributeChangedCallback`是一个重点生命周期，在介绍该生命周期之前，有必要了解`attribute`和`property`的区别。两者都有「属性」之意，但`attribute`指的是标签上的属性，而`property`指的是对象上的属性（标签和对象是同一元素的不同表示形式）。如果尝试修改标签上的`id`，会发现对象上的`id`也会被同步修改，反之亦然，给人一种「两者是同一回事」的错觉

```ts
const div = document.querySelector('div')
console.log(div) // 以标签的形式显示
console.log('%O', div) // 以对象的形式显示
div.setAttribute('id', 'xx') // attribute
div.id = 'xx' // property
```

实际上两者有着不少差异：

- `attribute`只能接受`string`类型的值，而`property`可接受所有 JS 支持的类型
- `property`中有一个字段`attributes`专门存储对应标签上的属性（实锤`attribute`是`property`的一部分）
- 给自定义字段赋值，无论是`attribute`还是`property`上的，都不会将值同步到另外一侧

PS：标签属性名不一定和对象属性名一致，如`attribute`的`class`在`property`中叫`className`

之所以有以上的错觉，是因为内置的标签元素实现了类似「双向绑定」的特性，将`attribute`和`property`上的值进行同步，但这种同步**仅限于标签的自有字段**，对自定义字段无效

------

如何让自定义字段也实现`attribute`和`property`的双向同步，是实现自定义元素数据传递的关键，解决方法出奇的简单，只需使用 getter/setter 充当`property`即可

- 要将`attribute`的变动同步到`property`，可利用 getter 在读取`property`时读取`attribute`的值进行返回（顺带处理默认值和类型转换）
- 要将`property`的变动同步到`attribute`，可利用 setter 在写入`property`时顺带写入到`attribute`中

注意：`attribute`只接受`string`类型的值

```ts
class Counter extends HTMLElement {
  // 需要监听变化的 attribute
  static get observedAttributes() {
    return ['count'];
  }

  private $btn: HTMLElement;
  private $shadowRoot: ShadowRoot;
  
  // getter、setter 实现 attribute 和 property 的双向绑定
  private get count(): number {
    return parseInt(this.getAttribute('count') || '0', 10);
  }

  private set count(value) {
    this.setAttribute('count', `${value}`);
  }

  constructor() {
    super();
    this.$shadowRoot = this.attachShadow({ mode: 'open' });
    this.$shadowRoot.appendChild(template.content.cloneNode(true));
    this.$btn = this.$shadowRoot.querySelector('.btn')!;
    this.render();
    this.$btn.addEventListener('click', () => {
      this.count += this.step;
    });
  }

  private render() {
    this.$btn.textContent = this.count.toString();
  }
  
  // attribute 变化回调
  public attributeChangedCallback() {
    this.render();
  }
}
```

有了数据之后，还需要监听数据的变化，适时重新渲染，这里就需要静态属性`observedAttributes`配合生命周期`attributeChangedCallback`来实现了

- `observedAttributes`表示要监听的`attribute`属性
- `attributeChangedCallback`会在任一被监听的`attribute`属性发生变动后被调用，入参分别是属性名，属性变动前后的值

再把渲染逻辑放在`attributeChangedCallback`，「数据驱动视图」不就来了嘛，只要`attribute`发生变化，就会触发渲染函数，读取`property`的同时把最新的`attribute`同步过来用于视图的构建，Perfect

------

自定义元素的事件可以分成两类：一类是内置原生事件，一类是自定义事件

- 大部分内置原生事件，如鼠标事件、focus 事件等，会自动向 Shadow DOM 外部传递并且将事件源重定向为自定义元素本身，无需任何处理
- 对于自定义事件，默认是只会在内部传递不会向外传递的，除非在派发事件的时候为事件指定了`bubbles: true`和`composed: true`

使用`element.dispatchEvent(event)`以自定义元素为事件源来派发一个自定义事件（DOM2），这里的`event`参数是一个`Event`类型的参数，对于自定义事件通常会使用`Event`的子类`CustomEvent`，在实例化事件的时候指定事件名、携带事件载荷、配置事件的传递性。之后便可像平时监听普通事件一样使用`element.addEventListener()`来监听自定义的事件

```ts
this.$btn.addEventListener('click', () => {
  // 在 detail 字段中携带自定义数据（载荷）
  this.dispatchEvent(new CustomEvent('btnClick', { detail: somePayload, bubbles: true, composed: true }))
  // 兼容 DOM0 事件（可选）
  this?.onBtnClick()
});

// 监听自定义元素的派发的自定义事件
document.querySelector('wc-element').addEventListener('btnClick', (e) => {
  console.log(e.detail)
})
```

### Template & Slot

`template`（模板）是浏览器的一个内置标签元素，在 HTML 中他不会被页面渲染，但是仍可以作为节被获取。利用这一特性，开发者可以把预先定义的 DOM 结构以模板的方式进行描述，在实际使用时克隆模板内部的节点来实现结构的复用，或者实现 UI 和数据的分离维持代码的整洁性

`slot`（插槽）也是浏览器的内置标签元素，他允许开发者在模板中定义占位符。在定义插槽时可以选择使用`name`属性给插槽分配名字（具名插槽），插槽对应的位置可以被其他元素代替，只要该元素加上了`slot`属性表明他要代替的插槽位置。插槽可有自己的默认内容，会在没被指名替代的时候生效

```html
<template id="wc-btn">
  <slot name='label'>Default Label</slot>
	<button class="btn">btn</button>
</template>
<!-- wc-btn 用的是上面的模板 -->
<wc-btn>
	<span slot="label">按钮</span>
</wc-btn>
<!-- 等价于 -->
<>
  <span>按钮</slot>
	<button class="btn">btn</button>
</>
```

PS1：`slot`元素有一个`slotchange`事件，会在对应替代元素的`slot`属性被设置/移除时调用

PS2：`slot`元素可通过`slotElement.assignedNodes()`来取得对应替代元素节点集合

### Using In React

> From: [React doc](https://react.docschina.org/docs/web-components.html)
>
> React 和 Web Components 为了解决不同的问题而生。Web Components 为可复用组件提供了强大的封装，而 React 则提供了声明式的解决方案，使 DOM 与数据保持同步。两者旨在互补。作为开发人员，可以自由选择在 Web Components 中使用 React，或者在 React 中使用 Web Components，或者两者共存
>

假设现在有一个 Web Component 开发的按钮组件`wc-button`，api 描述如下

1. `label`-按钮内容
2. `disabled`-是否禁用
3. `appearence`-外观
4. `click`-点击事件
5. `long-click`-长按事件

❌ 错误的用法-直接在业务代码中使用（理所当然的当成 React 组件使用了）

```tsx
import React from 'react';
import './wc-button';

const XXX = (props) => {
  return (
    <>
      {/* 其他业务代码 */}
    	<wc-button
        label="Button"
        appearence="link"
        onClick={console.log} // work，因为 click 事件是 element 的原生事件，而 React 会将 onClick 映射到该原生事件上
        onLongClick={console.log} // not work，不是原生事件，而且 React 不会处理处理他不认识的函数类型参数的映射
      />
    </> 
  );
};
```

✅ 正确的用法，建立适配/包装层，分别处理基础类型参数和事件回调

```tsx
import React from 'react';
import './wc-button';

interface Props {
  label: string
  disabled?: boolean
  appearence?: 'link' | 'outline'
  onClick?: () => void
  onLongClick?: () => void
}

const WcButton: React.FC<Props> = (props) => {
  const ref = React.createRef(null)
  const basicProps = useBasicProps(props)
  const fnProps = useFunctionProps(props)

  // 处理事件
  useEffect(() => {
    if(!ref.current) return;
    Object.entries(fnProps).forEach(([fnName, fn]) => {
      ref.current?.addEventListener(fnName, fn)
    })

    return () =>
      Object.entries(fnProps).forEach(([fnName, fn]) => {
        ref.current?.removeEventListener(fnName, fn)
      })
  }, [fnProps, ref.current])

  return <wc-dropdown {...basicProps} ref={ref} />
}

```

「为了在 React 中用上 Web Component，需要给每个组件都套一层，未免也过于麻烦了吧」，你可能会会想

先别急，你所遇到的问题一些组件库的开发者早就想到了，以微软的 [Fluent UI Web Component](https://fluent-components.azurewebsites.net/?path=/docs/integrations-react--page) 版为例，就贴心的提供了一个包装函数专门处理这些机械性重复工作，甚至允许在其上进行一些自定义配置如事件映射。最后在补充下声明文件就可以开始快乐的使用了

```tsx
import { provideFluentDesignSystem, fluentMenuItem, fluentButton } from '@fluentui/web-components';
import { provideReactWrapper } from '@microsoft/fast-react-wrapper';
import React from 'react';

const { wrap } = provideReactWrapper(React, provideFluentDesignSystem());

export const FluentButton = wrap(fluentButton());
export const FluentMenuItem = wrap(fluentMenuItem(), {
  // 事件映射（可选）
  events: {
    onExpandedChange: 'expanded-change',
  },
});

// web-components.d.ts
declare namespace JSX {
  interface IntrinsicElements {
    'fluent-menu': React.DetailedHTMLProps<React.HTMLAttributes<HTMLElement>, HTMLElement> & {
      'onExpandedChange': () => void;
    };
  }
}
```

> Web Components 并不是为了取代任何现有框架而生，它不会取代 React，也不会取代 Vue，Web Component 的目的是为了从原生层面实现组件化，可以使开发者开发、复用、扩展自定义组件，实现自定义标签，解决 Web 组件的重用和共享问题，并使 Web 生态保持持续的开放和统一
>

## Lit

> Lit is a simple library for building fast, lightweight web components

手撸一个 Web Component，又要 getter/setter 绑定、又要使用 DOM API 操作 DOM，对于用惯框架的人来说反而有点倒退了。这里介绍一个 轻量库 [Lit](https://lit.dev/)（前身 Google Polymer），Lit 在 Web Compoennt 上进一步进行封装，充分利用 JS 的新特性（语法糖）来减少繁琐重复的工作，并为习惯框架的开发者提供了较为友好的用法

> 为了获取最佳的开发体验，建议使用 vscode 的开发者安装「Lit-Plugin」以获取语法高亮和提示 

基类`LitElement`是 Lit 的核心组成，他继承自原生的`HTMLElement`，并在此基础上进行了扩展，提供包括响应式状态、生命周期以及一些诸如控制器和混入等高级用法。使用 Lit 开发 Web Component 要求开发者继承`LitElement`进行开发。下面是官方的一个「Hello World」栗子

```ts
import {LitElement, html, css} from 'lit';
import {customElement, property} from 'lit/decorators.js';

@customElement('my-element')
export class MyElement extends LitElement {
  static override styles = css`
    :host {
      display: block;
      border: solid 1px gray;
      padding: 16px;
      max-width: 800px;
    }
  `;

  @property()
  name = 'World';

  @property({type: Number})
  count = 0;

	private

  override render() {
    return html`
      <h1>${this.sayHello(this.name)}!</h1>
      <button @click=${this._onClick} part="button">
        Click Count: ${this.count}
      </button>
      <slot></slot>
    `;
  }

  private _onClick() {
    this.count++;
    this.dispatchEvent(new CustomEvent('count-changed'));
  }

  sayHello(name: string): string {
    return `Hello, ${name}`;
  }
}
```

代码使用了装饰器，这是 Lit 给开发者提供的一种可选方案，合理使用能极大降低模板代码的重复编写，比如以上代码中的两个装饰器

- `@customElement(elementName)`类装饰器，用于注册自定义组件

- `@property(options?)`成员装饰器，用于声明`attribute`和`property`的映射关系，`options`的可选配置如下
  
  | 字段         | 说明                                                        | 类型                                                         |
  | ------------ | ----------------------------------------------------------- | ------------------------------------------------------------ |
  | `type`       | 表明`property`的类型，作为`attribute`->`property`的转换函数 | String、Number...                                            |
  | `attribute`  | 表明`property`要和哪个`attribute`建立联系（默认小写同名）   | `boolean | string`                                           |
  | `reflect`    | 是否将`property`的变动同步到`attribute`上                   | `boolean`                                                    |
  | `converter`  | `attribute`和`property`的相互转换函数                       | `Function | {fromAttribute: Function, toAttribute: Function}` |
  | `noAccessor` | 是否监听`property`的变化并自动触发视图更新                  | `boolean`                                                    |
  | `hasChanged` | 返回布尔值表示是否需要更新视图                              | `(newVal, oldVal) => boolean`                                |

此外还有`@state`、`@query`、`@eventOption`等常用装饰器，具体可查看官方文档 [Decorators](https://lit.dev/docs/components/decorators/) 一节

注意：JS 装饰器仍在提案阶段（Stage3）仍未被各大浏览器支持，如需使用需要对 Babel 和 TS 进行[相关配置](https://lit.dev/docs/components/decorators/)

------

Lit 的渲染对应`render`生命周期，在该生命周期中会用到模板语法：通过`html`标签函数配合模板字符串来描述视图，利用模板字符串中的表达式来嵌入 JS 逻辑，支持多种表达式，有着非常强的即视感

| Type                                                         | Example                                                |
| :----------------------------------------------------------- | :----------------------------------------------------- |
| [Child nodes](https://lit.dev/docs/templates/expressions/#child-expressions) | html\`<h1>Hello ${name}</h1><ul>${listItems}</ul>\`      |
| [Attributes](https://lit.dev/docs/templates/expressions/#attribute-expressions) | html\`<div class=${highlightClass}></div>\`              |
| [Boolean Attributes](https://lit.dev/docs/templates/expressions/#boolean-attribute-expressions) | html\`<div ?hidden=${!show}></div>\`                     |
| [Properties](https://lit.dev/docs/templates/expressions/#property-expressions) | html\`<input .value=${value}>\`                          |
| [Event listeners](https://lit.dev/docs/templates/expressions/#event-listener-expressions) | html\`<button @click=${this._clickHandler}>Go</button>\` |
| [Element directives](https://lit.dev/docs/templates/expressions/#element-expressions) | html\`<input ${ref(inputRef)}>\`                         |

（这里的核心`html`函数来自`lit-html`，是一个独立的模板引擎，有兴趣可[了解](https://segmentfault.com/a/1190000039754597)）

组件的全局样式写在`styles`静态属性中，类似于`html`标签函数，Lit 提供了`css`标签函数，可以在样式中嵌入表达式，以复用 css 代码

```ts
import {css，unsafeCSS} from 'lit-element';
import style from './my-elements.less'

const commonCss = css`...`
const themeColor = css`#5abebc`

class MyElement extends LitElement {
	static styles = [
    commonCss,
    css`
    	div {
    		font-size: 12px;
    		color: ${themeColor}
    	}
    `,
    // 使用编译后的 less 样式  
    css`${unsafeCSS(style)}`
  ]
}
```

Lit 也支持动态 class 和 style，可以字符串的形式用表达式嵌入到模板中，也可以利用`styleMap()`和`classMap()`指令将 styleObj/classObj 转换后嵌入到模板中


## 参考

https://developer.mozilla.org/zh-CN/docs/Web/Web_Components
https://zhuanlan.zhihu.com/p/524015045
https://juejin.cn/post/7010580819895844878