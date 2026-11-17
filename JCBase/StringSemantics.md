# StringSemantics
## 此文本作用
阐述该库 String 类模块对某些接口的语义定义，以及某些函数行为的规范。本文件只讲"为什么这样定义、不同接口之间如何保持一致"，具体每个函数的返回值细节请配合 `String.md` 阅读。

## 通用概念
### 三种空
库内 String 实例存在三种"空"状态，语义互不相同：

1. **实例为空（`str == NULL`）**：`JC_String*` 指针为 NULL，不存在任何实例。
2. **缓冲区为空（`str->buffer == NULL`）**：实例存在，但内部 `buffer` 为 NULL，内容不可访问。
3. **内容为空（`str->buffer[0] == '\0'`）**：实例存在，`buffer` 有效，内容长度为 0。

三种空在库内的处理分三类：
- **身份类**（`equals`、`contains`、`compare`）：三种空互不相等，同种空视为相等。
- **查询类**（`isEmpty`、`getLength`）：三种空均视为"空"。
- **操作类**（`append`、`replace`、`trim`、`sub`、`reverse` 等）：按各自规则返回 NULL、异常状态实例或正常结果。

### 实例可用性
"**实例可用**"指：实例指针非 NULL 且 `buffer` 非 NULL，内容可读可操作。

"**主体可用**"用于 append / replace 等以首参数为主体的接口：主体不可用时操作无法执行，返回 NULL（实例为 NULL）或异常状态实例（buffer 为 NULL）；参数不可用时主体内容被保留，返回"主体拷贝 + 参数异常状态"。

### JC_SpaceMode
`JC_SpaceMode` 控制字符类别判定是否跟随 locale：

- `JC_SPACE_ASCII`：仅识别固定 ASCII 字符集（空白：空格、`\t`、`\n`、`\r`、`\v`、`\f`；大小写：`A-Z`、`a-z`）。
- `JC_SPACE_LOCALE` 或未知模式：跟随当前 locale 的 `isspace`、`tolower`、`toupper`。

影响范围：`trim`、`trimFront`、`trimBack`、`isBlank`、`equalsIgnoreCase*`、`containsIgnoreCase*`、`compareIgnoreCase*`、`toLowerCase`、`toUpperCase`、`toInt`、`toLong`、`toLongLong`、`toFloat`、`toDouble`。

### 成功码
所有成功操作的返回值统一为 `JC_STR__SUCCESSFUL_OP`（`split` 的部分成功为 `JC_STR__PARTIAL_SPLIT`）。不再保留 `SUCCEED_TO_INT` 等专用成功码，调用方以"返回码 == `SUCCESSFUL_OP`"作为成功判据。

## 具体接口与语义或规范

### append 类
遵循 **leader first, follower next**：首参数（leader）决定结果实例是否存在，次参数（follower）决定能否完成拼接。

**主体规则（str1 / str）**
- 实例为 NULL：返回 NULL（无法构造结果实例）。
- `buffer == NULL`：返回状态为 `JC_STR__NULL_BUFFER` 的实例（主体不可访问）。

**follower 规则（str2 / content）**
- follower 为 NULL 或 `buffer == NULL`：返回"主体拷贝 + `JC_STR__INVALID_FOLLOWER` / `JC_STR__INVALID_FOLLOWER_BUFFER`"。主体内容被保留，异常通过状态码传达。
- follower 为空串（`buffer != NULL && buffer[0] == '\0'`）：视为拼接单位元，返回主体拷贝，状态 `AVAILABLE`。

**正常拼接**：返回 str1 + str2 的新实例，状态 `AVAILABLE`。

### reverse
对三种空分别处理：
- `str == NULL`：返回 NULL。
- `buffer == NULL`：返回 `JC_STR__NULL_BUFFER` 实例。
- 空串：返回空串实例（`AVAILABLE`）。
- 其余：返回反转内容副本。

### split
遵循**如实返回**规范：不掩饰任何元素的失败。

- `outArray` / `count` 为 NULL：分别返回 `JC_STR__NULL_ARRAY` / `JC_STR__NULL_COUNT`，不修改另一输出参数。
- `str == NULL` / `buffer == NULL`：返回对应错误码，`*outArray = NULL`、`*count = 0`。
- 全部段都被过滤：返回 `JC_STR__SUCCESSFUL_OP`，`*outArray = NULL`、`*count = 0`（成功但空结果）。
- 部分元素分配失败：返回 `JC_STR__PARTIAL_SPLIT`，对应位置可能是 NULL，或状态为 `JC_STR__NULL_BUFFER` 的实例。
- 全部成功：返回 `JC_STR__SUCCESSFUL_OP`。

