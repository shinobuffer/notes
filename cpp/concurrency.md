### chrono

`<chrono>` 头文件提供了计时相关的支持，包含三大类型，他们都定义在 `std::chrono` 命名空间下

- ==时钟==可通过成员函数 `c.now()` 获取当前时刻，根据计时起点的不同存在不同类型的时钟
  
  - `system_clock`，系统时钟，通常对应系统的 UTC 时间
  - `steady_clock`，单调时钟
- ==时间段 duration<T, Period>==表示两个时刻的时间间隔，其中 `Period` 表示单位，`T` 表示该单位下的数值类型。`std::chrono` 提供了常用单位下的类型别名，如 `milliseconds/seconds/minutes`，可使用这些预设类型来构造时间段对象
- ==时间点 time_point<Clock, duration>==表示当前时钟的某个时刻，本质是计时起点 + 时间段，因此时间点也隐含着单位

时间点和时间段之间可以相互运算，如时间点作差可得到时间段，时间点加减时间段得到新的时间点；当不同单位的对象参与运算时，其结果单位会隐式的转换到更高精度的单位上

不同精度单位进行隐式转换时，仅当没有潜在的精度丢失时转换才会成立，使用 `duration_cast<to_duration>()` 则可以忽略精度丢失进行强制转换，具体规则可看下面示例

```c++
std::chrono::milliseconds i_ms{3}; // 3ms
auto d_us = std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(i_ms); // 3000.0us
// 允许构造自低精度单位
std::chrono::microseconds i_us = i_ms; // 3ms -> 3000us
// 当源数值单位为整型时，禁止构造自高精度单位，因为有可能被截断导致精度丢失
std::chrono::milliseconds from_i_us = i_us; // error
// 当源数值单位为浮点时，可以构造自任意精度的单位 (C++17)
std::chrono::milliseconds ms_from_d_us = d_us;
```

### 锁
