# JCBase Library

一个轻量级、模块化的 C 语言基础工具库，提供不可变字符串（String）、可变字符串（StringBuilder）、动态数组（ArrayList）、哈希表（HashMap）等常用数据结构，以及String配套的输入（Scanner中的nextLine）、类型转换、字符串处理函数。

**设计目标**：简洁、安全、易用，适合作为 C/C++ 项目的底层基础组件。

---

## 功能特性

**不可变字符串（String）**

- 半自动内存管理，使用delete~~函数进行内存释放，带有长度结构体成员以提升性能
- 丰富的操作：拼接、截取、分割、查找、替换、修剪、大小写转换等
- 类型转换：安全转换为 int、long、long long、float、double
- 状态码机制，每个函数返回明确的状态值
- 注意：该字符串不可变，所以最好不要频繁进行拼接之类的操作

**可变字符串（StringBuilder） **

- 特性同String
- 注意：该字符串可变，所以拼接等操作可以使用该字符串

**动态数组（ArrayList）**

- 存储任意类型（通过 typeSize 指定）
- 支持增删改查、插入、弹出、清空、缩容（shrinkToFit）
- 动态扩容采用倍增策略（均摊 O(1) 插入）

**哈希表（HashMap）**

- 泛型键值对存储（keySize / valueSize 指定）
- 拉链法解决冲突，支持扩缩容
- 支持自定义哈希函数和比较函数，`char*` 键按内容比较
- **句柄机制**：对外暴露 `uint64_t identifier` 而非裸指针，从设计上解决别名、double free、use-after-free 问题
- 提供 `hmCreate` 宏做类型安全的构造入口

**输入辅助（Scanner）**

- 提供 `nextLine()` 从标准输入读取一行，返回 String 实例

**统一错误处理**

- 所有函数通过返回值报告状态（成功/失败/非法参数等）
- 每个模块有独立的状态枚举，状态码定义在各自的 `XxxSrc.h` 里
- 提供状态码转字符串函数，便于调试

---

## 快速开始

### 1. 获取源码

从 GitHub 克隆或下载本仓库。

### 2. 构建动态库（Visual Studio 示例）

- 使用 Visual Studio 创建 C++ DLL 项目（或空项目）
- 将所有 `.c` 和 `.h` 文件添加到项目中
- 关闭预编译头（项目属性 → C/C++ → 预编译头 → 选择“不使用预编译头”）
- 在 `wheel_ex.h` 中，`JCBASE_EXPORTS` 宏已在 DLL 项目中自动定义，确保导出符号
- 编译为 Release 模式，生成 `.dll` 和 `.lib` 文件

### 3. 在你的项目中使用

- 包含头文件：`#include "String.h"`、`#include "ArrayList.h"` 或 `#include "HashMap.h"`（根据需求）
- 链接生成的 `.lib` 文件，运行时确保 `.dll` 在搜索路径中
- 所有 API 均以 `wheels` 修饰符导出（Windows 专用，其他平台可忽略）

---

## 核心 API 概览

### String 模块

