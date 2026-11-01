# JCBase-json

C 语言 JSON 解析库。从 `.json` 文件读取文本，解析成内存中的节点树。

本库不依赖任何第三方库，只用 C 标准库。可跨平台编译（Windows / Linux / macOS）。

## 当前状态

**已完成：**

- 从文件读取 JSON 文本
- 解析成节点树
- 递归销毁节点树

**未完成：**

- 从字符串 / 内存缓冲区解析
- 序列化（把节点树写回 JSON 文本）
- 访问接口（按 key 取对象成员、按索引取数组元素、类型转换）
- 流式解析

**注意：** 当前节点结构体定义在 `.c` 文件中，头文件里只有前向声明。用户拿到 `jsonNode*` 后，暂时无法直接访问节点内容。访问接口是下一步的开发内容。

## 文件

| 文件 | 说明 |
|---|---|
| `jsonmain.h` | 对外头文件，声明公开 API 和错误码 |
| `jsonmain.c` | 实现 |
| `wheel_json_ex.h` | 导出宏定义，控制 DLL 导出/导入 |

## 编译

### Windows（MSVC）

1. 创建 C++ DLL 项目。
2. 删除项目模板中所有头文件和源文件。
3. 把本库的三个文件加入项目。
4. 项目属性 → C/C++ → 预编译头 → 选择"不使用预编译头"。
5. 项目属性 → C/C++ → 预处理器 → 预处理器定义，添加 `JCBASEJSON_EXPORTS`。
6. 生成解决方案。

### Linux / macOS

```bash
# 编译成动态库
gcc -shared -fPIC -O2 -o libjcbase-json.so jsonmain.c

# 如果要严格按大文件支持编译
gcc -shared -fPIC -O2 -D_FILE_OFFSET_BITS=64 -o libjcbase-json.so jsonmain.c
```

## API

### jsonParserCreate

```c
jsonParser* jsonParserCreate(const char* filepath);
```

从文件创建一个解析器。

- 打开文件，读取全部内容到内存，返回一个 `jsonParser*`。
- 返回的 parser 持有文本内存，使用完后需要调用 `jsonParserDestroy` 释放（当前版本尚未提供该函数，见"已知限制"）。
- 失败时返回的 parser 非 `NULL`，但状态码非 `JSON_SUCCESSFULOP`。用 `jsonGetParserStatus` 检查。
- 只有 `malloc` 失败时才可能返回 `NULL`。

### jsonGetParserStatus

```c
int jsonGetParserStatus(jsonParser* parser);
```

获取 parser 的状态码，返回 `jsonError` 枚举值。

- `JSON_SUCCESSFULOP` 表示一切正常。
- 其他值表示错误，具体见错误码表。
- 传入 `NULL` 返回 `JSON_NULLPARSER`。

### jsonParse

```c
jsonNode* jsonParse(jsonParser* parser);
```

解析文本，返回根节点。

- 成功返回根节点指针，非 `NULL`。
- 失败返回 `NULL`，具体原因在 `parser->error` 里，用 `jsonGetParserStatus` 获取。

### jsonNodeDestroy

```c
void jsonNodeDestroy(jsonNode* node);
```

递归销毁一棵节点树。

- 传入 `NULL` 安全返回。
- 会释放所有子节点、字符串内容、数组成员、对象成员。

## 错误码

定义在 `jsonmain.h` 的 `jsonError` 枚举里。

| 错误码 | 含义 |
|---|---|
| `JSON_SUCCESSFULOP` | 操作成功 |
| `JSON_NULLPATH` | 文件路径为 NULL |
| `JSON_NULLSTR` | 传入的 jsonString 指针为 NULL |
| `JSON_NULLSTRBUF` | jsonString 的 data 为 NULL |
| `JSON_NULLPARSER` | parser 为 NULL |
| `JSON_ALLOCFAILED` | 内存分配失败 |
| `JSON_OPENFILEFAILED` | 打开文件失败 |
| `JSON_SEEKFAILED` | fseek 失败 |
| `JSON_TELLFAILED` | ftell 失败 |
| `JSON_TOOLARGE` | 文件太大，超出 size_t 范围 |
| `JSON_READFAILED` | 读取文件失败 |
| `JSON_PARSEERROR` | 解析错误 |
| `JSON_UNEXPECTEDEND` | 文本意外结束 |
| `JSON_INVALIDTOKEN` | 遇到非法字符 |

