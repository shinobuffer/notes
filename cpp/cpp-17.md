## C++17

### filesystem

> 偷懒一下，std::filesystem 简写为 fs

#### path

`path` 用于表示路径，可隐式构造自字符串。**注意 `path` 是路径表示，而不是路径**，同一个路径可以有多种表示，比如 "/usr/bin" 和 "/usr/./bin" 两种路径表示都会解析到同一个路径，但这两个路径表示是不相等的（即 `p1==p2` 会得到 `false`），如果需要判断两者解析的路径是否相等需使用 `fs::equivalent(p1, p2)`
- 路径表示中允许存在 `.` 或者 `..` 表示当前/上级目录
- 路径表示中如果存在连续的路径分隔符，在解析时会被当成一个，如 "/usr//bin" 和 "/usr//bin" 的指向是一致的
- 路径分绝对和相对，绝对路径指的是 `p.root_path()` 非空的
- `path` 之间可以使用 `/=, /, +=` 运算符进行拼接
  - `/` 拼接相对路径时会自动加上路径分隔符 "/"，拼接绝对路径时直接返回右侧绝对路径，对应 `append(p)` 成员函数
  - `+=` 拼接是直接拼接，对应 `concat(p)` 成员函数
- `path` 是可迭代的，迭代时按照根、根目录、后续文件名的顺序进行迭代（不含路径分隔符）

除了构造函数之外，也有一些常用的函数用于特定 `path` 的获取和转换，比如 `fs::current_path()` 返回当前路径，`fs::canonical(p)` 校验路径是否存在的同时转换为规范路径表示


#### 文件访问

> 文件访问涉及系统接口的调用，大部分读写函数在路径非法或者权限限制的情况下会出现异常，可以通过捕获 `fs::filesystem_error` 来处理，也可以使用不抛异常的重载版本，通常只需传多一个 `std::error_code` 参数即可

判断文件是否存在： `fs::exist(p)`

判断文件类型：形如 `fs::is_xxxx`，比如 `is_regular_file(p)`、`is_directory(p)` 等

获取文件属性：如 `fs::status(p)` 获取文件属性（内含文件类型、权限）、`fs::space(p)` 获取文件大小等

拷贝文件：
- `fs::copy_file(src_p, dst_p, [options])` 拷贝单个非目录文件专用，默认情况下如果目标路径指向已经存在的文件会抛异常（可通过 `options` 参数修改默认行为）
- `fs::copy(src_p, dst_p, [options])` 拷贝任意文件（可通过 `options` 参数修改默认行为）
  - 源文件非目录，则相当于在 `fs::copy_file` 的基础上允许目标路径指向已经存在的目录（将源文件拷贝到目标目录）
  - 源文件为目录，则目标文件也必须为目录（可以不存在但是父目录必须存在，后续会自动创建空目录），默认拷贝源目录的直接文件到目标目录中

移动文件：`fs::rename(src_p, dst_p)` 如果目标路径指向已经存在的文件，要求两者同为非目录/目录，否则抛异常
- 源文件非目录，如果目标路径指向已经存在的文件，**会进行覆盖**
- 源文件为目录，如果目标路径指向已经存在的非空目录，会抛异常

删除文件：允许删除不存在的路径（无操作），并通过返回值告知是否执行了删除操作
- `fs::remove(p)` 删除单个文件/空目录，删除非空目录会抛异常
- `fs::remove_all(p)` 递归删除文件/目录（含自身）

创建目录：允许创建已经存在的路径（无操作），并通过返回值告知是否执行了删除操作
- `fs::create_directory(p)` 非递归
- `fs::create_directories(p)` 递归

#### 目录遍历

`fs::directory_iterator(p, [options])` 来获取一个目录的迭代器（要求路径指向存在且可访问的目录），迭代器会非递归的迭代目标目录的直属文件（不含自身）。迭代项的类型为 `fs::diector_entry`，提供大量成员函数供文件属性的获取和类型的判断

> PS：另外也存在递归版本的迭代器 `fs::recursive_directory_iterator`



### optional

`std::optional<T>` 类模板用于包装一个可空的值。一个 `optioanl` 对象要么包含一个类型为 `T` 值，要么包含一个 `std::nullopt` 表示为空。一个空的 `optioanl` 对象在条件表达式中会隐式的转换为 `false`，用于判断是否有值

