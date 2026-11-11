# JCBase Library

一个轻量级、模块化的 C 语言基础工具库，提供不可变字符串（String）、可变字符串（StringBuilder）、动态数组（ArrayList）、链表（LinkedList）、哈希表（HashMap）等常用数据结构，以及String配套的输入（Scanner中的nextLine）、类型转换、字符串处理函数。

**设计目标**：简洁、安全、易用，适合作为 C/C++ 项目的底层基础组件。
[点击查看更新日志](UPDATE.md)<br>
[点击查看ArrayList.md](ArrayList.md)[点击查看ArrayList_EN.md](ArrayList_EN.md)<br>
[点击查看String.md](String.md)[点击查看String_EN.md](String_EN.md)<br>
[点击查看MutableString.md](MutableString.md)[点击查看MutableString_EN.md](MutableString_EN.md)
[点击查看ArrayList.md](ArrayList.md)[点击查看ArrayList_EN.md](ArrayList_EN.md)

---

## 功能特性
**链表（LinkedList）**

- 带哨兵的循环双链表，存储任意类型（通过 typeSize 指定）
- 节点结构与数据分离，支持 O(1) 摘链、O(1) 在任意已知节点前后插入
- 双向遍历、节点移动、链表拼接均为 O(1)（仅改指针，不移动数据）
- 支持值操作（pushBack / popFront / insertAt 等）与节点操作（insertBefore / insertAfter / removeNode / moveBefore 等）两层接口
- 范围清理（clearInRange）按区间批量删除节点

**哈希表（HashMap）**

- 泛型键值对存储（keySize / valueSize 指定）
- 拉链法解决冲突，支持扩缩容
- 支持自定义哈希函数和比较函数，`char*` 键按内容比较
- **句柄机制**：对外暴露 句柄结构体 而非裸指针，从设计上尽量解决别名、double free、use-after-free 问题
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
- 关闭预编译头（项目属性 → C/C++ → 预编译头 → 选择"不使用预编译头"）
- 在 `wheel_ex.h` 中，`JCBASE_EXPORTS` 宏已在 DLL 项目中自动定义，确保导出符号
- 编译为 Release 模式，生成 `.dll` 和 `.lib` 文件

### 3. 在你的项目中使用

- 包含头文件：`#include "String.h"`、`#include "ArrayList.h"` 或 `#include "HashMap.h"`（根据需求）
- 链接生成的 `.lib` 文件，运行时确保 `.dll` 在搜索路径中
- 所有 API 均以 `wheels` 修饰符导出（Windows 专用，其他平台可忽略）

---

## 核心 API 概览
### LinkedList 模块

| 返回类型 | 函数 / 宏 | 说明 |
|---|---|---|
| `linkedList` | `newLinkedList(pointer)` | 类型安全构造宏，展开为 `llCreate(sizeof(*(pointer)))` |
| `linkedList` | `llCreate(size_t typeSize)` | 创建链表 |
| `int` | `llDestroy(linkedList list)` | 销毁实例，释放所有数据节点和 header |
| `int` | `llClear(linkedList list)` | 清空所有数据节点，保留实例 |
| `int` | `llGetStatus(const LinkedList* const list)` | 获取状态 |
| `size_t` | `llGetCount(const LinkedList* const list)` | 获取元素数量 |
| `const char*` | `llStatusToCharArray(int status)` | 状态码转描述 |
| `int` | `llPushBack(linkedList list, const void* content)` | 尾部插入值 |
| `int` | `llPushFront(linkedList list, const void* content)` | 头部插入值 |
| `int` | `llInsertAt(linkedList list, const void* content, size_t index)` | 指定位置插入值 |
| `int` | `llPopBack(linkedList list)` | 尾部弹出 |
| `int` | `llPopFront(linkedList list)` | 头部弹出 |
| `int` | `llRemoveAt(linkedList list, size_t index)` | 删除指定位置 |
| `int` | `llGetContentAt(const LinkedList* list, size_t index, void* out)` | 按索引取内容 |
| `int` | `llSetContentAt(linkedList list, size_t index, const void* content)` | 按索引设内容 |
| `int` | `llGetFrontNode(const linkedList list, linkedListNode* out)` | 获取首个数据节点 |
| `int` | `llGetBackNode(const linkedList list, linkedListNode* out)` | 获取最后一个数据节点 |
| `int` | `llGetHeader(const linkedList list, linkedListNode* out)` | 获取头结点（哨兵） |
| `int` | `llGetNodeAt(const linkedList list, size_t index, linkedListNode* out)` | 获取索引处节点 |
| `int` | `llNext(linkedListNode* node)` | 游标前移 |
| `int` | `llPrev(linkedListNode* node)` | 游标后移 |
| `int` | `llGetFromNode(const linkedList list, const LinkedListNode* node, void* out)` | 按节点取内容 |
| `int` | `llSetOfNode(linkedList list, linkedListNode node, const void* content)` | 按节点设内容 |
| `int` | `llInsertBefore(linkedList list, linkedListNode node, const void* content)` | 在指定节点前插入 |
| `int` | `llInsertAfter(linkedList list, linkedListNode node, const void* content)` | 在指定节点后插入 |
| `int` | `llRemoveNode(linkedList list, linkedListNode node)` | 删除指定节点 |
| `int` | `llMoveBefore(linkedList list, linkedListNode node, linkedListNode pos)` | 将节点移到 pos 前 |
| `int` | `llMoveAfter(linkedList list, linkedListNode node, linkedListNode pos)` | 将节点移到 pos 后 |
| `int` | `llMoveToFront(linkedList list, linkedListNode node)` | 将节点移到链表头 |
| `int` | `llMoveToBack(linkedList list, linkedListNode node)` | 将节点移到链表尾 |
| `int` | `llSpliceBack(linkedList front, linkedList back)` | 将 back 拼接到 front 尾部，back 被消耗 |
| `int` | `llSpliceBefore(linkedList hold, linkedListNode node, linkedList material)` | 将 material 拼到 node 前 |
| `int` | `llSpliceAfter(linkedList hold, linkedListNode node, linkedList material)` | 将 material 拼到 node 后 |
| `int` | `llSpliceAt(linkedList hold, size_t index, linkedList material)` | 将 material 拼到指定索引前 |
| `int` | `llClearInRange(linkedList list, size_t start, size_t end)` | 清空 `[start, end)` 区间 |

