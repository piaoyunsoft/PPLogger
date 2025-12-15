# PPLogger - 轻量级跨平台彩色日志系统 🎨

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Windows-lightgrey.svg)](https://www.microsoft.com/windows)
[![C++](https://img.shields.io/badge/C++-11-00599C.svg)](https://isocpp.org/)

> 一个遵循接口隔离原则的现代化 C++ 日志库，支持彩色输出、文件记录、线程安全

## ✨ 特性

### 核心功能
- ✅ **多级别日志** - Debug/Info/Warning/Error/Fatal 五个级别
- 🎨 **彩色输出** - Windows 原生彩色 + ANSI 跨平台支持
- 📁 **文件日志** - 支持日志持久化到文件
- 🔒 **线程安全** - 使用互斥锁保证并发安全
- ⚡ **零开销** - Release 版本可完全禁用日志（编译优化）
- 🧩 **模块化设计** - 遵循接口隔离原则（ISP）

### 设计亮点
- 🎯 **单例模式** - 全局统一访问点
- 🔌 **策略模式** - 灵活切换日志实现
- 🎭 **外观模式** - 简化复杂接口
- 🛡️ **RAII 风格** - 自动资源管理
- 📝 **格式化支持** - printf 风格的格式化输出

---

## 🚀 快速开始

### 5行代码搞定日志

```cpp
#include <PPLogger.hpp>

int main()
{
    PPLOG_INFO("程序启动成功！");
    PPLOG_ERROR("错误码: %d", 404);
    return 0;
}