# JCBase Library

一个轻量级、模块化的 C 语言基础工具库，提供不可变字符串（String）、可变字符串（MutableString）、动态数组（ArrayList）、链表（LinkedList）等常用数据结构，以及 输入模块。

**设计目标**：语义清晰、安全、易用，适合作为 C 项目的底层基础组件。

## 各个模块及更新日志

[点击查看更新日志](UPDATE.md)<br>
[点击查看ArrayList.md](ArrayList.md) [点击查看ArrayList_EN.md](ArrayList_EN.md)<br>
[点击查看String.md](String.md) [点击查看String_EN.md](String_EN.md)<br>
[点击查看MutableString.md](MutableString.md) [点击查看MutableString_EN.md](MutableString_EN.md)<br>
[点击查看IO.md](IO.md) [点击查看IO_EN.md](IO_EN.md)<br>
[点击查看LinkedList.md](LinkedList.md) [点击查看LinkedList_EN.md](LinkedList_EN.md)

---

## 功能特性

**统一错误处理**

- 所有函数通过返回值报告状态（成功/失败/非法参数等）
- 每个模块有独立的状态枚举，状态码定义在各自的 `XxxSrc.h` 里
- 提供状态码转字符串函数，便于调试

---

## 快速开始

### 1. 获取源码

从 GitHub 克隆或下载本仓库。

### 2. 构建动态库（Visual Studio 示例，其他构建方式可通过将 wheel_ex.h 发给懂得怎么构建的人或是你信任的 AI）

- 使用 Visual Studio 创建 C++ DLL 项目（或空项目）
- 将所有 `.c` 和 `.h` 文件添加到项目中
- 关闭预编译头（项目属性 → C/C++ → 预编译头 → 选择"不使用预编译头"）
- 在 `wheel_ex.h` 中，`JCBASE_EXPORTS` 宏已在 DLL 项目中自动定义，确保导出符号
- 编译为 Release 模式，生成 `.dll` 和 `.lib` 文件

### 3. 在你的项目中使用

- 包含头文件：`#include "JC_String.h"`、`#include "JC_ArrayList.h"` 或 `#include "JC_MutableString"`等（根据需求）
- 链接生成的 `.lib` 文件，运行时确保 `.dll` 在搜索路径中
- 所有 API 均以 `wheels` 修饰符导出（Windows 专用，其他平台可忽略，实际测试中或许在使用时无需使用修饰符）

---

## ⚠️ 重要提醒

### 1. 若要确保安全，请始终检查实例状态

在调用任何操作函数之前，务必使用 `jcxx_getStatus` 检查实例是否为可用状态。

非法状态（如 带有 `NULL` 的）可能导致未定义行为。

```c
if (jcal_getStatus(list) != JC_AL__AVAILABLE) {
    // 处理错误或退出
}
```
最好不要将带有指针的结构体类型传给容器，容器使用浅拷贝，如果传入了，请注意是否该释放某个资源

### 2. 内存管理

使用 `jcxx_delete` 释放实例，库不会自动回收。

对于 ArrayList / LinkedList，元素数据由库管理（逐字节拷贝），但元素本身若包含指针（如动态分配的字符串），指针指向的内容仍需调用者自行管理。

`jcstr_splite` / `jcms_splite` 返回的数组和其中的 String / MutableString 对象都需要分别释放（先释放每个 String / MutableString，再释放数组本身）。

### 3. 错误码含义

每个模块的状态码定义在各自的 `JC_XxxSrc.h` 里，包含该模块时会自动包含状态码，无需手动包含：

可使用 `jcxx_statusToCharArray` 将状态码转换为可读字符串。

### 4. 线程安全

**本库 v2.0 非线程安全**。ArrayList / String / MutableString / LinkedList 的实例状态由调用方管理。多线程环境中需调用者自行加锁。

### 5. 语言标准

因该库使用了 `_Generic`，所以请注意使用 C11 或更高标准来编译此库。

---