部分错误码附带 `errno`，保存在 `parser->errnoStatus` 里，可用于进一步排查。

## 数据类型

JSON 有六种类型，对应 `jsonType` 枚举：

| 枚举值 | JSON 类型 |
|---|---|
| `JSON_NULL` | null |
| `JSON_BOOL` | true / false |
| `JSON_NUMBER` | 数字 |
| `JSON_STRING` | 字符串 |
| `JSON_ARRAY` | 数组 |
| `JSON_OBJECT` | 对象 |

节点结构 `jsonNode` 内部用一个 union 保存具体内容：

- `content.boolean` — bool
- `content.number` — double
- `content.string` — jsonString（指针 + 长度）
- `content.array` — jsonArray
- `content.object` — jsonObject

对象成员结构 `jsonMember` 有一个 `key`（jsonString）和一个 `value`（`jsonNode*`）。

## 示例

当前 API 只支持"解析"和"销毁"。节点内容暂时不对外暴露。

```c
#include "jsonmain.h"
#include <stdio.h>

int main(void) {
    jsonParser* parser = jsonParserCreate("data.json");

    if (jsonGetParserStatus(parser) != JSON_SUCCESSFULOP) {
        printf("打开或创建 parser 失败，错误码：%d\n",
               jsonGetParserStatus(parser));
        return 1;
    }

    jsonNode* root = jsonParse(parser);
    if (!root) {
        printf("解析失败，错误码：%d\n", jsonGetParserStatus(parser));
        return 1;
    }

    // 拿到根节点了。
    // 当前版本没有访问接口，无法读取节点内容。
    // 保留 root 指针，等待后续版本补充访问 API。

    jsonNodeDestroy(root);
    return 0;
}
```

## 已知限制

1. **节点不透明。** `jsonNode`、`jsonMember` 等结构体定义在 `.c` 文件里，头文件只有前向声明。用户拿到 `jsonNode*` 后无法访问节点内容。
2. **无解析器销毁函数。** `jsonParserCreate` 会分配 parser 和文本内存，当前没有 `jsonParserDestroy`，会造成内存泄漏。
3. **只能从文件解析。** 不支持从字符串、内存缓冲区、stdin 解析。
4. **无序列化。** 不能把节点树写回 JSON 文本。
5. **一次性读入内存。** 解析时需要把整个文件装入内存，超大文件会失败。
6. **数字用 double。** 大整数会丢失精度。

## 解析器行为

- **空白字符**：只认空格、制表、换行、回车（严格按 JSON 标准）。
- **字符串**：完整支持转义，包括 `\"`、`\\`、`\/`、`\b`、`\f`、`\n`、`\r`、`\t`、`\uXXXX`。
- **Unicode**：`\uXXXX` 会转成 UTF-8 输出。支持代理对（`\uD800`-`\uDBFF` 加 `\uDC00`-`\uDFFF`）。
- **数字**：严格按 JSON 语法校验（不允许前导零、尾随小数点、缺少整数部分等）。
- **尾部内容**：解析完根节点后，如果后面还有非空白内容，会报 `JSON_PARSEERROR`。
- **嵌套深度**：递归下降解析器，无深度限制，超深嵌套可能导致栈溢出。
- **大文件**：使用 64 位 fseek/ftell（Windows 用 `_fseeki64`，POSIX 用 `fseeko`/`ftello`），不受 2GB 限制。但仍受内存上限约束。

## 待办

- 解析器销毁函数
- 节点访问接口
- 从字符串 / 缓冲区解析
- 序列化
- 流式解析