| 返回类型 | 函数 / 宏 | 说明 |
|---|---|---|
| `string` | `newString()` | 创建空字符串 |
| `string` | `newStringFrom(const char* const content)` | 从 C 字符串创建 |
| `void` | `deleteString(string str)` | 释放实例，可传 `NULL` |
| `int` | `getStringStatus(const String* const str)` | 获取实例状态 |
| `size_t` | `getStringLength(const String* const str)` | 获取长度 |
| `char*` | `getStringContent(const String* const str)` | 返回内容副本，需释放 |
| `const char*` | `stringStatusToCharArray(const int status)` | 状态码转描述 |
| `string` | `appendString(const String* const str1, const String* const str2)` | 拼接两个 String |
| `string` | `appendStringFrom(const String* const str, const char* const content)` | 拼接 String 与 C 字符串 |
| `string` | `reverseString(const String* const str)` | 反转字符串 |
| `int` | `splitString(const String* const str, char delimiter, string** outArray, size_t* count)` | 按分隔符分割 |
| `string` | `trim(const String* const str)` | 去除首尾空白 |
| `string` | `clone(const String* const str)` | 克隆 |
| `int` | `charAt(const String* const str, size_t index, char* result)` | 获取指定位置字符 |
| `int` | `indexOfFront(const String* const str, char ch, size_t* result)` | 从前往后查找字符 |
| `int` | `indexOfBack(const String* const str, char ch, size_t* result)` | 从后往前查找字符 |
| `string` | `subStringInLengthFront(const String* const str, size_t start, size_t length)` | 从 start 向后取 length |
| `string` | `subStringInLengthBack(const String* const str, size_t end, size_t length)` | 以 end 为终点向前取 length |
| `string` | `subStringInRange(const String* const str, size_t start, size_t end)` | 截取 `[start, end)` |
| `string` | `subStringInLengthFrontFrom(const char* const ch, size_t start, size_t length)` | 对 C 字符串从 start 向后取 length |
| `string` | `subStringInLengthBackFrom(const char* const ch, size_t end, size_t length)` | 对 C 字符串以 end 为终点向前取 length |
| `string` | `subStringInRangeFrom(const char* const ch, size_t start, size_t end)` | 对 C 字符串截取 `[start, end)` |
| `string` | `replaceFirst(const String* const str, const char* const target, const String* const replacement)` | 替换首次匹配 |
| `string` | `replaceAll(const String* const str, const char* const target, const String* const replacement)` | 替换全部匹配 |
| `string` | `replaceFirstFrom(const String* const str, const char* const target, const char* const replacement)` | 替换首次匹配，替换内容为 C 字符串 |
| `string` | `replaceAllFrom(const String* const str, const char* const target, const char* const replacement)` | 替换全部匹配，替换内容为 C 字符串 |
| `bool` | `startWith(const String* const str, const char* const prefix)` | 是否以 prefix 开头 |
| `bool` | `endWith(const String* const str, const char* const suffix)` | 是否以 suffix 结尾 |
| `bool` | `startWithString(const String* const str, const String* const prefix)` | 是否以 String 前缀开头 |
| `bool` | `endWithString(const String* const str, const String* const suffix)` | 是否以 String 后缀结尾 |
| `bool` | `isEmpty(const String* const str)` | 是否为空串 |
| `bool` | `isBlank(const String* const str)` | 是否全空白 |
| `bool` | `equals(const String* const str1, const String* const str2)` | 判断两个 String 是否相等 |
| `bool` | `equalsFrom(const String* const str, const char* const ch)` | 判断 String 与 C 字符串是否相等 |
| `bool` | `contains(const String* const str1, const String* const str2)` | 判断是否包含 |
| `bool` | `containsFrom(const String* const str, const char* const ch)` | 判断是否包含 C 字符串 |
| `char*` | `toArray(const String* const str)` | 转为 C 字符串副本，需释放 |
| `int` | `toInt(const String* const str, int* result)` | 转 int |
| `int` | `toLong(const String* const str, long* result)` | 转 long |
| `int` | `toLongLong(const String* const str, long long* result)` | 转 long long |
| `int` | `toFloat(const String* const str, float* result)` | 转 float |
| `int` | `toDouble(const String* const str, double* result)` | 转 double |
| `string` | `toLowerCase(const String* const str)` | 转小写，返回新实例 |
| `string` | `toUpperCase(const String* const str)` | 转大写，返回新实例 |
| `stringBuilder` | `stringToSb(const String* const str)` | String 转 StringBuilder |

### StringBuilder 模块

