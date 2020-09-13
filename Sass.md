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