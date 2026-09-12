# JCBase Library

一个轻量级、模块化的 C 语言基础工具库，提供动态字符串（String）、动态数组（ArrayList）、哈希表（HashMap）等常用数据结构，以及配套的输入、类型转换、字符串处理函数。

**设计目标**：简洁、安全、易用，适合作为 C/C++ 项目的底层基础组件。

---

## 功能特性

**动态字符串（String）**

- 自动内存管理，支持长度缓存（cachedLen）提升性能
- 丰富的操作：拼接、截取、分割、查找、替换、修剪、大小写转换等
- 类型转换：安全转换为 int、long、long long、float、double
- 状态码机制，每个函数返回明确的状态值

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

| 函数 | 说明 |
|---|---|
| `string newString()` | 创建空字符串（初始容量 200） |
| `string newStringFrom(const char*)` | 从 C 字符串创建 |
| `void deleteString(string)` | 释放实例 |
| `int getStringStatus(const String*)` | 获取实例状态（强烈建议先检查） |
| `size_t getStringLength(const String*)` | 获取长度 |
| `const char* getStringContent(const String*)` | 获取只读 C 字符串 |
| `string appendString(...) / appendStringFrom(...)` | 拼接 |
| `string trim(...)` | 去除首尾空白 |
| `string* splitString(..., char delimiter, int* count)` | 按分隔符分割（返回数组） |
| `string replaceFirst(...) / replaceAll(...)` | 替换子串 |
| `int toInt(...) / toLong(...) / toFloat(...)` 等 | 类型转换（安全，检查溢出） |
| `string toLowerCase(...) / toUpperCase(...)` | 转换大小写 |
| `string readLine()` | 从 stdin 读取一行（返回 String） |

### ArrayList 模块

| 函数 | 说明 |
|---|---|
| `ArrayList* alCreate(size_t typeSize)` | 创建存储指定类型元素的数组 |
| `#define newArrayList(pointer)` | 类型安全的构造宏，自动推导 `typeSize` |
| `void alDestroy(ArrayList*)` | 销毁实例 |
| `int alGetStatus(const ArrayList*)` | 获取状态（强烈建议先检查） |
| `size_t alGetCount(...)` | 元素个数 |
| `size_t alGetMaxCount(...)` | 当前容量 |
| `int alSetMaxCount(...)` | 手动调整容量（需 >= 元素数） |
| `int alShrinkToFit(...)` | 缩容至恰好容纳元素 |
| `int alPushBack(...) / alPushFront(...)` | 尾部/头部插入 |
| `int alInsertAt(...)` | 指定位置插入 |
| `int alPopBack(...) / alPopFront(...)` | 尾部/头部弹出 |
| `int alRemoveAt(...)` | 删除指定位置 |
| `int alGetAt(...) / alSetAt(...)` | 获取/设置元素 |
| `int alClear(...)` | 清空（不释放内存，仅置 count=0） |

### HashMap 模块

| 函数 / 宏 | 说明 |
|---|---|
| `#define hmCreate(keyPointer, valuePointer, customHash, customCompare)` | **唯一对外构造入口**，根据 keyPointer 类型自动选 hash/compare |
| `void hmDestroy(hashMap* handle)` | 销毁实例，同时把调用方的句柄置 0 |
| `int hmPut(hashMap, const void* key, const void* value)` | 插入或覆盖键值对 |
| `int hmGet(hashMap, const void* key, void* outValue)` | 查找，把 value 拷贝到 `outValue` |
| `int hmRemove(hashMap, const void* key)` | 删除键值对 |
| `int hmContains(hashMap, const void* key)` | 判断 key 是否存在 |
| `int hmClear(hashMap)` | 清空所有键值对 |
| `size_t hmGetCount(hashMap)` | 键值对数量 |
| `size_t hmGetBucketCount(hashMap)` | 桶数量 |
| `int hmGetStatus(hashMap)` | 获取实例状态 |
| `int hmGetLastCreateStatus()` | 获取最近一次 `hmCreate` 的结果 |
| `hashMapStatusReport hmGetStatusReport(hashMap)` | 一次性返回句柄有效、创建状态、实例状态 |
| `bool hmHasIdentifier(hashMap)` | 检查句柄是否被赋值过（`identifier != 0`） |

**HashMap 内部构造器**：`hmNew(keySize, valueSize, hash, compare)` 由 `hmCreate` 宏展开调用，不要直接调用。它不做类型检查，直接接收函数指针，误用会导致运行时崩溃。

### 辅助函数

- `const char* stringStatusToArray(int status)`
- `const char* alStatusToArray(int status)`
- `const char* hmStatusToArray(int status)`

---

## 使用示例

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

**本库 v1.3 非线程安全**。HashMap 的注册表是全局的，ArrayList / String 的实例状态由调用方管理。多线程环境中需调用者自行加锁。

**后续版本计划**：为 HashMap 提供线程安全方案，为其他模块统一引入注册表机制解决别名问题，届时会一并考虑并发安全。

---

## 后续更新计划

- **AL**：解决别名问题、线程安全
- **HM**：每实例独立创建状态（替代当前全局 `lastCreateStatus`）、线程安全
- **LL**：写出框架、解决别名问题
- **SB**：完成实现、线程安全
- **String**：改为真正不可变、解决别名问题、线程安全
- **Scanner**：从老库迁移原生数据类型读取函数
- **FO**：文件操作封装
- **SQL 扩展库**：SQLite 封装、MySQL 封装
- **模板版本**：AL / LL / HM 的宏模板，最大化类型安全