### HashMap 模块

| 返回类型 | 函数 / 宏 | 说明 |
|---|---|---|
| `bool` | `hmIsValid(hashMap handle)` | `static inline`，检查句柄是否非 0 |
| `HashMapRegistry*` | `hmRegistryCreate(void)` | 创建注册表 |
| `void` | `hmRegistryDestroy(HashMapRegistry* reg)` | 销毁注册表 |
| `hashMapStatusReport` | `hmGetStatusReport(hashMap handle)` | 返回句柄有效、创建状态、实例状态 |
| `hashMap` | `hmNew(size_t keySize, size_t valueSize, hashFunction hash, compareFunction compare)` | 内部构造器，不建议直接调用 |
| `hashMap` | `hmCreate(reg, keyPointer, valuePointer, customHash, customCompare)` | 唯一对外构造宏 |
| `int` | `hmGetLastCreateStatus(HashMapRegistry* reg)` | 获取最近一次创建状态 |
| `void` | `hmDestroy(hashMap* handle)` | 销毁实例，并将句柄置 0 |
| `int` | `hmPut(hashMap handle, const void* key, const void* value)` | 插入或覆盖键值对 |
| `int` | `hmGet(hashMap handle, const void* key, void* outValue)` | 获取 value |
| `int` | `hmRemove(hashMap handle, const void* key)` | 删除键值对 |
| `int` | `hmContains(hashMap handle, const void* key)` | 判断 key 是否存在 |
| `int` | `hmClear(hashMap handle)` | 清空所有键值对 |
| `size_t` | `hmGetCount(hashMap handle)` | 获取键值对数量 |
| `size_t` | `hmGetBucketCount(hashMap handle)` | 获取桶数量 |
| `int` | `hmGetStatus(hashMap handle)` | 获取实例状态 |
| `const char*` | `hmStatusToCharArray(int status)` | 状态码转描述 |
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
| `llStatus` | `LinkedListSrc.h` | LinkedList 状态枚举 |
| `hashMapStatus` | `HashMapSrc.h` | HashMap 状态枚举 |
| `hashFunction` | `HashMap.h` | 哈希函数指针类型 |
| `compareFunction` | `HashMap.h` | 比较函数指针类型 |
| `hashMap` | `HashMap.h` | HashMap 句柄结构体 |
| `hashMapStatusReport` | `HashMap.h` | HashMap 状态报告结构体 |
| `wheels` | `wheel_ex.h` | DLL 导出 / 导入宏 |

---

## 使用示例
### LinkedList 操作

```c
#include "JC_LinkedList.h"
#include <stdio.h>

int main() {
    int dummy = 0;
    linkedList list = newLinkedList(&dummy);
    // 展开成 llCreate(sizeof(int))

    if (llGetStatus(list) != LL_AVAILABLE) {
        fprintf(stderr, "LinkedList creation failed\n");
        return 1;
    }

    /* 按值操作 */
    int val = 42;
    llPushBack(list, &val);      // 42
    val = 100;
    llPushFront(list, &val);     // 100, 42
    val = 7;
    llInsertAt(list, &val, 1);   // 100, 7, 42

    int out;
    llGetContentAt(list, 1, &out);
    printf("Element at index 1: %d\n", out);   // 7

    /* 按节点操作 */
    linkedListNode node;
    llGetNodeAt(list, 2, &node);   // 指向 42
    val = 999;
    llInsertBefore(list, node, &val);  // 100, 7, 999, 42
    llMoveToFront(list, node);         // 42, 100, 7, 999
    llRemoveNode(list, node);          // 100, 7, 999

    /* 遍历 */
    linkedListNode h, p;
    llGetHeader(list, &h);
    llGetFrontNode(list, &p);
    while (p != h) {
        llGetFromNode(list, p, &out);
        printf("%d ", out);
        llNext(&p);
    }
    printf("\n");

    llDestroy(list);
    return 0;
}
```

