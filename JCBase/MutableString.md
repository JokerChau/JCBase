# MutableString
## 其他语言md
### English
[MutableString.md English version](MutableString_EN.md)
## 接口介绍
> jc_MutableString jcms_new(void)
```text
此为库对外接口，返回一个内容为空、容量为 INITIAL_MS_SIZE + 1 的可变字符串实例
注意：返回的实例需通过 jcms_delete 释放
```
> jc_MutableString jcms_newFrom(const char* const content)
```text
此为库对外接口，从 C 字符串内容创建一个可变字符串实例
注意：若 content 为 NULL，返回的实例状态为 JC_MS__NULL_BUFFER；该接口对 content 进行拷贝，不接管其所有权；容量为 max(strlen(content), INITIAL_MS_SIZE) + 1；返回的实例需通过 jcms_delete 释放
```
> void jcms_delete(jc_MutableString ms)
```text
此为库对外接口，无返回值，释放传入的可变字符串实例及其内部缓冲区
注意：传入 NULL 不会崩溃；但请不要传入强转为 “jc_MutableString” 的非 jc_MutableString 变量
```
> int jcms_getStatus(const JC_MutableString* const ms)
```text
此为库对外接口，返回 int 类型的状态码，需要传入一个可变字符串实例
注意：当 ms 为 NULL 时返回 JC_MS__NULL；否则返回实例当前状态
```
> size_t jcms_getLength(const JC_MutableString* const ms)
```text
此为库对外接口，返回 size_t 类型的字符串长度（不含结尾 '\0'），需要传入一个可变字符串实例
注意：当 ms 为 NULL 时返回 0；否则返回实例缓存的长度，而非容量
```
> const char* jcms_viewBuffer(const JC_MutableString* const ms)
```text
此为库对外接口，直接返回实例内部的 buffer 指针，用于只读查看
注意：建议只读，不应通过该指针修改内容；若修改导致意外错误，非该库责任；ms 为 NULL 时返回 NULL
```
> int jcms_setContent(const jc_MutableString ms, const char* const content)
```text
此为库对外接口，返回 int 类型的状态码，将实例内容设置为 content
注意：若 content 为 NULL，行为等同于传入空串——将 buffer 首字节置 '\0'、len 置 0，但不释放 buffer；该接口不能用于释放内存；ms 状态异常时返回 ms->status；返回的实例容量不足时会自动扩容
```
> int jcms_changeAt(const jc_MutableString ms, size_t index, char ch)
```text
此为库对外接口，返回 int 类型的状态码，将实例中下标为 index 的字符替换为 ch
注意：index 从 0 开始；ms 为 NULL 返回 JC_MS__NULL；缓冲区为 NULL 返回 JC_MS__NULL_BUFFER；状态异常返回 ms->status；index 越界返回 JC_MS__INDEX_OUT_OF_BOUNDS；ch 不能为 '\0'，否则返回 JC_MS__INVALID_CHARACTER_FOR_CHANGE；成功返回 JC_MS__SUCCESSFUL_OP
```
> const char* jcms_statusToCharArray(const int status)
```text
此为库对外接口，返回 int 类型状态码对应的可读字符串描述
注意：返回的是静态字符串，调用者无需也不应释放
```
> int jcms_appendWithInst(const jc_MutableString ms, const JC_MutableString* const src)
```text
此为库对外接口，返回 int 类型的状态码，将 src 的内容追加到 ms 末尾
注意：ms 为 NULL 返回 JC_MS__NULL；ms 缓冲区为 NULL 返回 JC_MS__NULL_BUFFER；src 为 NULL 返回 JC_MS__NULL_FOLLOWER；src 缓冲区为 NULL 返回 JC_MS__NULL_FOLLOWER_BUFFER；src 长度为 0 时返回成功但不修改；ms 长度为 0 时等价于 setContent；容量不足时自动扩容
```
> int jcms_appendWithCh(const jc_MutableString ms, const char* const ch)
```text
此为库对外接口，返回 int 类型的状态码，将 C 字符串 ch 的内容追加到 ms 末尾
注意：ms 为 NULL 返回 JC_MS__NULL；ms 缓冲区为 NULL 返回 JC_MS__NULL_BUFFER；ch 为 NULL 返回 JC_MS__NULL_FOLLOWER；ch 指向 ms->buffer 内部时返回 JC_MS__INVALID_SOURCE；ch 为空串时返回成功但不修改；容量不足时自动扩容
```
> int jcms_reverse(jc_MutableString ms)
```text
此为库对外接口，返回 int 类型的状态码，原地反转 ms 的内容
注意：ms 为 NULL、缓冲区为 NULL 或内容为空串时直接返回 JC_MS__SUCCESSFUL_OP（不做任何修改）
```
> int jcms_split(const JC_MutableString* const ms, char delimiter, jc_MutableString** outArray, size_t* count, bool removeEmptyEntries)
```text
此为库对外接口，返回 int 类型的状态码，按分隔符 delimiter 将 ms 分割为若干子串实例，通过 outArray 输出数组、count 输出元素个数
注意：outArray 与 count 必须非 NULL；removeEmptyEntries 为 true 时会丢弃空子串；调用者负责释放 outArray 中每个实例（jcms_delete）以及 outArray 本身（free）；若部分元素构造失败，返回 JC_MS__PARTIAL_SPLIT，对应位置可能是 NULL 或状态异常的实例；若所有段都被过滤，返回 JC_MS__SUCCESSFUL_OP 且 *outArray 为 NULL、*count 为 0
```
> int jcms_trim(const jc_MutableString ms, JC_SpaceMode mode)
```text
此为库对外接口，返回 int 类型的状态码，原地去除 ms 首尾的空白字符
注意：空白字符判定受 mode 控制——JC_SPACE_ASCII 时使用固定 ASCII 空白字符（空格、\t、\n、\r、\v、\f），JC_SPACE_LOCALE 或未知模式时跟随当前 locale 的 isspace；ms 为 NULL、缓冲区为 NULL 或内容为空时直接返回 JC_MS__SUCCESSFUL_OP；若全为空白则将内容清空、len 置 0
```
> int jcms_trimFront(const jc_MutableString ms, JC_SpaceMode mode)
```text
此为库对外接口，返回 int 类型的状态码，原地去除 ms 首部的空白字符
注意：mode 语义同 jcms_trim；边界处理同 jcms_trim
```
> int jcms_trimBack(const jc_MutableString ms, JC_SpaceMode mode)
```text
此为库对外接口，返回 int 类型的状态码，原地去除 ms 尾部的空白字符
注意：mode 语义同 jcms_trim；边界处理同 jcms_trim
```
> jc_MutableString jcms_clone(const JC_MutableString* const ms)
```text
此为库对外接口，返回一个与 ms 内容相同的深拷贝新实例
注意：ms 为 NULL 时返回 NULL；ms 缓冲区为 NULL 时返回状态为 JC_MS__NULL_BUFFER 的新实例；返回的实例需通过 jcms_delete 释放
```
> int jcms_charAt(const JC_MutableString* const ms, size_t index, char* result)
```text
此为库对外接口，返回 int 类型的状态码，取 ms 中下标为 index 的字符写入 result
注意：index 从 0 开始；result 为 NULL 返回 JC_MS__NULL_CHAR；ms 为 NULL 返回 JC_MS__NULL；缓冲区为 NULL 返回 JC_MS__NULL_BUFFER；index 越界返回 JC_MS__INDEX_OUT_OF_BOUNDS；成功返回 JC_MS__SUCCESSFUL_OP
```
> int jcms_indexOfFront(const JC_MutableString* const ms, char ch, size_t* result)
```text
此为库对外接口，返回 int 类型的状态码，从前往后查找字符 ch 首次出现的位置写入 result
注意：result 为 NULL 返回 JC_MS__NULL_RESULT；ms 为 NULL 返回 JC_MS__NULL；缓冲区为 NULL 返回 JC_MS__NULL_BUFFER；未找到返回 JC_MS__WITHOUT_CHAR 且不修改 *result；找到返回 JC_MS__SUCCESSFUL_OP
```
> int jcms_indexOfBack(const JC_MutableString* const ms, char ch, size_t* result)
```text
此为库对外接口，返回 int 类型的状态码，从后往前查找字符 ch 首次出现的位置写入 result
注意：result 为 NULL 返回 JC_MS__NULL_RESULT；ms 为 NULL 返回 JC_MS__NULL；缓冲区为 NULL 返回 JC_MS__NULL_BUFFER；未找到返回 JC_MS__WITHOUT_CHAR 且不修改 *result；找到返回 JC_MS__SUCCESSFUL_OP
```
> int jcms_indexOfFrontWithInst(const JC_MutableString* const ms, const JC_MutableString* const sub, size_t* result)
```text
此为库对外接口，返回 int 类型的状态码，从前往后查找子串实例 sub 首次出现的位置写入 result
注意：result 为 NULL 返回 JC_MS__NULL_RESULT；任一参数为 NULL 返回 JC_MS__NULL；任一缓冲区为 NULL 返回 JC_MS__NULL_BUFFER；sub 为空串时 *result 被置为 ms 的长度并返回 JC_MS__SUCCESSFUL_OP；未找到返回 JC_MS__WITHOUT_CHAR
```
> int jcms_indexOfFrontWithCh(const JC_MutableString* const ms, const char* const ch, size_t* result)
```text
此为库对外接口，返回 int 类型的状态码，从前往后查找 C 字符串 ch 首次出现的位置写入 result
注意：result 为 NULL 返回 JC_MS__NULL_RESULT；ms 为 NULL 返回 JC_MS__NULL；缓冲区或 ch 为 NULL 返回 JC_MS__NULL_BUFFER；ch 为空串时 *result 被置为 ms 的长度并返回 JC_MS__SUCCESSFUL_OP；未找到返回 JC_MS__WITHOUT_CHAR
```
> jc_MutableString jcms_subInLengthFrontWithInst(const JC_MutableString* const ms, size_t start, size_t length)
```text
此为库对外接口，返回一个取 ms 从 start 开始、长度为 length 的新可变字符串实例
注意：ms 为 NULL 返回 NULL；缓冲区为 NULL 返回状态为 NULL_BUFFER 的新实例；ms 为空或 length 为 0 时返回空串实例；start 越界时返回状态为 JC_MS__INVALID_START 的空串实例；length 超过剩余长度时自动截断；返回的实例需通过 jcms_delete 释放
```
> jc_MutableString jcms_subInLengthBackWithInst(const JC_MutableString* const ms, size_t end, size_t length)
```text
此为库对外接口，返回一个取 ms 以 end 为末位、向前长度为 length 的新可变字符串实例
注意：ms 为 NULL 返回 NULL；缓冲区为 NULL 返回状态为 NULL_BUFFER 的新实例；ms 为空或 length 为 0 时返回空串实例；end 越界时返回状态为 JC_MS__INVALID_END 的空串实例；length 超过 end+1 时自动截断到开头；返回的实例需通过 jcms_delete 释放
```
> jc_MutableString jcms_subInRangeWithInst(const JC_MutableString* const ms, size_t start, size_t end)
```text
此为库对外接口，返回一个取 ms 区间 [start, end) 的新可变字符串实例
注意：ms 为 NULL 返回 NULL；缓冲区为 NULL 返回状态为 NULL_BUFFER 的新实例；ms 为空或 start == end 时返回空串实例；start > end 时返回状态为 JC_MS__INVALID_INDEX 的空串实例；start 或 end 越界时返回状态为 JC_MS__INDEX_OUT_OF_BOUNDS 的空串实例；返回的实例需通过 jcms_delete 释放
```
> jc_MutableString jcms_subInLengthFrontWithCh(const char* const ch, size_t start, size_t length)
```text
此为库对外接口，返回一个取 C 字符串 ch 从 start 开始、长度为 length 的新可变字符串实例
注意：ch 为 NULL 时返回状态为 JC_MS__NULL_BUFFER 的实例；ch 为空或 length 为 0 时返回空串实例；start 越界时返回状态为 JC_MS__INVALID_START 的空串实例；length 超过剩余长度时自动截断；返回的实例需通过 jcms_delete 释放
```
> jc_MutableString jcms_subInLengthBackWithCh(const char* const ch, size_t end, size_t length)
```text
此为库对外接口，返回一个取 C 字符串 ch 以 end 为末位、向前长度为 length 的新可变字符串实例
注意：ch 为 NULL 时返回状态为 JC_MS__NULL_BUFFER 的实例；ch 为空或 length 为 0 时返回空串实例；end 越界时返回状态为 JC_MS__INVALID_END 的空串实例；length 超过 end+1 时自动截断到开头；返回的实例需通过 jcms_delete 释放
```
> jc_MutableString jcms_subInRangeWithCh(const char* const ch, size_t start, size_t end)
```text
此为库对外接口，返回一个取 C 字符串 ch 区间 [start, end) 的新可变字符串实例
注意：ch 为 NULL 时返回状态为 JC_MS__NULL_BUFFER 的实例；ch 为空或 start == end 时返回空串实例；start > end 时返回状态为 JC_MS__INVALID_INDEX 的空串实例；越界时返回状态为 JC_MS__INDEX_OUT_OF_BOUNDS 的空串实例；返回的实例需通过 jcms_delete 释放
```
> int jcms_replaceFirstWithInst(JC_MutableString* ms, const char* const target, const JC_MutableString* const replacement)
```text
此为库对外接口，返回 int 类型的状态码，原地将 ms 中 target 第一次出现替换为 replacement 的内容
注意：ms 为 NULL 返回 JC_MS__NULL；状态异常返回 ms->status；target 为 NULL 返回 JC_MS__NULL_TARGET；replacement 为 NULL 或缓冲区为 NULL 返回 JC_MS__INVALID_REPLACEMENT；replacement 指向 ms->buffer 内部时返回 JC_MS__INVALID_REPLACEMENT（防止 realloc 悬空）；空 target 的语义：空内容 + 空 target 时整体设为 replacement，空内容 + 非空 target 返回 NO_TARGET_MATCHED，非空内容 + 空 target 时末尾追加；非空 target 未找到返回 JC_MS__NO_TARGET_MATCHED
```
> int jcms_replaceFirstWithCh(JC_MutableString* ms, const char* const target, const char* const replacement)
```text
此为库对外接口，返回 int 类型的状态码，原地将 ms 中 target 第一次出现替换为 C 字符串 replacement 的内容
注意：ms 为 NULL 返回 JC_MS__NULL；状态异常返回 ms->status；target 为 NULL 返回 JC_MS__NULL_TARGET；replacement 为 NULL 返回 JC_MS__INVALID_REPLACEMENT；空 target 语义同 jcms_replaceFirstWithInst；非空 target 未找到返回 JC_MS__NO_TARGET_MATCHED
```
> int jcms_replaceAllWithInst(JC_MutableString* ms, const char* const target, const JC_MutableString* const replacement)
```text
此为库对外接口，返回 int 类型的状态码，原地将 ms 中 target 所有非重叠出现替换为 replacement 的内容
注意：ms 为 NULL 返回 JC_MS__NULL；状态异常返回 ms->status；target 为 NULL 返回 JC_MS__NULL_TARGET；replacement 为 NULL 或缓冲区为 NULL 返回 JC_MS__INVALID_REPLACEMENT；replacement 指向 ms->buffer 内部时返回 JC_MS__INVALID_REPLACEMENT；空 target 语义同 jcms_replaceFirstWithInst；非空 target 未找到返回 JC_MS__NO_TARGET_MATCHED
```
> int jcms_replaceAllWithCh(JC_MutableString* ms, const char* const target, const char* const replacement)
```text
此为库对外接口，返回 int 类型的状态码，原地将 ms 中 target 所有非重叠出现替换为 C 字符串 replacement 的内容
注意：ms 为 NULL 返回 JC_MS__NULL；状态异常返回 ms->status；target 为 NULL 返回 JC_MS__NULL_TARGET；replacement 为 NULL 返回 JC_MS__INVALID_REPLACEMENT；空 target 语义同 jcms_replaceFirstWithInst；非空 target 未找到返回 JC_MS__NO_TARGET_MATCHED
```
> bool jcms_startWithCh(const JC_MutableString* const ms, const char* const prefix)
```text
此为库对外接口，返回 bool 类型结果，判断 ms 是否以 C 字符串 prefix 开头
注意：ms 为 NULL 或缓冲区为 NULL 时返回 false；prefix 为 NULL 时返回 false；prefix 为空串时返回 true
```
> bool jcms_startWithInst(const JC_MutableString* const ms, const JC_MutableString* const prefix)
```text
此为库对外接口，返回 bool 类型结果，判断 ms 是否以字符串实例 prefix 开头
注意：prefix 为 NULL 时返回 false；其余边界与 jcms_startWithCh 一致
```
> bool jcms_endWithCh(const JC_MutableString* const ms, const char* const suffix)
```text
此为库对外接口，返回 bool 类型结果，判断 ms 是否以 C 字符串 suffix 结尾
注意：ms 为 NULL 或缓冲区为 NULL 时返回 false；suffix 为 NULL 时返回 false；suffix 为空串时返回 true
```
> bool jcms_endWithInst(const JC_MutableString* const ms, const JC_MutableString* const suffix)
```text
此为库对外接口，返回 bool 类型结果，判断 ms 是否以字符串实例 suffix 结尾
注意：suffix 为 NULL 时返回 false；其余边界与 jcms_endWithCh 一致
```
> bool jcms_isEmpty(const JC_MutableString* const ms)
```text
此为库对外接口，返回 bool 类型结果，判断 ms 是否为空串
注意：ms 为 NULL、缓冲区为 NULL、或内容为空串时均返回 true
```
> bool jcms_isBlank(const JC_MutableString* const ms, JC_SpaceMode mode)
```text
此为库对外接口，返回 bool 类型结果，判断 ms 是否为空串或仅由空白字符组成
注意：空白字符判定受 mode 控制——JC_SPACE_ASCII 时使用固定 ASCII 空白字符（空格、\t、\n、\r、\v、\f），JC_SPACE_LOCALE 或未知模式时跟随当前 locale 的 isspace；ms 为 NULL 或缓冲区为 NULL 时返回 true
```
> bool jcms_equalsWithInst(const JC_MutableString* const ms1, const JC_MutableString* const ms2)
```text
此为库对外接口，返回 bool 类型结果，判断两个可变字符串实例是否相等
注意：两个参数均为 NULL 时返回 true；仅一个为 NULL 时返回 false；两个实例缓冲区均为 NULL 时返回 true；仅一个缓冲区为 NULL 时返回 false；两个均为空串时返回 true；仅一个为空串时返回 false；否则按内容比较
```
> bool jcms_equalsIgnoreCaseWithInst(const JC_MutableString* const ms1, const JC_MutableString* const ms2, JC_SpaceMode mode)
```text
此为库对外接口，返回 bool 类型结果，判断两个可变字符串实例是否在忽略大小写后相等
注意：mode 语义同 jcms_isBlank，决定大小写转换使用固定 ASCII 还是跟随 locale；NULL/缓冲区/空串的判定同 jcms_equalsWithInst；内容比较前先比长度
```
> bool jcms_equalsWithCh(const JC_MutableString* const ms, const char* const ch)
```text
此为库对外接口，返回 bool 类型结果，判断 ms 内容与 C 字符串 ch 是否相等
注意：ms 为 NULL 返回 false；ms 缓冲区与 ch 同时为 NULL 返回 true；仅一方为 NULL 返回 false；两者均为空串返回 true；仅一方为空串返回 false；否则按内容比较
```
> bool jcms_equalsIgnoreCaseWithCh(const JC_MutableString* const ms, const char* const ch, JC_SpaceMode mode)
```text
此为库对外接口，返回 bool 类型结果，判断 ms 内容与 C 字符串 ch 是否在忽略大小写后相等
注意：mode 语义同 jcms_isBlank；NULL/缓冲区/空串的判定同 jcms_equalsWithCh；内容比较前先比长度
```
> bool jcms_containsWithInst(const JC_MutableString* const ms1, const JC_MutableString* const ms2)
```text
此为库对外接口，返回 bool 类型结果，判断 ms1 是否包含子串 ms2
注意：两个参数均为 NULL 时返回 true；仅一个为 NULL 时返回 false；两个实例缓冲区均为 NULL 时返回 true；仅一个缓冲区为 NULL 时返回 false；其余情况交给 strstr，空串 needle 视为任何串的子串
```
> bool jcms_containsIgnoreCaseWithInst(const JC_MutableString* const ms1, const JC_MutableString* const ms2, JC_SpaceMode mode)
```text
此为库对外接口，返回 bool 类型结果，判断 ms1 是否在忽略大小写后包含子串 ms2
注意：mode 语义同 jcms_isBlank；NULL/缓冲区判定同 jcms_containsWithInst；空串 needle 视为任何串的子串
```
> bool jcms_containsWithCh(const JC_MutableString* const ms, const char* const ch)
```text
此为库对外接口，返回 bool 类型结果，判断 ms 是否包含 C 字符串 ch
注意：ms 为 NULL 返回 false；ms 缓冲区与 ch 同时为 NULL 返回 true；仅一方为 NULL 返回 false；其余情况交给 strstr，空串 ch 视为任何串的子串
```
> bool jcms_containsIgnoreCaseWithCh(const JC_MutableString* const ms, const char* const ch, JC_SpaceMode mode)
```text
此为库对外接口，返回 bool 类型结果，判断 ms 是否在忽略大小写后包含 C 字符串 ch
注意：mode 语义同 jcms_isBlank；NULL/缓冲区判定同 jcms_containsWithCh；空串 ch 视为任何串的子串
```
> int jcms_compareWithInst(const JC_MutableString* const ms1, const JC_MutableString* const ms2, int* result)
```text
此为库对外接口，返回 int 类型的状态码，按字典序比较两个可变字符串实例，将比较结果写入 result
注意：result 为 NULL 返回 JC_MS__NULL_RECEIVER；两个参数均为 NULL 时 *result 被置为 0 并返回 JC_MS__SUCCESSFUL_OP；仅一个为 NULL 时返回 JC_MS__NULL；两个实例缓冲区均为 NULL 时 *result 被置为 0 并返回 JC_MS__SUCCESSFUL_OP；仅一个缓冲区为 NULL 时返回 JC_MS__NULL_BUFFER；其余按 strcmp 结果写入 *result 并返回 JC_MS__SUCCESSFUL_OP
```
> int jcms_compareIgnoreCaseWithInst(const JC_MutableString* const ms1, const JC_MutableString* const ms2, int* result, JC_SpaceMode mode)
```text
此为库对外接口，返回 int 类型的状态码，按字典序在忽略大小写后比较两个可变字符串实例，将比较结果写入 result
注意：mode 语义同 jcms_isBlank；NULL/缓冲区判定同 jcms_compareWithInst；其余按忽略大小写比较的结果写入 *result 并返回 JC_MS__SUCCESSFUL_OP
```
> int jcms_compareWithCh(const JC_MutableString* const ms, const char* const ch, int* result)
```text
此为库对外接口，返回 int 类型的状态码，按字典序比较 ms 与 C 字符串 ch，将比较结果写入 result
注意：result 为 NULL 返回 JC_MS__NULL_RECEIVER；ms 为 NULL 返回 JC_MS__NULL；ms 缓冲区与 ch 同时为 NULL 时 *result 被置为 0 并返回 JC_MS__SUCCESSFUL_OP；仅一方为 NULL 返回 JC_MS__NULL_BUFFER；其余按 strcmp 结果写入 *result 并返回 JC_MS__SUCCESSFUL_OP
```
> int jcms_compareIgnoreCaseWithCh(const JC_MutableString* const ms, const char* const ch, int* result, JC_SpaceMode mode)
```text
此为库对外接口，返回 int 类型的状态码，按字典序在忽略大小写后比较 ms 与 C 字符串 ch，将比较结果写入 result
注意：mode 语义同 jcms_isBlank；NULL/缓冲区判定同 jcms_compareWithCh；其余按忽略大小写比较的结果写入 *result 并返回 JC_MS__SUCCESSFUL_OP
```
> char* jcms_toCharArray(const JC_MutableString* const ms)
```text
此为库对外接口，返回一个新分配的 C 字符串副本，内容与 ms 相同
注意：调用者负责 free 返回值；ms 为 NULL 或缓冲区为 NULL 时返回 NULL；空串会返回一个可 free 的非 NULL 空串
```
> int jcms_toInt(const JC_MutableString* const ms, int* result, JC_SpaceMode mode)
```text
此为库对外接口，返回 int 类型的状态码，将 ms 按十进制解析为 int 写入 result
注意：mode 控制前后空白跳过使用固定 ASCII 还是跟随 locale；允许正负号；空串或缓冲区为 NULL 返回 JC_MS__EMPTY；实例状态异常返回 ms->status；result 为 NULL 返回 JC_MS__NULL_INT；格式非法返回 JC_MS__NOT_INT；溢出返回 JC_MS__OVERFLOW_INT；成功返回 JC_MS__SUCCESSFUL_OP
```
> int jcms_toLong(const JC_MutableString* const ms, long* result, JC_SpaceMode mode)
```text
此为库对外接口，返回 int 类型的状态码，将 ms 按十进制解析为 long 写入 result
注意：mode 语义同 jcms_toInt；空串或缓冲区为 NULL 返回 JC_MS__EMPTY；实例状态异常返回 ms->status；result 为 NULL 返回 JC_MS__NULL_LONG；格式非法返回 JC_MS__NOT_LONG；溢出返回 JC_MS__OVERFLOW_LONG；成功返回 JC_MS__SUCCESSFUL_OP
```
> int jcms_toLongLong(const JC_MutableString* const ms, long long* result, JC_SpaceMode mode)
```text
此为库对外接口，返回 int 类型的状态码，将 ms 按十进制解析为 long long 写入 result
注意：mode 语义同 jcms_toInt；空串或缓冲区为 NULL 返回 JC_MS__EMPTY；实例状态异常返回 ms->status；result 为 NULL 返回 JC_MS__NULL_LONGLONG；格式非法返回 JC_MS__NOT_LONGLONG；溢出返回 JC_MS__OVERFLOW_LONGLONG；成功返回 JC_MS__SUCCESSFUL_OP
```
> int jcms_toFloat(const JC_MutableString* const ms, float* result, JC_SpaceMode mode)
```text
此为库对外接口，返回 int 类型的状态码，将 ms 解析为 float 写入 result
注意：mode 语义同 jcms_toInt；底层使用 strtod，支持小数、指数、以及 C 标准允许的格式（包括十六进制浮点与 inf/nan）；空串或缓冲区为 NULL 返回 JC_MS__EMPTY；实例状态异常返回 ms->status；result 为 NULL 返回 JC_MS__NULL_FLOAT；格式非法返回 JC_MS__NOT_FLOAT；超出 float 范围返回 JC_MS__OVERFLOW_FLOAT；成功返回 JC_MS__SUCCESSFUL_OP
```
> int jcms_toDouble(const JC_MutableString* const ms, double* result, JC_SpaceMode mode)
```text
此为库对外接口，返回 int 类型的状态码，将 ms 解析为 double 写入 result
注意：mode 语义同 jcms_toInt；底层使用 strtod，支持小数、指数、以及 C 标准允许的格式（包括十六进制浮点与 inf/nan）；空串或缓冲区为 NULL 返回 JC_MS__EMPTY；实例状态异常返回 ms->status；result 为 NULL 返回 JC_MS__NULL_DOUBLE；格式非法返回 JC_MS__NOT_DOUBLE；超出 double 范围返回 JC_MS__OVERFLOW_DOUBLE；成功返回 JC_MS__SUCCESSFUL_OP
```
> int jcms_toLowerCase(JC_MutableString* ms, JC_SpaceMode mode)
```text
此为库对外接口，返回 int 类型的状态码，原地将 ms 内容转换为小写
注意：mode 语义同 jcms_isBlank，决定大小写转换使用固定 ASCII 还是跟随 locale；ms 为 NULL 返回 JC_MS__NULL；缓冲区为 NULL 返回 JC_MS__NULL_BUFFER；空串不做修改直接返回成功；成功返回 JC_MS__SUCCESSFUL_OP
```
> int jcms_toUpperCase(JC_MutableString* ms, JC_SpaceMode mode)
```text
此为库对外接口，返回 int 类型的状态码，原地将 ms 内容转换为大写
注意：mode 语义同 jcms_isBlank，决定大小写转换使用固定 ASCII 还是跟随 locale；ms 为 NULL 返回 JC_MS__NULL；缓冲区为 NULL 返回 JC_MS__NULL_BUFFER；空串不做修改直接返回成功；成功返回 JC_MS__SUCCESSFUL_OP
```
> jc_String jcms_toString(const JC_MutableString* const ms)
```text
此为库对外接口，返回一个内容与 ms 相同的新不可变字符串实例
注意：ms 为 NULL 时返回一个状态为 JC_STR__NULL_BUFFER 的不可变字符串实例；ms 缓冲区为 NULL 时也按此处理；返回的实例需通过 jcstr_delete 释放
```