| 返回类型 | 函数 / 宏 | 说明 |
|---|---|---|
| `stringBuilder` | `newSb()` | 创建空 StringBuilder |
| `stringBuilder` | `newSbFrom(const char* const ch)` | 从 C 字符串创建 |
| `void` | `deleteSb(stringBuilder sb)` | 释放实例 |
| `int` | `getSbStatus(const StringBuilder* const sb)` | 获取状态 |
| `size_t` | `getSbLength(const StringBuilder* const sb)` | 获取长度 |
| `const char*` | `getSbBuffer(const StringBuilder* const sb)` | 获取内部 buffer，只读 |
| `const char*` | `getSbContent(const StringBuilder* const sb)` | 返回内容副本 |
| `int` | `setSbContent(StringBuilder* const str, const char* const content)` | 设置内容 |
| `int` | `changeAt(stringBuilder str, size_t index, char ch)` | 修改指定位置字符 |
| `const char*` | `sbStatusToCharArray(const int status)` | 状态码转描述 |
| `int` | `appendSb(StringBuilder* sb, const StringBuilder* const src)` | 追加 StringBuilder |
| `int` | `appendSbFrom(stringBuilder sb, const char* const ch)` | 追加 C 字符串 |
| `int` | `reverseSb(stringBuilder sb)` | 原地反转 |
| `int` | `splitSb(const StringBuilder* const str, char delimiter, stringBuilder** outArray, size_t* count)` | 分割 |
| `int` | `trimSb(stringBuilder const str)` | 原地去除首尾空白 |
| `stringBuilder` | `cloneSb(const StringBuilder* const str)` | 克隆 |
| `int` | `charAtSb(const StringBuilder* const str, size_t index, char* result)` | 获取指定位置字符 |
| `int` | `sbIndexOfFront(const StringBuilder* const str, char ch, size_t* result)` | 从前往后查找字符 |
| `int` | `sbIndexOfBack(const StringBuilder* const str, char ch, size_t* result)` | 从后往前查找字符 |
| `stringBuilder` | `subSbInLengthFront(const StringBuilder* const str, size_t start, size_t length)` | 从 start 向后取 length |
| `stringBuilder` | `subSbInLengthBack(const StringBuilder* const str, size_t end, size_t length)` | 以 end 为终点向前取 length |
| `stringBuilder` | `subSbInRange(const StringBuilder* const str, size_t start, size_t end)` | 截取 `[start, end)` |
| `stringBuilder` | `subSbInLengthFrontFrom(const char* const ch, size_t start, size_t length)` | 对 C 字符串从 start 向后取 length |
| `stringBuilder` | `subSbInLengthBackFrom(const char* const ch, size_t end, size_t length)` | 对 C 字符串以 end 为终点向前取 length |
| `stringBuilder` | `subSbInRangeFrom(const char* const ch, size_t start, size_t end)` | 对 C 字符串截取 `[start, end)` |
| `int` | `replaceSbFirst(StringBuilder* str, const char* const target, const StringBuilder* const replacement)` | 替换首次匹配 |
| `int` | `replaceSbFirstFrom(StringBuilder* str, const char* const target, const char* const replacement)` | 替换首次匹配，替换内容为 C 字符串 |
| `int` | `replaceSbAll(StringBuilder* str, const char* const target, const StringBuilder* const replacement)` | 替换全部匹配 |
| `int` | `replaceSbAllFrom(StringBuilder* str, const char* const target, const char* const replacement)` | 替换全部匹配，替换内容为 C 字符串 |
| `bool` | `sbStartWith(const StringBuilder* const str, const char* const prefix)` | 是否以 prefix 开头 |
| `bool` | `sbEndWith(const StringBuilder* const str, const char* const suffix)` | 是否以 suffix 结尾 |
| `bool` | `sbStartWithString(const StringBuilder* const str, const StringBuilder* const prefix)` | 是否以 StringBuilder 前缀开头 |
| `bool` | `sbEndWithString(const StringBuilder* const str, const StringBuilder* const suffix)` | 是否以 StringBuilder 后缀结尾 |
| `bool` | `sbIsEmpty(const StringBuilder* const str)` | 是否为空串 |
| `bool` | `sbIsBlank(const StringBuilder* const str)` | 是否全空白 |
| `bool` | `equalsSb(const StringBuilder* const str1, const StringBuilder* const str2)` | 判断两个 StringBuilder 是否相等 |
| `bool` | `equalsSbFrom(const StringBuilder* const str, const char* const ch)` | 判断与 C 字符串是否相等 |
| `bool` | `containsSb(const StringBuilder* const str1, const StringBuilder* const str2)` | 判断是否包含 |
| `bool` | `containsSbFrom(const StringBuilder* const str, const char* const ch)` | 判断是否包含 C 字符串 |
| `char*` | `sbToArray(const StringBuilder* const str)` | 转为 C 字符串副本，需释放 |
| `int` | `sbToInt(const StringBuilder* const str, int* result)` | 转 int |
| `int` | `sbToLong(const StringBuilder* const str, long* result)` | 转 long |
| `int` | `sbToLongLong(const StringBuilder* const str, long long* result)` | 转 long long |
| `int` | `sbToFloat(const StringBuilder* const str, float* result)` | 转 float |
| `int` | `sbToDouble(const StringBuilder* const str, double* result)` | 转 double |
| `int` | `sbToLowerCase(StringBuilder* str)` | 原地转小写 |
| `int` | `sbToUpperCase(StringBuilder* str)` | 原地转大写 |
| `string` | `sbToString(const StringBuilder* const str)` | StringBuilder 转 String |