### 链表拼接

```c
#include "JC_LinkedList.h"
#include <stdio.h>

int main() {
    linkedList a = llCreate(sizeof(int));
    linkedList b = llCreate(sizeof(int));

    int v;
    v = 1; llPushBack(a, &v);
    v = 2; llPushBack(a, &v);
    v = 3; llPushBack(b, &v);
    v = 4; llPushBack(b, &v);

    /* 把 b 拼到 a 末尾，O(1)，b 被消耗 */
    llSpliceBack(a, &b);
    printf("count = %zu\n", llGetCount(a));   // 4

    /* b 内部资源已被释放，只能 llDestroy 或 free */
    llDestroy(b);

    llDestroy(a);
    return 0;
}
```

### HashMap 操作

```c
#include "JC_HashMap.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
    /* -------------------------------------------------- */
    /* 1. 创建注册表                                      */
    /* -------------------------------------------------- */
    HashMapRegistry* reg = hmRegistryCreate();
    if (!reg) {
        fprintf(stderr, "failed to create registry\n");
        return 1;
    }

    /* -------------------------------------------------- */
    /* 2. 创建一个 int -> int 的 map                      */
    /*    hmCreate 第一个参数是 registry                  */
    /*    key 是 int*，value 是 int*，所以传 NULL, NULL   */
    /*    表示用默认的 fnv1a + memcmp                     */
    /* -------------------------------------------------- */
    int  key   = 0;   /* 仅用于取类型 */
    int  value = 0;
    hashMap scores = hmCreate(reg, &key, &value, NULL, NULL);

    if (!hmIsValid(scores)) {
        fprintf(stderr, "failed to create map: %s\n",
                hmStatusToCharArray(hmGetLastCreateStatus(reg)));
        hmRegistryDestroy(reg);
        return 1;
    }

    /* -------------------------------------------------- */
    /* 3. 插入                                            */
    /*    hmPut 按值拷贝 key 和 value                     */
    /* -------------------------------------------------- */
    int k, v;

    k = 1; v = 100;  hmPut(scores, &k, &v);
    k = 2; v = 200;  hmPut(scores, &k, &v);
    k = 3; v = 300;  hmPut(scores, &k, &v);

    printf("count = %zu\n", hmGetCount(scores));      /* 3 */
    printf("buckets = %zu\n", hmGetBucketCount(scores));

    /* -------------------------------------------------- */
    /* 4. 查询                                            */
    /*    hmGet 把 value 拷贝到 out                       */
    /* -------------------------------------------------- */
    k = 2;
    int out = 0;
    if (hmGet(scores, &k, &out) == HM_SUCCESSFULOP) {
        printf("key 2 -> %d\n", out);                 /* 200 */
    }

    /* -------------------------------------------------- */
    /* 5. 判断 key 是否存在                               */
    /* -------------------------------------------------- */
    k = 3;
    if (hmContains(scores, &k) == HM_SUCCESSFULOP) {
        printf("key 3 exists\n");
    }

    /* -------------------------------------------------- */
    /* 6. 删除单个 key                                    */
    /* -------------------------------------------------- */
    k = 1;
    hmRemove(scores, &k);
    printf("after remove, count = %zu\n", hmGetCount(scores));  /* 2 */

    /* -------------------------------------------------- */
    /* 7. 清空 map（保留容量）                            */
    /* -------------------------------------------------- */
    hmClear(scores);
    printf("after clear, count = %zu\n", hmGetCount(scores));   /* 0 */

    /* -------------------------------------------------- */
    /* 8. 销毁句柄                                        */
    /*    hmDestroy 接受 hashMap*，销毁后句柄清零         */
    /* -------------------------------------------------- */
    hmDestroy(&scores);
    printf("scores valid after destroy: %d\n", hmIsValid(scores)); /* 0 */

    /* -------------------------------------------------- */
    /* 9. 字符串 key 的 map                               */
    /*    key 类型是 char*，_Generic 会选 hashCString 和  */
    /*    compareCString，按字符串内容比较                */
    /* -------------------------------------------------- */
    char* nameKey   = NULL;
    int   nameValue = 0;
    hashMap byName = hmCreate(reg, &nameKey, &nameValue, NULL, NULL);

    char* n;
    int   nv;

    n = "alice"; nv = 20; hmPut(byName, &n, &nv);
    n = "bob";   nv = 25; hmPut(byName, &n, &nv);
    n = "carol"; nv = 30; hmPut(byName, &n, &nv);

    /* 查询：用另一个内容相同的指针也能命中 */
    char* search = "alice";
    int   age    = 0;
    if (hmGet(byName, &search, &age) == HM_SUCCESSFULOP) {
        printf("alice is %d\n", age);                 /* 20 */
    }

    hmDestroy(&byName);

    /* -------------------------------------------------- */
    /* 10. 销毁注册表                                     */
    /*     内部会回收所有残留的 map（即便上面忘了         */
    /*     hmDestroy 某个句柄也不会泄漏 map 本身）        */
    /* -------------------------------------------------- */
    hmRegistryDestroy(reg);

    return 0;
}
```

