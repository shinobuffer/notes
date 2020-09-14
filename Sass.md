使用`$`来定义一个变量，变量是块级作用域的，变量内容可以是任意css属性值。引用变量时要带上`$`，在css属性值中引用变量相当于一个替换操作。可以用变量来定义变量、同一个变量多次声明后者覆盖前者

在sass中使用`!default`来强制生效变量，忽略变量声明的顺序，相当于css中的`!important`

```scss
$c1: gold;
$border1: 1px solid $c1;
//sass 提供了双斜杠注释，作用于一行
.title{
	color: $c1
}
```

sass中使用嵌套来避免重复编写的父子选择器

```scss
#content {
  article {
    h1 { color: #333 }
    > p { margin-bottom: 1.4em }
  }
  aside { background-color: #EEE }
}
/*相当于*/
#content article h1 { color: #333 }
#content article > p { margin-bottom: 1.4em }
#content aside { background-color: #EEE }
```

**选择器嵌套在解开时默认以空格间隔**，所以要想在嵌套中使用伪元素时需要使用父选择器`&`，它将被替换为父选择器。

```scss
#content aside {
  color: black;
  &:hover { color: red }
  body.ie & { color: green }
}
/*相当于*/
#content aside{color:black}
#content aside:hover{color:red}
body.ie #content aside{color:green}
```

群组选择器的嵌套遵从分配原则，类似一种完全组合，请小心使用，否则可能会编译出大量css。

```scss
.container {
  h1, h2 {color: red}
}
ul, ol {
  li {color: red}
}
/*相当于*/
.container h1, .container h2 {color: red}
ul li, ol li {color: red}
```

css属性同样可以嵌套，**属性嵌套在解开时默认以横杠间隔**

```scss
.nav {
  border: 1px solid red {
  left: 0;
  right: 0;
  }
}
/*相当于*/
.nav {
  border: 1px solid red;
  border-left: 0;
  border-right: 0;
}
```

sass中的`@import`是编译时的，这与css中运行时的`@import`是不一样的。在sass中使用`@import`时可以省略文件后缀。

一些专门用于`@import`的sass文件，不需要生成独立的css文件，这称为局部文件，局部文件命名以下划线开头如`_colors.scss`，导入局部文件时也可以省略前面的下划线。

sass中的`@import`可以在规则内使用，相当于替换操作。

```scss
//son.scss
a{
  color: red;
}
//father.scss
.title{@import "son.scss"}
//相当于
.title{
  a{
    color: red;
  }
}
```

sass中通过混合器`@mixin`来定义一组css属性作为变量来复用（不用带`$`），甚至，把一组属性和一组规则作为变量来复用（不用带`$`）。使用`@include`来引用混合器，相当于替换操作

```scss
@mixin ul-no-style {
  list-style: none;
  li {
    list-style-type: 0;
    margin-left: 0;
  }
}
ul {
  color: red;
  @include ul-no-style;
}
//相当于如下嵌套结构
ul {
  color: red;
  list-style: none;
  li {
    list-style-type: 0;
    margin-left: 0;
  }
}
```

如果一组css属性被重复使用在多个规则中且这些css属性所生成的效果是可以描述的（或者说这一组css属性放在一起是有意义的），那么这组css属性非常适合使用混合器来复用。相反的，如果是一组没有意义的css（如font和position）则不适合使用混合器，因为你很难用一个变量名来描述这个效果。


混合器可以通过传参来动态调整css属性值（类似于一个函数），比如说有多个a标签，他们的基础颜色不一样，hover、visited时的颜色也不一样，可以写一个含参混合器。定义时使用`$name: value`来定义默认参数。

```scss
@mixin link-colors($normal, $hover, $visited) {
  color: $normal;
  &:hover { color: $hover; }
  &:visited { color: $visited; }
}
//按参数顺序调用
a {
  @include link-colors(blue, red, green);
}
//分别指定参数调用
a {
    @include link-colors(
      $normal: blue,
      $visited: green,
      $hover: red
  );
}
//相当于
a { color: blue; }
a:hover { color: red; }
a:visited { color: green; }
```

sass中使用`@extend`来继承一个选择器所有相关样式，和混合器（将内容重复）不一样，继承在编译后被继承的属性只会保留一份，通过重复选择器来实现继承

```scss
.error { //被继承，应用到 .seriousError
  border: 1px solid red;
  background-color: #fdd;
}
.error a{ //被继承，应用到 .seriousError a
  color: red;
  font-weight: bold;
}
.seriousError {
  @extend .error;
  border-width: 3px;
}
//以上编译为
.error, .seriousError {
  border: 1px solid red;
  background-color: #fdd;
}
.error a, .seriousError a {
  color: red;
  font-weight: bold;
}
.seriousError {
  border-width: 3px;
}
```

继承是基于类的，用于语义化的重用，如上面的seriousError继承了error的css属性的同时添加了属于自己css属性。当一个类是另一个类的细化时，可以考虑使用继承。

**避免在后代选择器中使用继承**

> 入门到此为止，剩下靠实战磨炼
