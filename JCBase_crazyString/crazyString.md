# crazyString
## 其他语言md
### English
[crazyString.md English version](crazyString_EN.md)
## 接口介绍
> jc_czyString jccstr_new(void)
```text
此为库对外接口，返回一个内容为空的实例，内部缓冲区为 1 字节，len 为 0，cap 为 1
注意：分配失败返回 NULL；返回的实例 buffer 恒非 NULL，需通过 jccstr_delete 释放
```
> jc_czyString jccstr_newWithSize(size_t capacity)
```text
此为库对外接口，返回一个内容为空、容量为 capacity 字节的实例（capacity 含结尾 '\0'）
注意：capacity 为 0 返回 NULL；分配失败返回 NULL；例如 capacity 为 201 时最多可存放 200 个内容字符
```
> jc_czyString jccstr_newFrom(const char* content, size_t length)
```text
此为库对外接口，从 content 拷贝 length 字节创建一个实例，并在末尾补 '\0'，容量为 length + 1
注意：content 为 NULL 或 length 为 SIZE_MAX 时返回 NULL；length 小于 content 实际内容长度时按截断处理，属于本接口语义；length 不得大于 strlen(content)，否则属于调用方违约；返回的实例需通过 jccstr_delete 释放
```
> void jccstr_delete(jc_czyString str)
```text
此为库对外接口，无返回值，释放传入的实例及其内部缓冲区
注意：传入 NULL 不会崩溃；但请不要传入强转为 “jc_czyString” 的非 jc_czyString 变量
```
> const char* jccstr_viewBuffer(jc_czyString str)
```text
此为库对外接口，直接返回实例内部的 buffer 指针，用于只读查看
注意：建议只读，不应通过该指针修改内容；该指针在实例被修改、扩容或释放后可能失效；str 为 NULL 时返回 NULL
```
> char* jccstr_toCharArray(jc_czyString str)
```text
此为库对外接口，返回一份内容拷贝，以 '\0' 结尾
注意：返回值需要调用者 free；分配失败或 str 为 NULL 时返回 NULL
```
> size_t jccstr_getLength(jc_czyString str)
```text
此为库对外接口，返回 size_t 类型的内容长度（截至第一个 '\0'，不含结尾 '\0'）
注意：str 为 NULL 时返回 0
```
> size_t jccstr_getCapacity(jc_czyString str)
```text
此为库对外接口，返回 size_t 类型的容量大小（字节，含结尾 '\0'）
注意：str 为 NULL 时返回 0；容量与内容长度无关，可存放的内容字符数为 cap - 1
```
> jc_czyString jccstr_changeAt(jc_czyString str, size_t index, char content)
```text
此为库对外接口，返回 jc_czyString 类型，将实例中下标为 index 的字符改为 content
注意：content 为 '\0' 时行为等同 jccstr_endAt，即在此截断且 len 变为 index；写非 '\0' 时要求 index < len，写 '\0' 时要求 index <= len；返回值为 str 本身，便于链式调用
```
> jc_czyString jccstr_changeFrom(jc_czyString str, size_t from, size_t length, const char* content)
```text
此为库对外接口，返回 jc_czyString 类型，从 from 开始用 content 的前 length 字节覆盖原内容
注意：若写入范围越过原 len，会在 from + length 处补 '\0'；容量不足时会尝试扩容，失败则原样返回；from + length + 1 溢出时原样返回；content 中的 '\0' 会按 C 语义截断字符串；要求 from <= len
```
> jc_czyString jccstr_endAt(jc_czyString str, size_t index)
```text
此为库对外接口，返回 jc_czyString 类型，将下标 index 处设为 '\0' 并令 len 为 index
注意：要求 index <= len；返回值为 str 本身
```
> jc_czyString jccstr_clear(jc_czyString str)
```text
此为库对外接口，返回 jc_czyString 类型，将缓冲区首字节置 '\0' 并将 len 置 0
注意：容量保持不变；返回值为 str 本身；需要释放多余内存请再调用 jccstr_shrinkToFit
```
> jc_czyString jccstr_removeAt(jc_czyString str, size_t index)
```text
此为库对外接口，返回 jc_czyString 类型，删除下标 index 处的字符，后续内容左移一位，len 减一
注意：要求 index < len；返回值为 str 本身
```
> jc_czyString jccstr_removeInRange(jc_czyString str, size_t from, size_t to)
```text
此为库对外接口，返回 jc_czyString 类型，删除左闭右开区间 [from, to) 内的字符，len 减少 to - from
注意：要求 from <= to <= len；from 等于 to 时为空区间，内容不变；返回值为 str 本身
```
> jc_czyString jccstr_insertAt(jc_czyString str, size_t index, char content)
```text
此为库对外接口，返回 jc_czyString 类型，在下标 index 处插入一个字符，[index, len]（含终止符）整体右移一位
注意：要求 index <= len，index 等于 len 时等价于追加；content 为 '\0' 时行为等同 jccstr_endAt；容量不足时每次只扩容 1 字节，连续插入大量字符建议先调用 jccstr_reserve；返回值为 str 本身
```
> jc_czyString jccstr_insertBytes(jc_czyString str, size_t index, const char* content, size_t length)
```text
此为库对外接口，返回 jc_czyString 类型，在下标 index 处插入 content 的前 length 字节，原有内容整体右移
注意：length 为 0 时无操作；length 使得 str->len + length + 1 溢出时原样返回；容量不足时会尝试扩容，失败则原样返回；要求 index <= len，index 等于 len 时等价于追加；content 不得指向 str->buffer 内部
```
> jc_czyString jccstr_setContent(jc_czyString str, const char* content, size_t length)
```text
此为库对外接口，返回 jc_czyString 类型，用 content 的前 length 字节替换整个内容
注意：容量不足时会尝试扩容，失败则原样返回；length 为 SIZE_MAX 时按 SIZE_MAX - 1 处理，扩容必然失败并原样返回；要求 content 非 NULL；返回值为 str 本身
```
> jc_czyString jccstr_reserve(jc_czyString str, size_t wholeCapacity)
```text
此为库对外接口，返回 jc_czyString 类型，为实例预分配空间；当 cap 小于 wholeCapacity 时扩容到至少 wholeCapacity 字节
注意：只增不减；扩容失败时保持原容量；返回值为 str 本身
```
> jc_czyString jccstr_shrinkToFit(jc_czyString str)
```text
此为库对外接口，返回 jc_czyString 类型，将容量收缩到恰好 len + 1 字节，第一个 '\0' 之后的余量全部释放
注意：收缩失败时保持原容量；内容不变；返回值为 str 本身
```
> jc_czyString jccstr_reallocate(jc_czyString str, size_t capacity)
```text
此为库对外接口，返回 jc_czyString 类型，将容量直接设置为 capacity 字节；若 capacity 小于等于 len，内容会被截断到 capacity - 1 个字符
注意：缩容成功且内容放不下时会截断并补 '\0'；capacity 为 0 时依赖 realloc 的实现相关行为，由调用方自行负责；重新分配失败时容量与内容均不变；返回值为 str 本身
```
> jc_czyString jccstr_clone(jc_czyString str)
```text
此为库对外接口，返回一份完全相同的克隆实例，容量、长度以及第一个 '\0' 之后的残留内容一并复制
注意：str 为 NULL 或分配失败时返回 NULL；返回的实例需通过 jccstr_delete 释放；只需干净内容时可使用 jccstr_newFrom(jccstr_viewBuffer(str), jccstr_getLength(str))
```
> jc_czyString jccstr_appendBytes(jc_czyString str, const char* content, size_t length)
```text
此为库对外接口，返回 jc_czyString 类型，将 content 的前 length 字节追加到末尾，容量不足时精确扩容到所需大小
注意：长度溢出或扩容失败时原样返回；content 中出现 '\0' 时按 C 语义截断（追加部分止于该 '\0'）；要求 content 非 NULL；返回值为 str 本身
```
> jc_czyString jccstr_append(jc_czyString str, jc_czyString other)
```text
此为库对外接口，返回 jc_czyString 类型，将 other 的内容追加到 str 末尾，other 本身不变
注意：允许 str 与 other 为同一实例（自拼接）；other 长度为 0 时无操作；长度溢出或扩容失败时原样返回；返回值为 str 本身
```
> jc_czyString jccstr_concat(jc_czyString a, jc_czyString b)
```text
此为库对外接口，返回一个新实例，内容为 a 后接 b，a 与 b 均不变
注意：任一参数为 NULL、长度溢出或分配失败时返回 NULL；返回的实例需通过 jccstr_delete 释放
```