### ArrayList 模块

| 返回类型 | 函数 / 宏 | 说明 |
|---|---|---|
| `ArrayList*` | `newArrayList(pointer)` | 类型安全构造宏，展开为 `alCreate(sizeof(*(pointer)))` |
| `ArrayList*` | `alCreate(size_t typeSize)` | 创建动态数组 |
| `void` | `alDestroy(ArrayList* list)` | 销毁实例 |
| `int` | `alGetStatus(const ArrayList* const list)` | 获取状态 |
| `size_t` | `alGetCount(const ArrayList* const list)` | 获取元素数量 |
| `size_t` | `alGetMaxCount(const ArrayList* const list)` | 获取当前容量 |
| `int` | `alSetMaxCount(ArrayList* const list, size_t count)` | 手动设置容量 |
| `int` | `alShrinkToFit(ArrayList* const list)` | 缩容到恰好容纳元素 |
| `int` | `alGetAt(const ArrayList* const list, size_t index, void* ele)` | 获取指定位置元素 |
| `int` | `alSetAt(ArrayList* const list, size_t index, const void* const content)` | 设置指定位置元素 |
| `int` | `alPushBack(ArrayList* const list, const void* const content)` | 尾部插入 |
| `int` | `alPushFront(ArrayList* const list, const void* const content)` | 头部插入 |
| `int` | `alInsertAt(ArrayList* const list, const void* const content, size_t index)` | 指定位置插入 |
| `int` | `alPopBack(ArrayList* const list)` | 尾部弹出 |
| `int` | `alPopFront(ArrayList* const list)` | 头部弹出 |
| `int` | `alRemoveAt(ArrayList* const list, size_t index)` | 删除指定位置 |
| `int` | `alClear(ArrayList* const list)` | 清空元素数量，不释放 buffer |
| `const char*` | `alStatusToArray(int status)` | 状态码转描述 |

### HashMap 模块

| 返回类型 | 函数 / 宏 | 说明 |
|---|---|---|
| `bool` | `hmHasIdentifier(hashMap handle)` | `static inline`，检查句柄是否非 0 |
| `hashMapStatusReport` | `hmGetStatusReport(hashMap handle)` | 返回句柄有效、创建状态、实例状态 |
| `hashMap` | `hmNew(size_t keySize, size_t valueSize, hashFunction hash, compareFunction compare)` | 内部构造器，不建议直接调用 |
| `hashMap` | `hmCreate(keyPointer, valuePointer, customHash, customCompare)` | 唯一对外构造宏 |
| `int` | `hmGetLastCreateStatus(void)` | 获取最近一次创建状态 |
| `void` | `hmDestroy(hashMap* handle)` | 销毁实例，并将句柄置 0 |
| `int` | `hmPut(hashMap handle, const void* key, const void* value)` | 插入或覆盖键值对 |
| `int` | `hmGet(hashMap handle, const void* key, void* outValue)` | 获取 value |
| `int` | `hmRemove(hashMap handle, const void* key)` | 删除键值对 |
| `int` | `hmContains(hashMap handle, const void* key)` | 判断 key 是否存在 |
| `int` | `hmClear(hashMap handle)` | 清空所有键值对 |
| `size_t` | `hmGetCount(hashMap handle)` | 获取键值对数量 |
| `size_t` | `hmGetBucketCount(hashMap handle)` | 获取桶数量 |
| `int` | `hmGetStatus(hashMap handle)` | 获取实例状态 |
| `const char*` | `hmStatusToArray(int status)` | 状态码转描述 |
| `size_t` | `hashCString(const void* key, size_t keySize)` | `char*` 键按内容哈希 |
| `int` | `compareCString(const void* keyA, const void* keyB, size_t keySize)` | `char*` 键按内容比较 |

