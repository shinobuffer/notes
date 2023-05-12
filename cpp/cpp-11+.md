## C++ 17

### optional

头文件 `<optional>` 提供 `std::optional<T>` 类模板用于包装一个可空的值。一个 `optioanl` 对象要么包含一个类型为 `T` 值，要么包含一个 `std::nullopt` 表示为空。一个空的 `optioanl` 对象在条件表达式中会隐式的转换为 `false`，用于判断是否有值

在确定有值的情况下，可以像使用指针一样使用  `*/->` 访问 `optional` 对象中的目标值（不检查值是否存在）。成员函数 `op.value()` 同样可以获取目标值，但是会在值不存在时抛异常，成员函数 `op.value_or(def)` 则可以在值不存在时返回 `def` 作为替代

### string_view

`string_view` 用于表示字符串切片，即

`string_view` 和 `string` 可以相互转换

### 结构化绑定

==结构化绑定==允许将一个静态数组中的元素或者结构体中的成员按照其顺序绑定到指定的变量序列中，由编译器推断各个变量的类型。其基本形式为 `auto [v1, v2] = arr/struct`，和平时的 `auto` 类型推断一样，`auto` 关键字前可选配 `const` 限定符，关键字后可选配 `&/&&` 说明符表示引用绑定

> 结构化绑定要求全量绑定，不能只绑定部分成员，实在用有用不上的变量可以加上 `[[maybe_unused]]` 属性抑制警告

```c++
struct Person {
  string name;
  unsigned int age;
}

int arr[3] = {1, 2, 3};
Person p{"student", 24};
// 拷贝绑定
auto [e1, e2, e3] = arr;
// 引用绑定
auto& [name, age] = p;
```

### 初始化语句

## C++ 20