在确定有值的情况下，可以像使用指针一样使用 `*/->` 访问 `optional` 对象中的当前值。成员函数 `op.value()` 同样可以获取当前值，但是会在值不存在时抛异常，成员函数 `op.value_or(def)` 则可以在值不存在时返回 `def` 作为替代

> 使用 `*/->` 访问 `optional` 对象不会进行非法访问的检查，访问前需确保值存在

### variant

`std::variant<T...>` 类模板包装一个可能有多个类型的值。一个 `variant` 对象可以直接被赋值为其支持的类型的值，由对象自动管理旧类型值的销毁和新类型值的构建

访问 `variant` 对象中保存的值必须知道当前值的类型，然后通过 `std::get<T>(v)` 获取当前值的引用。或者知道当前值类型在模板实参列表的下标，然后通过 `std::get<N>(v)` 获取当前值的引用。为了确定当前值的类型或类型下标，需要使用如下函数

- `std::holds_alternative<T>(v)`，判断当前值的类型是否为 `T`
- `v.index()`，返回当前值的类型在模板实参列表的下标

```c++
std::variant<int, float> var = 1;
// 如果当前值为整型，以该类型获取
if (std::holds_alternative<int>(var)) {
  std::cout << std::get<int>(var); // 1
}
// 如果当前值的类型为第二个类型，以该类型获取
var = 2.0;
if (var.index() == 1) {
  std::cout << std::get<1>(var); // 2.0
}
```

为了处理 `variant` 对象，可以使用 `std::visit(callable, vars...)` 将不同类型的处理方式“打包”成一个可调用对象，根据实际的类型选择对应的处理方式。“打包”的方式可以是 `()` 运算符的重载，也可以是自带模板特性的 `auto lambda`，后者会在编译阶段生成所有可能类型的实例，确保运行时能处理所有可能的情况

```c++
template<class... Fs>
struct overload: Fs... {
  using Fs::operator()...;
};
// 将多个函数包装成一个可调用对象
template<class... Fs>
overload(Fs const &...) -> overload<Fs...>;

std::variant<int, std::string> var = "123";
// 1. 分发到可调用对象的不同重载上
std::visit(overload(
  [] (int v) {/* 处理 int 的情况 */},
  [] (std::string v) {/* 处理 string 的情况 */},
), var);

// 2. 分发到 auto lambda 的不同实例上
std::visit([] (auto v) {
  if constexpr (std::is_intergral<decltype(v), int>){
    // 处理 int 的情况
  }else {
    // 处理 string 的情况
  }
}, var);
```

### string_view

`string_view` 底层由一个指向起始位置的指针和一个长度组成，用于表示字符串视图的**只读引用**，`string_view` 可以构造自 `string/const char*`

- `string_view(cs)`，构造对字符串 `cs` 的完整引用
- `string_view(cp, [n])`，构造对常量 C 风格字符串 `cp` 范围 `[0, n)` 的引用

`string_view` 只是一个只读引用，本身并不管理内存的分配和回收，因此拷贝一个 `string_viwe` 对象只是拷贝了内部的指针和长度，底层的字符串并不会被拷贝

> 和指针/引用类似，如果源字符串因某种原因被销毁，`string_view` 对象将失效，对其访问的行为是未定义的
>
> `string_view` 的成员函数基本上对应着 `string` 的成员函数（除了部分会修改字符串本身的成员函数）

各种字符串类型的转换规则

| 源类型      | explicit? | 目标类型    |
| ----------- | --------- | ----------- |
| const char* | No      | string_view |
| string      | No      | string_view |
| const char* | No      | string      |
| string_view | Yes      | string      |

`string/const char*` 可以隐式的转换为 `string_view`，`string_view` 类型的函数形参可以接受各种形式的常量字符串，避免函数重载和实参拷贝

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

允许在 `if/switch` 判断之前额外执行一条表达式，放在条件之前，用分号分隔

```c++
if (int flag = false; condiction) {
  flag = true;
}
// 等价于
int flag = false;
if (condiction) {
  flag = true;
}
```

### misc

- 类模板的模板参数可以自动推导了，不用显式指定，如 `vector v = {1, 2};` 将自动推导出 `vector<int>`