### Scanner 模块

| 返回类型 | 函数 / 宏 | 说明 |
|---|---|---|
| `string` | `nextLine()` | 从 stdin 读取一行，返回 String |

### 类型与状态

| 类型 / 宏 | 头文件 | 说明 |
|---|---|---|
| `stringStatus` | `StringSrc.h` | `typedef stringCommonStatus stringStatus;` |
| `stringBuilderStatus` | `StringBuilderSrc.h` | `typedef stringCommonStatus stringBuilderStatus;` |
| `stringCommonStatus` | `StringComStatus.h` | String / StringBuilder 公共状态枚举 |
| `alStatus` | `ArrayListSrc.h` | ArrayList 状态枚举 |
| `hashMapStatus` | `HashMapSrc.h` | HashMap 状态枚举 |
| `hashFunction` | `HashMap.h` | 哈希函数指针类型 |
| `compareFunction` | `HashMap.h` | 比较函数指针类型 |
| `hashMap` | `HashMap.h` | HashMap 句柄结构体 |
| `hashMapStatusReport` | `HashMap.h` | HashMap 状态报告结构体 |
| `wheels` | `wheel_ex.h` | DLL 导出 / 导入宏 |
---

### 使用示例

### String 基本操作

```c
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
```

### ArrayList 操作

```c
#include "ArrayList.h"
#include <stdio.h>

int main() {
    int dummy = 0;
    ArrayList* list = newArrayList(&dummy);
    // 展开成 alCreate(sizeof(int))

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
```

### HashMap 操作

```c
#include "HashMap.h"
#include <stdio.h>

int main() {
    // key 是 int，value 是 char*
    int key = 42;
    char* value = "hello";

    hashMap map = hmCreate(&key, &value, NULL, NULL);
    // 展开成 hmNew(sizeof(int), sizeof(char*), NULL, NULL)

    if (!hmHasIdentifier(map)) {
        fprintf(stderr, "HashMap creation failed: %s\n",
                hmStatusToArray(hmGetLastCreateStatus()));
        return 1;
    }

    if (hmGetStatus(map) != AVAILABLEHM) {
        fprintf(stderr, "Instance status: %s\n",
                hmStatusToArray(hmGetStatus(map)));
        hmDestroy(&map);
        return 1;
    }

    hmPut(map, &key, &value);

    char* out = NULL;
    if (hmGet(map, &key, &out) == SUCCESSFULOP) {
        printf("Value: %s\n", out);
    }

    hmDestroy(&map);
    return 0;
}
```

**HashMap 用 char\* 作 key（按内容比较）**：

```c
char* key = "大王";
char* value = "李**";

hashMap map = hmCreate(&key, &value, NULL, NULL);
// 展开成 hmNew(sizeof(char*), sizeof(char*), hashCString, compareCString)
// 两个内容相同、地址不同的 char* 会被当作同一个 key

hmPut(map, &key, &value);

char* searchKey = "大王";   // 不同地址
char* out = NULL;
hmGet(map, &searchKey, &out);   // 找得到
```

**HashMap 别名安全示例**：