**HashMap 用 char\* 作 key（按内容比较）**：

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

在调用任何操作函数之前，务必使用 `alGetStatus` / `llGetStatus` / `getStringStatus` / `hmGetStatus` 检查实例是否为可用状态。

非法状态（如 `NULLAL`、`INVALIDALTYPESIZE`、`NULLBUFFER`、`INVALIDHMSIZE`、`LL_NULLHEADER`）可能导致未定义行为。

```c
if (alGetStatus(list) != AVAILABLEAL) {
    // 处理错误或退出
}
```
最好不要将带有指针的结构体类型传给容器，容器使用浅拷贝，如果传入了，请注意是否该释放某个资源

### 2. 内存管理

使用 `alDestroy` / `llDestroy` / `deleteString` / `deleteSb` / `hmDestroy` 释放实例，库不会自动回收。

对于 ArrayList / LinkedList，元素数据由库管理（逐字节拷贝），但元素本身若包含指针（如动态分配的字符串），指针指向的内容仍需调用者自行管理。

`splitString` 返回的数组和其中的 String 对象都需要分别释放（先释放每个 String，再释放数组本身）。

### 3. 错误码含义

每个模块的状态码定义在各自的 `XxxSrc.h` 里：

- `StringSrc.h` / `StringBuilderSrc.h`：`stringCommonStatus`
- `ArrayListSrc.h`：`alStatus`
- `LinkedListSrc.h`：`llStatus`
- `HashMapSrc.h`：`hashMapStatus`

建议使用 `stringStatusToArray` / `alStatusToArray` / `llStatusToCharArray` / `hmStatusToArray` 将状态码转换为可读字符串，便于调试。

### 4. HashMap 的句柄机制

HashMap 对外暴露的不是裸指针，而是一个句柄。这个设计被用于尽力解决几个常见问题：

- **别名**：`map2 = map1` 只是复制 identifier，释放 `map1` 后 `map2` 自动失效，不会 double free 或 use-after-free。
- **ABA**：identifier 全局递增、不复用，旧 identifier 永远查不到。
- **无效句柄**：所有操作先查内部注册表，查不到就返回 `NULLHM`。

### 5. LinkedList 的节点机制

LinkedList 对外暴露节点句柄 `linkedListNode`，与值操作接口并存：

- **值操作**：`llPushBack` / `llInsertAt` / `llGetContentAt` 等，调用方只关心数据，不接触节点；
- **节点操作**：`llGetNodeAt` / `llInsertBefore` / `llMoveBefore` / `llRemoveNode` 等，调用方持有节点句柄，可以获得 O(1) 的插入、删除、移动性能。

节点句柄在节点被删除后即悬垂，调用方应遵守"删除后不再使用"的约定。链表内部有哨兵节点 `header`，`llGetHeader` 可获取；哨兵不能通过 `llRemoveNode` 删除，也不能通过 `llSetOfNode` 写入内容。

### 6. 扩容策略

ArrayList 采用倍增扩容（容量不足时翻倍），保证大量插入的均摊性能 O(1)。

HashMap 采用负载因子控制：超过 0.75 扩容翻倍，最小桶数 16，避免抖动。

LinkedList 每个节点单独分配，没有容量概念，不需要扩容/缩容。

若对内存占用敏感，ArrayList 可使用 `alShrinkToFit` 在批量插入后收缩内存。

### 7. 线程安全

**本库 v1.5 非线程安全**。HashMap 的注册表是全局的（或由调用方持有的 `HashMapRegistry`），ArrayList / String / StringBuilder / LinkedList 的实例状态由调用方管理。多线程环境中需调用者自行加锁。

**后续版本计划**：为 HashMap 提供线程安全方案，为其他模块统一引入注册表机制解决别名问题，届时会一并考虑并发安全。

### 8. 语言标准

因该库使用了 `_Generic`，所以请注意使用 C11 或更高标准来编译此库。

---
