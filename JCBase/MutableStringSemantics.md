# MutableStringSemantics
## 此文本作用
阐述该库 MutableString 类模块对某些接口的语义定义，以及某些函数行为的规范。本文件只讲"为什么这样定义、不同接口之间如何保持一致"，具体每个函数的返回值细节请配合 `MutableString.md` 阅读。与 `StringSemantics.md` 有大量共享规则，两文件应交叉阅读。

## 通用概念
### 三种空
MutableString 实例存在三种"空"状态，语义互不相同：

1. **实例为空（`ms == NULL`）**：`JC_MutableString*` 指针为 NULL，不存在任何实例。
2. **缓冲区为空（`ms->buffer == NULL`）**：实例存在，但内部 `buffer` 为 NULL，内容不可访问。
3. **内容为空（`ms->len == 0`，等价 `ms->buffer[0] == '\0'`）**：实例存在，`buffer` 有效，`len` 为 0。

三种空在库内的处理分三类，和 String 模块一致：
- **身份类**（`equals`、`contains`、`compare`）：三种空互不相等，同种空视为相等。
- **查询类**（`isEmpty`、`getLength`）：三种空均视为"空"。
- **操作类**（`append`、`replace`、`trim`、`sub`、`reverse` 等）：按各自规则处理。

### 实例可用性
"**实例可用**"指：实例指针非 NULL 且 `buffer` 非 NULL。

### 原地修改 vs 返回新实例
MutableString 的接口分两类语义：

- **原地修改类**：`setContent`、`changeAt`、`append*`、`reverse`、`trim*`、`replace*`、`toLowerCase`、`toUpperCase`。成功时修改 `ms` 本身，失败时保证 `ms` 保持操作前状态（不产生半成品）。
- **返回新实例类**：`clone`、`sub*`、`split` 的每个元素、`toString` 的结果、`toCharArray` 的结果。不修改 `ms`，返回独立实例。

### JC_SpaceMode
和 String 模块一致。影响范围：`trim`、`trimFront`、`trimBack`、`isBlank`、`equalsIgnoreCase*`、`containsIgnoreCase*`、`compareIgnoreCase*`、`toLowerCase`、`toUpperCase`、`toInt`、`toLong`、`toLongLong`、`toFloat`、`toDouble`。

### 成功码
所有成功操作的返回值统一为 `JC_MS__SUCCESSFUL_OP`（`split` 的部分成功为 `JC_MS__PARTIAL_SPLIT`）。旧的 `MS_SUCCEEDTOINT` 等专用成功码已废弃，调用方以"返回码 == `SUCCESSFUL_OP`"作为成功判据。

### status 字段的检查取舍
可变版对 `status != AVAILABLE` 的处理分两类：

- **写入依赖实例完整性的操作**：`setContent`、`changeAt`、`replace*` 在入口检查 `ms->status != AVAILABLE`，异常时返回 `ms->status`。理由：写入一个"状态标记为不可信"的实例会破坏不变量。
- **按内容计算的操作**：`reverse`、`trim*`、`toLowerCase`、`toUpperCase` 不检查 `status`，只在 `buffer == NULL` 时失败。理由：这些操作读的是内容本身，只要 `buffer` 可读，转换不会破坏任何东西；`status` 异常说明"上次操作失败"，不说明"内容不可用"。

不可变版对应地采取同样的取舍（`toLowerCase` 等不查 `status`，`toInt` 等查 `status`）。

## 具体接口与语义或规范

### 构造与容量
`jcms_new`：返回空实例，容量 `INITIAL_MS_SIZE + 1`。

`jcms_newFrom`：容量 `max(strlen(content), INITIAL_MS_SIZE) + 1`。这是唯一一次性预留较大容量的构造路径，后续追加在容量足够时不触发 `realloc`。