```c
int key = 1;
char* value = "a";

hashMap map1 = hmCreate(&key, &value, NULL, NULL);
hashMap map2 = map1;   // 复制句柄

hmDestroy(&map1);       // 释放，map1 置 0

hmPut(map2, &key, &value);   // 查注册表失败 → 返回 NULLHM，不会崩
hmDestroy(&map2);            // 安全返回，不会 double free
```

### 读取一行输入

```c
#include "Scanner.h"

string line = nextLine();   // 等价于 readLine()
if (line && getStringStatus(line) == AVAILABLE) {
    printf("You entered: %s\n", getStringContent(line));
}
deleteString(line);
```

---

## ⚠️ 重要提醒

### 1. 始终检查实例状态

在调用任何操作函数之前，务必使用 `alGetStatus` / `getStringStatus` / `hmGetStatus` 检查实例是否为可用状态。

非法状态（如 `NULLAL`、`INVALIDALTYPESIZE`、`NULLBUFFER`、`INVALIDHMSIZE`）可能导致未定义行为。

```c
if (alGetStatus(list) != AVAILABLEAL) {
    // 处理错误或退出
}
```
最好不要将带有指针的结构体类型传给容器，容器使用浅拷贝，如果传入了，请注意是否该释放某个资源

### 2. 内存管理

使用 `alDestroy` / `deleteString` / `hmDestroy` 释放实例，库不会自动回收。

对于 ArrayList，元素数据由库管理，但元素本身的内存（如动态分配的指针）仍需调用者自行管理。

`splitString` 返回的数组和其中的 String 对象都需要分别释放（先释放每个 String，再释放数组本身）。

### 3. 错误码含义

每个模块的状态码定义在各自的 `XxxSrc.h` 里：

- `StringSrc.h`：`stringStatus`
- `ArrayListSrc.h`：`alStatus`
- `HashMapSrc.h`：`hashMapStatus`

建议使用 `stringStatusToArray` / `alStatusToArray` / `hmStatusToArray` 将状态码转换为可读字符串，便于调试。

### 4. HashMap 的句柄机制

HashMap 对外暴露的不是裸指针，而是一个只含 `uint64_t identifier` 的值句柄。这个设计解决了几个常见问题：

- **别名**：`map2 = map1` 只是复制 identifier，释放 `map1` 后 `map2` 自动失效，不会 double free 或 use-after-free。
- **ABA**：identifier 全局递增、不复用，旧 identifier 永远查不到。
- **无效句柄**：所有操作先查内部注册表，查不到就返回 `NULLHM`。

**关于 `hmHasIdentifier`**：它只检查句柄是否被赋值过（`identifier != 0`），**不保证**实例存在，也不保证实例可用。要确认实例状态，用 `hmGetStatus` 或 `hmGetStatusReport`。

### 5. 扩容策略

ArrayList 采用倍增扩容（容量不足时翻倍），保证大量插入的均摊性能 O(1)。

HashMap 采用负载因子控制：超过 0.75 扩容翻倍，低于 0.25 缩容减半，最小桶数 16，避免抖动。

若对内存占用敏感，ArrayList 可使用 `alShrinkToFit` 在批量插入后收缩内存。

### 6. 线程安全

**本库 v1.5 非线程安全**。HashMap 的注册表是全局的，ArrayList / String 的实例状态由调用方管理。多线程环境中需调用者自行加锁。

**后续版本计划**：为 HashMap 提供线程安全方案，为其他模块统一引入注册表机制解决别名问题，届时会一并考虑并发安全。

### 7. 语言标准

因该库使用了_Generic，所以请注意使用C11或更高标准来编译此库

---

## 后续更新计划

- **AL**：解决别名问题、线程安全
- **HM**：每实例独立创建状态（替代当前全局 `lastCreateStatus`）、线程安全
- **LL**：写出框架、解决别名问题
- **SB**：线程安全
- **String**：改为真正不可变、解决别名问题、线程安全
- **Scanner**：整型等原生类型的标准输入读取函数
- **FO**：文件操作封装
- **SQL 扩展库**：SQLite 封装、MySQL 封装
- **模板版本**：AL / LL / HM 的宏模板，最大化类型安全