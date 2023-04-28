> 指令小写、关键字大写、形参小写

### 变量

使用 `set(var values...)` 指令来声明变量 `var` 并初始化为 `values` 序列（序列中多个值空格分隔，若值含有空格或关键字则需引号包裹）；若变量已存在，则相当于赋值操作。默认情况下 `set(var values...)` 声明的变量是==局部变量==，只能在当前文件使用。局部变量的传递规则默认父传子，子不传父，若子模块（`add_subdirectory`）中要修改或新增父级的变量，需要在最后加上 `PARENT_SCOPLE` 关键字

跨文件/跨命令共享变量需要使用==缓存变量==，缓存变量只有一份并且记录在 `CMakeCache.txt` 文件中，使用 `set(var values CACHE STRING help [FORCE])` 声明缓存变量并指明缓存变量的类型和描述，默认情况下该指令不会覆盖已有的缓存变量（除非指明了 `FORCE`）。缓存变量也可以外部执行命令时通过 -D 强制覆盖，如 `cmake -D USE_LIB=ON`

系统的==环境变量==需要 `$ENV{name}` 引用 

> 在指令中引用变量时需使用 `${}` 包裹（并且可以嵌套使用），在条件表达式中使用时可以省略
>
> 使用 `${}` 引用存在重名的变量时，局部变量的优先级高于缓存变量；但可以使用 `$CACHE{name}` 来强制引用缓存变量

此外还有多种方式设置和修改变量

- `list(APPEND var values...)`，往已有变量 `var` 中追加 `values` 序列
- `option(var help defValue)`，定义变量 `var`，让用户根据提示 `help` 输入变量值，默认值为 `defValue`（不会覆写已有的缓存变量）
- `cmake_host_system_information(RESULT var QUERY infos)`，查询系统信息 `infos` 并写入变量 `var`

cmake 提供了大量内置变量，主要分布在 `CMakeCache.txt/CMakeXXX.cmake` 文件中，比较常用的内置变量有

- `CMAKE_BUILD_TYPE`，指定构建类型，其值可以是 `Debug/Release`，在指定了构建类型后，会使用对应内置变量 `CMAKE_CXX_FLAGS_XXX` 作为全局编译选项
- `CMAKE_CXX_STANDARD`，指定 C++ 标准
- `CMAKE_CXX_STANDARD_REQUIRED`，是否要求编译器支持指定的 C++ 标准
- `CMAKE_CURRENT_LIST_DIR`，当前 CMakeLists.txt 所处目录
- `PROJECT_SOURCE_DIR/PROJECT_BINARY_DIR`，项目源码目录/项目输出目录
- `CMAKE_CURRENT_SOURCE_DIR/CMAKE_CURRENT_BINARY_DIR`，当前 CMakeLists.txt 的源码目录/输出目录

### 表达式

`foreach-endforeanch` 遍历一个序列，并将当前遍历的值存入 `item` 变量，有如下使用方式

- `foreach(item lists...)`
- `foreach(item IN LISTS var)`，其中 `var` 是已有变量，无需使用 `${}` 包裹

`if-elseif-else-endif` 用于条件判断，变量可以直接作为条件表达式，值为 `1/ON/true/Y` 的变量等价于【真】，值为 `0/OFF/false/N` 的变量等价于【假】。更复杂的条件表达式则需要配合运算关键字实现

```cmake
# 逻辑运算
if(NOT con1 AND con2 OR con3)

# 存在性相关
if(TARGET target) # 判断构建目标是否存在
if(DEFINED [CACHE/ENV] var}) # 判断变量是否存在
if(item IN_LIST lists) # 判断指定值是否在序列中

# 文件/目录相关
if(EXISTS path) # 判断文件/目录是否存在
if(IS_DIRECTORY path) # 判断是否为目录

# 正则匹配
if(v MATCHES regex)

# 数字比较（字符串比较需要加上 STR 前缀，版本号比较需要加上 VERSION_ 前缀）
if(vl LESS vr)
if(vl GREATER vr)
if(vl EQUAL vr)
if(vl LESS_EQUAL vr)
if(vl GREATER_EQUAL vr)
```

生成器表达式 `$<$<var:values>:exp>`，当变量 `var` 的值为 `values` 序列（逗号分割）中的一个时返回 `exp`，否则返回空字符串