容量增长策略（`appendCore` / `setContent` 内部）：
- `setContent`：只在容量不足时 `realloc` 到 `ceilAlignInitSize(needed)`，对齐到 `INITIAL_MS_SIZE` 的倍数，不留额外增长余量。
- `appendCore`：`srcLen` 大时精确对齐到 `needed`；`srcLen` 小时按 1.25 倍增长；其余情况按 2 倍增长。三种策略都对齐 `INITIAL_MS_SIZE`。

### append 族
**主体规则（ms）**
- `ms == NULL`：返回 `JC_MS__NULL`。
- `ms->buffer == NULL`：返回 `JC_MS__NULL_BUFFER`。

**follower 规则**
- `src == NULL`（`WithInst`）：返回 `JC_MS__NULL_FOLLOWER`。
- `src->buffer == NULL`（`WithInst`）：返回 `JC_MS__NULL_FOLLOWER_BUFFER`。
- `ch == NULL`（`WithCh`）：返回 `JC_MS__NULL_FOLLOWER`。
- `src->len == 0` 或 `ch[0] == '\0'`：返回 `SUCCESSFUL_OP`，不修改（空串是单位元）。

**自追加**：
- `appendWithInst(ms, ms)` 是**允许**的。`appendCore` 接受 `char**`，当 `src == ms` 时 `&src->buffer` 与 `&ms->buffer` 同址，`realloc` 后 `*src` 自动更新为新地址，不会悬空。
- `appendWithCh(ms, ms->buffer + offset)` 是**拒绝**的，返回 `JC_MS__INVALID_SOURCE`。`const char*` 参数无法在 `realloc` 后自动更新，为避免悬空，入口直接拒绝 `ch` 指向 `ms->buffer` 内部。

这个不对称是**有意**的：`WithInst` 的二级指针恰好能处理别名，`WithCh` 的 `const char*` 不能。

**与不可变版的差异**：不可变版 `append` 返回新实例；可变版原地修改 `ms`，返回值是状态码。失败时可变版的 `ms` 保持原样，不可变版返回带状态的新实例。语义对应但形式不同。

### setContent
将 `ms` 内容设置为 `content` 的副本。

**content 为 NULL**：等价于"清空"——`buffer[0] = '\0'`、`len = 0`，但**不释放 `buffer`、不修改 `cap`**。这样调用方可以反复 `setContent(ms, NULL)` 清空而不损失容量。

**status 检查**：入口检查 `ms->status != AVAILABLE`，异常时返回 `ms->status`。

**容量不足时**：`realloc` 到 `ceilAlignInitSize(needed)`，失败返回 `JC_MS__REALLOCATION_FAILED`，`ms` 不变。

### changeAt
**status 检查**：入口检查 `ms->status != AVAILABLE`，异常时返回 `ms->status`。

**拒绝 `\0`**：`ch == '\0'` 时返回 `JC_MS__INVALID_CHARACTER_FOR_CHANGE`。这条保证"内容中间不出现 `\0`"的不变量，进而保证 `len == strlen(buffer)`。

### reverse
**不查 status**，只在 `buffer == NULL` 时失败。

三种空分别处理：
- `ms == NULL`：返回 `JC_MS__SUCCESSFUL_OP`（无事可做）。
- `buffer == NULL`：返回 `JC_MS__SUCCESSFUL_OP`（无事可做）。
- 空串：返回 `JC_MS__SUCCESSFUL_OP`（无事可做）。

这跟不可变版不同：不可变版 `reverse(NULL)` 返回 NULL、`reverse(bufferNull)` 返回 `NULL_BUFFER` 实例。可变版是原地操作，"没有可改的东西"直接视为成功。

