#include <iostream>
#include <string>
#include <vector>
#include <cmath>

std::vector<int> get_vector();
std::vector<int> get_vector() {
  return {};
}

std::string get_string();
std::string get_string() {
  return "666";
}

class Vec {
public:
  // 类内声明构造函数
  Vec();
  Vec(double x, double y);

  // 类内声明常量成员函数 length
  double length() const;

  // 类内定义成员函数，自动内联
  double x() {
    return x_;
  }
  double y() {
    return y_;
  }

private:
  // 数据成员，由构造函数负责初始化
  double x_, y_;
};

// 类外定义构造函数，请求编译器合成默认构造函数
Vec::Vec() = default;
// 类外定义构造函数，使用初始化列表初始化数据成员
Vec::Vec(double x, double y): x_(x), y_(y) {}

// 类外定义常量成员函数 length
double Vec::length() const {
  return std::sqrt(x_ * x_ + y_ * y_);
}

int main(int argc, char const *argv[])
{
  Vec v;
  std::cout << v.length();
  return 0;
}