### 构建相关

#### add_executable

> `add_executable(target [files...])`
>
> 作用：添加一个可执行文件构建目标

使用 `add_executable` 添加构建目标时可同时指定源文件（对应 `files` 参数），也可以后续使用 `target_source` 追加源文件

#### add_library

> `add_library(target STATIC/SHARED [files...])`
>
> 作用：添加一个库构建目标

第二个参数 `STATIC/SHARED` 关键字分别对应静态库/动态库

#### target_source

> `target_source(target PUBLIC files...)`
>
> 作用：为构建目标追加源文件

#### target_link_libraries

> `target_link_libraries(binTarget libTargets...)`
>
> 作用：将库连接到可执行文件

#### set_target_properties

> `set_target_properties(target PROPERTIES propPairs...)`
>
> 作用：设置构建目标的属性

参数 `propPairs` 是要设置的属性键值对，可多个，常用的属性有

- `CXX_STANDARD/CXX_STANDARD_REQUIRED`，指定编译时的 C++ 标准
- `LIBRARY_OUTPUT_DIRECTORY`，动态库输出路径
- `ARCHIVE_OUTPUT_DIRECTORY`，静态库输出路径
- `RUNTIME_OUTPUT_DIRECTORY`，执行文件输出路径

#### target_compile_definitions

> `target_compile_definitions(target PUBLIC defs...)`
>
> 作用：为构建目标的源文件注入宏定义

宏可以不指定值如 `DEF`，也可以指定值如 `DEF=1`

#### target_include_directories

> `target_include_directories(target PUBLIC dirs...)`
>
> 作用：为构建目标的源文件添加头文件搜索路径

#### target_compile_options

> `target_compile_options(target PRIVATE flags...)`
>
> 作用：为构建目标指定编译选项

### 路径收集

#### file

> `file(GLOB var CONFIGURE_DEPENDS patterns...)`
>
> 作用：收集相对路径满足匹配规则 `patterns` 的文件（非递归），并存入变量 `var`

可以将第一个参数换成关键字 `GLOB_RECURSE` 来递归的收集

```cmake
# 收集当前目录下的直接 *.cpp *.h 文件
file(GLOB srcList CONFIGURE_DEPENDS *.cpp *.h)
# 递归收集当前目录下的所有 *.cpp *.h 文件
file(GLOB_RECURSE srcList CONFIGURE_DEPENDS *.cpp *.h)
```

#### aux_source_directory

> `aux_source_directory(dir var)`
>
> 作用：自动根据当前语言设置，收集指定目录 `dir` 下的源文件（非递归），并存入变量 `var`

### 三方依赖

#### find_packages

> `find_packages(pkg [CONFIG/MODULE] [COMPONENTS comps... REQUIRED])`
>
> 作用：检查指定包是否存在，并生成形如 `pgk::libname` 的伪对象供链接使用

`find_package` 会优先在 `/usr/share/cmake/Modules` 以及 `CMAKE_MODULE_PATH` 变量指定的路径下查找 `Findpkg.cmake` 文件，执行里面的逻辑得到包相关信息（对应 `MODULE` 模式）

在找不到 `Findpkg.cmake` 文件情况下， 则在 `/usr/lib/cmake/pkg` 下查找查找 `pkgConfig.cmake` 作为替代，也可以通过设置环境变量或缓存变量 `pkg_DIR` 来说明该文件的搜索路径（对应 `CONFIG` 模式）

若指定了 `REQUIRED` 关键字，则在找不到依赖时会报错并终止；未指定则提供 `pkg_FOUND`、`pkg_DIR` 等缓存变量供后续判断使用，不会报错终止

```cmake
find_package(Qt5 COMPONENTS Widgets Gui REQUIRED)
target_link_libraries(main PUBLIC Qt5::Widgets Qt5::Gui)
```

### 其他常用指令

- `cmake_minimum_required(VERSION version FATAL_ERROR)`，限制 cmake 最低版本
- `project(name)`，指定项目名
- `message([leval] msg)`，打印日志，日志类型 `level` 可以是关键字 `STATUS/WARNING/FATAL_ERROR` 中的一种，其中 `FATAL_ERROR` 会报错并终止
- `configure_file(tempPath confPath)`，填充模板并输出配置文件