**空串处理**：`removeEmptyEntries == false` 时空段作为空串实例保留；`true` 时丢弃。

**所有权**：调用者负责 `jcstr_delete` 每个元素、`free` 数组本身。

### trim 系列
三种空各自处理：
- `str == NULL`：返回 NULL。
- `buffer == NULL`：返回 `JC_STR__NULL_BUFFER` 实例。
- 空串：返回空串实例。
- 全空白：返回空串实例（内容清空）。
- 其余：返回去掉对应侧空白后的内容副本。

空白判定受 `mode` 控制。`trim` / `trimFront` / `trimBack` 与 `isBlank` 使用同一字符集判据，保证"`isBlank` 说全空白，`trim` 能清空"。

### charAt
不接受不可操作实例：
- `result == NULL` → `JC_STR__NULL_CHAR`。
- `str == NULL` → `JC_STR__NULL`。
- `buffer == NULL` → `JC_STR__NULL_BUFFER`。
- `index` 越界 → `JC_STR__IDX_OUT_BOUNDS`。

### indexOf 系列
不接受内容不可操作的实例：
- `result == NULL` → `JC_STR__NULL_RECEIVER`。
- `str == NULL` → `JC_STR__NULL`。
- `buffer == NULL` → `JC_STR__NULL_BUFFER`。
- 未找到 → `JC_STR__WITHOUT_CH`，不修改 `*result`。

**空子串语义**：`WithInst` / `WithCh` 版本中，空子串视为在**末尾**匹配，`*result = str->cachedLen`。这是有意的语义选择，而非 `strstr(str, "")` 返回"开头"的默认行为。选择"末尾"是为了与 `replace` 的空 target 语义（末尾追加）保持一致。

### sub 系列
三种空各自返回对应实例或 NULL：
- 实例为 NULL / `ch` 为 NULL：返回 NULL 或 `JC_STR__NULL_BUFFER` 实例。
- `buffer == NULL`：返回 `JC_STR__NULL_BUFFER` 实例。
- 空串 / `length == 0` / `start == end`：返回空串实例。

越界处理：返回状态为 `INVALID_START` / `INVALID_END` / `INVALID_IDX` / `IDX_OUT_BOUNDS` 的**空串实例**（不是 NULL、不是异常 buffer），内容部分截断到合法范围。这样调用方拿到的是"内容上安全、状态上告知越界"的实例。

### replace 系列
**主体规则**：`str == NULL` 返回 NULL；`buffer == NULL` 返回 `JC_STR__NULL_BUFFER` 实例。

**参数规则**：
- `target == NULL`：返回 str 拷贝 + `JC_STR__NULL_TARGET`。
- `replacement == NULL`（`WithCh`）或 `replacement == NULL || replacement->buffer == NULL`（`WithInst`）：返回 str 拷贝 + `JC_STR__INVALID_REPLACEMENT`。

`WithInst` 对 `replacement` 只看 buffer 是否可用，不看 `replacement->status`。和可变版保持一致。

**空 target 语义**：
| 情况 | 行为 |
|---|---|
| 空内容 + 空 target | 整体设为 replacement |
| 空内容 + 非空 target | 返回原内容副本（无可替换） |
| 非空内容 + 空 target | 末尾追加 replacement（等价 `appendWithCh`） |

"空 target = 末尾匹配"与 `indexOf("")` 返回 `cachedLen` 自洽。

**匹配规则**：`replaceFirst` 替换第一次出现；`replaceAll` 替换所有**非重叠**出现（`scan += targetLen`）。未找到时返回原内容副本。

**自替换**：不可变版**允许** `replacement` 指向 `str->buffer` 内部——因为分配新 buffer 后原 buffer 全程有效，不会悬空。可变版在入口**拒绝**自替换（返回 `JC_MS__INVALID_REPLACEMENT`），因为 `realloc` 会移动原 buffer。这是有意的不对称，由"分配模型"决定。

### startWith / endWith
**两种参数均拒绝不可操作实例**（按"参数是必需值"而非"可空值"处理）：
- `str == NULL` 或 `buffer == NULL`：返回 false。
- `affix == NULL`：返回 false（参数缺失，匹配不成立）。
- `affix` 为空串：返回 true（空串是任何有效字符串的前缀/后缀）。
- 其余：按 `strncmp` 判定。

`WithInst` 版本转交 `WithCh` 版本：`prefix->buffer == NULL` 由 `WithCh` 的 `!prefix` 分支处理为 false，`prefix->buffer == ""` 由 `WithCh` 的空串分支处理为 true。