### split
和不可变版 `split` 行为逐条一致：
- `outArray` / `count` 为 NULL：返回 `JC_MS__NULL_ARRAY` / `JC_MS__NULL_COUNT`。
- `ms == NULL` / `buffer == NULL`：返回对应错误码，输出参数置 `NULL` / 0。
- 全部段被过滤：返回 `SUCCESSFUL_OP`，`*outArray = NULL`、`*count = 0`。
- 部分元素构造失败：返回 `JC_MS__PARTIAL_SPLIT`，对应位置可能是 NULL 或状态异常的实例。
- 全部成功：返回 `SUCCESSFUL_OP`。

**所有权**：调用者负责 `jcms_delete` 每个元素、`free` 数组本身。

### trim 系列
**不查 status**，只在 `buffer == NULL` 或 `len == 0` 时直接返回 `SUCCESSFUL_OP`（无事可做）。

原地修改：去掉对应侧空白后 `memmove` 收缩内容，`buffer[newLen] = '\0'`，更新 `len`。全空白时 `buffer[0] = '\0'`、`len = 0`，但**不释放 buffer、不修改 cap**。

空白判定受 `mode` 控制，与 `isBlank` 同源。

### charAt
和不可变版一致：`result == NULL` → `JC_MS__NULL_CHAR`；`ms == NULL` → `JC_MS__NULL`；`buffer == NULL` → `JC_MS__NULL_BUFFER`；`index` 越界 → `JC_MS__INDEX_OUT_OF_BOUNDS`。

### indexOf 系列
和不可变版一致。空子串在 `WithInst` / `WithCh` 中视为**末尾匹配**，`*result = ms->len`。

`result == NULL` 在可变版返回 `JC_MS__NULL_RESULT`（不可变版是 `JC_STR__NULL_RECEIVER`）。命名不统一是历史原因，语义相同，都是"输出接收指针为 NULL"。

### sub 系列
**返回新实例，不修改原 `ms`**。和不可变版 `sub` 语义一致：
- `ms == NULL`：返回 NULL。
- `buffer == NULL`：返回 `JC_MS__NULL_BUFFER` 实例。
- 空串 / `length == 0` / `start == end`：返回空串实例。
- 越界：返回状态为 `INVALID_START` / `INVALID_END` / `INVALID_INDEX` / `INDEX_OUT_OF_BOUNDS` 的空串实例，内容截断到合法范围。

### replace 系列
**status 检查**：入口检查 `ms->status != AVAILABLE`，异常时返回 `ms->status`。

**参数规则**：
- `target == NULL` → `JC_MS__NULL_TARGET`。
- `replacement == NULL`（`WithCh`）或 `replacement == NULL || replacement->buffer == NULL`（`WithInst`）→ `JC_MS__INVALID_REPLACEMENT`。`WithInst` 只看 buffer 是否可用，不看 `replacement->status`，和不可变版一致。

**自替换**：**拒绝**。`replacement` 指向 `ms->buffer` 内部时返回 `JC_MS__INVALID_REPLACEMENT`。理由：不等长路径需要 `realloc`，`realloc` 移动块后 `replacement` 悬空。这和不可变版的"允许"形成不对称——不可变版分配新 buffer，原 buffer 全程有效。

**空 target 语义**：
| 情况 | 行为 |
|---|---|
| 空内容 + 空 target | 整体设为 replacement |
| 空内容 + 非空 target | `NO_TARGET_MATCHED`，`ms` 不变 |
| 非空内容 + 空 target | 末尾追加 replacement（等价 `appendCore`） |

**非空 target**：
- `strstr` 找第一次出现（`First`）或 `memcmp` 扫描所有非重叠出现（`All`）。
- 找不到 → `JC_MS__NO_TARGET_MATCHED`，`ms` 不变。
- 找到 → 原地替换，需要时 `realloc`。失败返回 `JC_MS__REALLOCATION_FAILED`，`ms` 不变（`realloc` 失败原块保留）。

**不变量**：所有失败路径都保证 `ms` 在操作前后的 `buffer`、`len`、`cap`、`status` 完全一致。

