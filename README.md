JCBase Library
一个轻量级、模块化的 C 语言基础工具库，提供动态字符串（String）和动态数组（ArrayList）等常用数据结构，以及配套的输入、类型转换、字符串处理函数。
设计目标：简洁、安全、易用，适合作为 C/C++ 项目的底层基础组件。

功能特性
动态字符串（String）

自动内存管理，支持长度缓存（cachedLen）提升性能

丰富的操作：拼接、截取、分割、查找、替换、修剪、大小写转换等

类型转换：安全转换为 int、long、long long、float、double

状态码机制，每个函数返回明确的状态值

动态数组（ArrayList）

存储任意类型（通过 typeSize 指定）

支持增删改查、插入、弹出、清空、缩容（shrinkToFit）

动态扩容采用倍增策略（均摊 O(1) 插入）

输入辅助（Scanner）

提供 nextLine() 从标准输入读取一行，返回 String 实例

统一错误处理

所有函数通过返回值报告状态（成功/失败/非法参数等）

提供状态码转字符串函数，便于调试

快速开始
1. 获取源码
从 GitHub 克隆或下载本仓库。

2. 构建动态库（Visual Studio 示例）
使用 Visual Studio 创建 C++ DLL 项目（或空项目）

将所有 .c 和 .h 文件添加到项目中

关闭预编译头（项目属性 → C/C++ → 预编译头 → 选择“不使用预编译头”）

在 wheel_ex.h 中，JCBASE_EXPORTS 宏已在 DLL 项目中自动定义，确保导出符号

编译为 Release 模式，生成 .dll 和 .lib 文件

3. 在你的项目中使用
包含头文件：#include "String.h" 或 #include "ArrayList.h"（根据需求）

链接生成的 .lib 文件，运行时确保 .dll 在搜索路径中

所有 API 均以 wheels 修饰符导出（Windows 专用，其他平台可忽略）

核心 API 概览
String 模块
函数	说明
string newString()	创建空字符串（初始容量 200）
string newStringFrom(const char*)	从 C 字符串创建
void deleteString(string)	释放实例
int getStringStatus(const String*)	获取实例状态（强烈建议先检查）
size_t getStringLength(const String*)	获取长度
const char* getStringContent(const String*)	获取只读 C 字符串
string appendString(...) / appendStringFrom(...)	拼接
string trim(...)	去除首尾空白
string* splitString(..., char delimiter, int* count)	按分隔符分割（返回数组）
string replaceFirst(...) / replaceAll(...)	替换子串
int toInt(...) / toLong(...) / toFloat(...) 等	类型转换（安全，检查溢出）
string toLowerCase(...) / toUpperCase(...)	转换大小写
string readLine()	从 stdin 读取一行（返回 String）
ArrayList 模块
函数	说明
ArrayList* alCreate(size_t typeSize)	创建存储指定类型元素的数组
void alDestroy(ArrayList*)	销毁实例
int alGetStatus(const ArrayList*)	获取状态（强烈建议先检查）
size_t alGetCount(...)	元素个数
size_t alGetMaxCount(...)	当前容量
int alSetMaxCount(...)	手动调整容量（需 >= 元素数）
int alShrinkToFit(...)	缩容至恰好容纳元素
int alPushBack(...) / alPushFront(...)	尾部/头部插入
int alInsertAt(...)	指定位置插入
int alPopBack(...) / alPopFront(...)	尾部/头部弹出
int alRemoveAt(...)	删除指定位置
int alGetAt(...) / alSetAt(...)	获取/设置元素
int alClear(...)	清空（不释放内存，仅置 count=0）
辅助函数
const char* stringStatusToArray(int status)

const char* alStatusToArray(int status)

使用示例
String 基本操作
c
#include "String.h"
#include <stdio.h>

int main() {
    string s = newStringFrom("Hello");
    if (getStringStatus(s) != AVAILABLE) {
        fprintf(stderr, "String creation failed\n");
        return 1;
    }

    string t = appendStringFrom(s, " World!");
    if (t) {
        printf("%s\n", getStringContent(t)); // Hello World!
        deleteString(t);
    }

    int num;
    if (toInt(s, &num) == SUCCEEDTOINT) {
        printf("Parsed number: %d\n", num);
    }

    deleteString(s);
    return 0;
}
ArrayList 操作
c
#include "ArrayList.h"
#include <stdio.h>

int main() {
    ArrayList* list = alCreate(sizeof(int));
    if (alGetStatus(list) != AVAILABLEAL) {
        fprintf(stderr, "ArrayList creation failed\n");
        return 1;
    }

    int val = 42;
    alPushBack(list, &val);
    val = 100;
    alPushBack(list, &val);

    int out;
    alGetAt(list, 0, &out);
    printf("First element: %d\n", out);

    alDestroy(list);
    return 0;
}
读取一行输入
c
#include "Scanner.h"

string line = nextLine(); // 等价于 readLine()
if (line && getStringStatus(line) == AVAILABLE) {
    printf("You entered: %s\n", getStringContent(line));
}
deleteString(line);
⚠️ 重要提醒
1. 始终检查实例状态
在调用任何操作函数之前，务必使用 alGetStatus 或 getStringStatus 检查实例是否为 AVAILABLEAL / AVAILABLE。
非法状态（如 NULLAL、INVALIDALTYPESIZE、NULLBUFFER）可能导致未定义行为。

c
if (alGetStatus(list) != AVAILABLEAL) {
    // 处理错误或退出
}
2. 内存管理
使用 alDestroy / deleteString 释放实例，库不会自动回收。

对于 ArrayList，元素数据由库管理，但元素本身的内存（如动态分配的指针）仍需调用者自行管理。

splitString 返回的数组和其中的 String 对象都需要分别释放（先释放每个 String，再释放数组本身）。

3. 错误码含义
所有函数返回的状态码定义在 resources.h（后续版本会拆分为独立头文件）。
建议使用 stringStatusToArray / alStatusToArray 将状态码转换为可读字符串，便于调试。

4. 扩容策略
ArrayList 采用倍增扩容（容量不足时翻倍），保证大量插入的均摊性能 O(1)。

若对内存占用敏感，可使用 alShrinkToFit 在批量插入后收缩内存。

5. 线程安全
本库非线程安全，多线程环境中需调用者自行加锁。