### isEmpty / isBlank
`isEmpty`：判断库中定义的三种空，`str == NULL`、`buffer == NULL`、`buffer[0] == '\0'` 均返回 true。

`isBlank`：`isEmpty` 为 true 则返回 true；其余按 `mode` 判断是否仅由空白字符组成。`isBlank` 包含 empty。

### equals
**判定规则**：
| str1 | str2 | 结果 |
|---|---|---|
| NULL | NULL | true |
| NULL | 非 NULL | false |
| buffer == NULL | buffer == NULL | true |
| buffer == NULL | buffer != NULL | false |
| 空串 | 空串 | true |
| 空串 | 非空 | false |
| 非空 | 非空 | 按内容比较 |

`WithCh` 版本把 `char*` 也视为"内容层"的一种存在形式：`str->buffer == NULL && ch == NULL` 返回 true（同种"无"），单方 NULL 返回 false。和 `WithInst` 的"实例层"判定是两套层次，不对等。

`IgnoreCase` 版本在内容比较前先比长度，再逐字符 `toLowerInternal(..., mode)`。

### contains
判定同 `equals`：
- 两个 NULL → true（NULL 包含 NULL）。
- 一个 NULL → false。
- 两个 `buffer == NULL` → true。
- 一个 `buffer == NULL` → false。
- 其余：`strstr(str1->buffer, str2->buffer) != NULL`。

空串 needle 视为任何有效字符串的子串（`strstr` 标准行为，无需额外处理）。

`contains` 与 `equals` 保持同一条"两种无相等"规则，`contains(a, b)` 在 `equals(a, b)` 为 true 时也必然为 true。

### compare
**判定同 `equals`**，但结果用 `int*` 输出，函数返回状态码：
- `result == NULL` → `JC_STR__NULL_RECEIVER`。
- 两个 str NULL → `*result = 0`，`JC_STR__SUCCESSFUL_OP`。
- 一个 str NULL → `JC_STR__NULL`。
- 两个 `buffer == NULL` → `*result = 0`，`JC_STR__SUCCESSFUL_OP`。
- 一个 `buffer == NULL` → `JC_STR__NULL_BUFFER`。
- 其余：`*result = strcmp(...)`，`JC_STR__SUCCESSFUL_OP`。

**与 `equals` 的对应关系**：`equals(a, b) == (compare(a, b, &r) == SUCCESSFUL_OP && r == 0)` 对所有输入成立。

`WithCh` 版本与 `equalsWithCh` 同构：`str->buffer == NULL && ch == NULL` 时 `*result = 0` 并返回 `SUCCESSFUL_OP`。

### toCharArray
返回拷贝，调用者负责 `free`。`str == NULL` 或 `buffer == NULL` 返回 NULL；空串返回可 `free` 的非 NULL 空串。

### toNum 系列
**允许非严谨数学式，允许十六进制，`inf` / `nan` 与 String 现有设计一致。**

- **整数**：十进制，允许前导/尾随空白（受 `mode` 控制）和 `+/-` 号；拒绝十六进制、下划线、中间空白。溢出返回对应 `OVERFLOW_*`。
- **浮点**：底层使用 `strtod`，接受 `.5`、`5.`、十六进制浮点（`0x1.8p1`）、`inf` / `infinity` / `nan`（大小写不敏感）；`errno == ERANGE` 检出溢出。

状态检查先于解析：`str == NULL` / `buffer == NULL` / 空串 → `JC_STR__EMPTY`；`status != AVAILABLE` → 返回 `str->status`；`result == NULL` → `JC_STR__NULL_*`。

### toLowerCase / toUpperCase
三种空各自处理：
- `str == NULL`：返回 NULL。
- `buffer == NULL`：返回 `JC_STR__NULL_BUFFER` 实例。
- 空串：返回空串实例。
- 其余：返回转换后内容副本，转换受 `mode` 控制。

### toMutableString
- `str == NULL`：返回 NULL。
- `buffer == NULL`：返回状态为 `JC_MS__NULL_BUFFER` 的可变字符串实例。
- 其余：返回内容相同的可变字符串实例。

## 不变量
库内维护以下不变量，所有接口的行为以此为假设：

1. `status == JC_STR__AVAILABLE` 蕴含 `buffer != NULL`。
2. `cachedLen == strlen(buffer)`（若 `buffer != NULL`）。
3. `buffer` 中间不出现 `'\0'`，终止符仅在 `buffer[cachedLen]` 位置（由 `changeAt` 拒绝写入 `'\0'` 等机制维持）。
4. 不可变版所有产生新实例的操作不修改原实例。