### startWith / endWith
和不可变版一致：
- `ms == NULL` 或 `buffer == NULL`：返回 false。
- `affix == NULL`：返回 false。
- `affix` 为空串：返回 true。
- 其余按 `strncmp` 判定。

`WithInst` 版本转交 `WithCh`，`prefix->buffer == NULL` 由 `WithCh` 的 `!prefix` 分支处理为 false，`prefix->buffer == ""` 由 `WithCh` 的空串分支处理为 true。

### isEmpty / isBlank
`isEmpty`：`ms == NULL`、`buffer == NULL`、`buffer[0] == '\0'` 均返回 true。

`isBlank`：`isEmpty` 为 true 则返回 true；其余按 `mode` 判断是否仅由空白字符组成。`isBlank` 包含 empty。

### equals / contains / compare
三个函数的判定规则和不可变版**逐条一致**：

- 两个 NULL → true / `*result = 0`。
- 一个 NULL → false / `JC_MS__NULL`。
- 两个 `buffer == NULL` → true / `*result = 0`。
- 一个 `buffer == NULL` → false / `JC_MS__NULL_BUFFER`。
- 两个空串 → true。
- 一个空串 → false。
- 其余按内容比较 / `strcmp`。

**对应关系**：`equals(a, b) == (compare(a, b, &r) == SUCCESSFUL_OP && r == 0)` 对所有输入成立。

**空串 needle**：`contains` 中空串 needle 视为任何有效字符串的子串（`strstr` 标准行为）。

### toCharArray
返回拷贝，调用者负责 `free`。`ms == NULL` 或 `buffer == NULL` 返回 NULL；空串返回可 `free` 的非 NULL 空串。

### toNum 系列
和不可变版**逐条对齐**：

- **status 检查**：`ms->status != AVAILABLE` 时返回 `ms->status`（不可变版同样检查）。
- **空检查**：`ms == NULL || buffer == NULL || buffer[0] == '\0'` → `JC_MS__EMPTY`。
- **result 检查**：`result == NULL` → `JC_MS__NULL_*`。
- **整数**：十进制，允许 `+/-` 号和空白（受 `mode` 控制），拒绝十六进制、下划线、中间空白。溢出返回 `OVERFLOW_*`。
- **浮点**：底层 `strtod`，接受 `.5`、`5.`、十六进制浮点、`inf` / `nan`。`errno == ERANGE` 检出溢出。

**和不可变版的内部差异**：可变版 `parseToLL` / `parseToDouble` 用 `ms->len` 作边界（不重新 `strlen`），不可变版用 `str->cachedLen`。两者都依赖"`len == strlen(buffer)`"的不变量，行为等价。

### toLowerCase / toUpperCase
**不查 status**，只在 `buffer == NULL` 时失败。理由：按内容计算，`status` 异常不阻止内容转换。

原地修改：逐字符 `toLowerInternal(..., mode)` / `toUpperInternal(..., mode)` 替换 `buffer[i]`。空串时循环零次，直接返回成功。

### toString
- `ms == NULL`：返回 `jcstr_newFrom(NULL)`，即状态为 `JC_STR__NULL_BUFFER` 的不可变实例。
- `buffer == NULL`：同上。
- 其余：返回内容相同的不可变实例，状态 `AVAILABLE`。

## 不变量
库内维护以下不变量，所有接口的行为以此为假设：

1. `status == JC_MS__AVAILABLE` 蕴含 `buffer != NULL`。
2. `len == strlen(buffer)`（若 `buffer != NULL`）。
3. `cap >= len + 1`（若 `buffer != NULL`），保证 `buffer[len] == '\0'` 有位置。
4. `buffer` 中间不出现 `'\0'`，终止符仅在 `buffer[len]` 位置（由 `changeAt` 拒绝 `'\0'`、`setContent` 用 `strlen` 计算长度等机制维持）。
5. 原地修改类接口失败时，`ms` 的 `buffer`、`len`、`cap`、`status` 与调用前完全一致。