# String
## 其他语言md
### English
[String.md English version](String_EN.md)
## 接口介绍
> jc_String jcstr_newFrom(const char* const content)
```text
此为库对外接口，从 C 字符串内容创建一个字符串实例
注意：若 content 为 NULL，返回的实例状态为 JC_STR__NULL_BUFFER；若内存分配失败，也可能返回状态为 JC_STR__NULL_BUFFER 的实例；若连实例本身分配失败则返回 NULL；返回的实例需通过 jcstr_delete 释放
```
> void jcstr_delete(jc_String str)
```text
此为库对外接口，无返回值，释放传入的字符串实例及其内部缓冲区
注意：传入 NULL 不会崩溃；但请不要传入强转为 “jc_String” 的非 jc_String 变量
```
> int jcstr_getStatus(const JC_String* const str)
```text
此为库对外接口，返回 int 类型的状态码，需要传入一个字符串实例
注意：当 str 为 NULL 时返回 JC_STR__NULL；否则返回实例当前状态
```
> size_t jcstr_getLength(const JC_String* const str)
```text
此为库对外接口，返回 size_t 类型的字符串长度（不含结尾 '\0'），需要传入一个字符串实例
注意：当 str 为 NULL 或缓冲区为 NULL 时返回 0；否则返回实例缓存的长度
```
> const char* jcstr_statusToCharArray(const int status)
```text
此为库对外接口，返回 int 类型状态码对应的可读字符串描述
注意：返回的是静态字符串，调用者无需也不应释放
```
> jc_String jcstr_appendWithInst(const JC_String* const str1, const JC_String* const str2)
```text
此为库对外接口，返回一个新字符串实例，内容为 str1 后拼接 str2，原实例不变
注意：若 str1 为 NULL 则返回 NULL；若 str1 缓冲区无效则返回状态为 JC_STR__NULL_BUFFER 的实例；若 str2 为 NULL 则返回 str1 拷贝并携带状态 JC_STR__INVALID_FOLLOWER；若 str2 缓冲区无效则返回 str1 拷贝并携带状态 JC_STR__INVALID_FOLLOWER_BUFFER；返回的实例需通过 jcstr_delete 释放
```
> jc_String jcstr_appendWithCh(const JC_String* const str, const char* const content)
```text
此为库对外接口，返回一个新字符串实例，内容为 str 后拼接 C 字符串 content，原实例不变
注意：若 str 为 NULL 则返回 NULL；若 str 缓冲区无效则返回状态为 JC_STR__NULL_BUFFER 的实例；若 content 为 NULL 则返回 str 拷贝并携带状态 JC_STR__INVALID_FOLLOWER；返回的实例需通过 jcstr_delete 释放
```
> jc_String jcstr_reverse(const JC_String* const str)
```text
此为库对外接口，返回一个内容为 str 反转后的新字符串实例，原实例不变
注意：str 为 NULL 时返回 NULL；str 缓冲区无效时返回状态为 JC_STR__NULL_BUFFER 的实例；空串返回空串实例；返回的实例需通过 jcstr_delete 释放
```
> int jcstr_split(const JC_String* const str, char delimiter, jc_String** outArray, size_t* count, bool removeEmptyEntries)
```text
此为库对外接口，返回 int 类型的状态码，按分隔符 delimiter 将 str 分割为若干子串实例，通过 outArray 输出数组、count 输出元素个数
注意：outArray 与 count 必须非 NULL；removeEmptyEntries 为 true 时会丢弃空子串；调用者负责释放 outArray 中每个实例（jcstr_delete）以及 outArray 本身（free）；若部分元素构造失败，返回 JC_STR__PARTIAL_SPLIT，对应位置可能是状态异常的实例；若所有段都被过滤，返回 JC_STR__SUCCESSFUL_OP 且 *outArray 为 NULL、*count 为 0
```
> jc_String jcstr_trim(const JC_String* const str, JC_SpaceMode mode)
```text
此为库对外接口，返回一个去除首尾空白字符后的新字符串实例，原实例不变
注意：空白字符判定受 mode 控制——JC_SPACE_ASCII 时使用固定 ASCII 空白字符（空格、\t、\n、\r、\v、\f），JC_SPACE_LOCALE 或未知模式时跟随当前 locale 的 isspace；若全为空白则返回空字符串实例；返回的实例需通过 jcstr_delete 释放
```
> jc_String jcstr_trimFront(const JC_String* const str, JC_SpaceMode mode)
```text
此为库对外接口，返回一个去除首部空白字符后的新字符串实例，原实例不变
注意：mode 语义同 jcstr_trim；若全为空白则返回空字符串实例；返回的实例需通过 jcstr_delete 释放
```
> jc_String jcstr_trimBack(const JC_String* const str, JC_SpaceMode mode)
```text
此为库对外接口，返回一个去除尾部空白字符后的新字符串实例，原实例不变
注意：mode 语义同 jcstr_trim；若全为空白则返回空字符串实例；返回的实例需通过 jcstr_delete 释放
```
> jc_String jcstr_clone(const JC_String* const str)
```text
此为库对外接口，返回一个与 str 内容相同的深拷贝新实例，原实例不变
注意：str 为 NULL 时返回 NULL；str 缓冲区无效时返回状态为 JC_STR__NULL_BUFFER 的实例；返回的实例需通过 jcstr_delete 释放
```
> int jcstr_charAt(const JC_String* const str, size_t index, char* result)
```text
此为库对外接口，返回 int 类型的状态码，取 str 中下标为 index 的字符写入 result
注意：index 从 0 开始；result 为 NULL 返回 JC_STR__NULL_CHAR；str 为 NULL 返回 JC_STR__NULL；缓冲区为 NULL 返回 JC_STR__NULL_BUFFER；index 越界返回 JC_STR__IDX_OUT_BOUNDS；成功返回 JC_STR__SUCCESSFUL_OP
```
> int jcstr_indexOfFront(const JC_String* const str, char ch, size_t* result)
```text
此为库对外接口，返回 int 类型的状态码，从前往后查找字符 ch 首次出现的位置写入 result
注意：result 为 NULL 返回 JC_STR__NULL_RECEIVER；未找到返回 JC_STR__WITHOUT_CH 且不修改 *result；找到返回 JC_STR__SUCCESSFUL_OP
```
> int jcstr_indexOfBack(const JC_String* const str, char ch, size_t* result)
```text
此为库对外接口，返回 int 类型的状态码，从后往前查找字符 ch 首次出现的位置写入 result
注意：result 为 NULL 返回 JC_STR__NULL_RECEIVER；未找到返回 JC_STR__WITHOUT_CH 且不修改 *result；找到返回 JC_STR__SUCCESSFUL_OP
```
> int jcstr_indexOfFrontWithInst(const JC_String* const str, const JC_String* const sub, size_t* result)
```text
此为库对外接口，返回 int 类型的状态码，从前往后查找子串实例 sub 首次出现的位置写入 result
注意：result 为 NULL 返回 JC_STR__NULL_RECEIVER；任一参数为 NULL 或缓冲区为 NULL 时返回对应错误码；sub 为空串时 *result 被置为 str 的长度并返回 JC_STR__SUCCESSFUL_OP；未找到返回 JC_STR__WITHOUT_CH
```
> int jcstr_indexOfFrontWithCh(const JC_String* const str, const char* const ch, size_t* result)
```text
此为库对外接口，返回 int 类型的状态码，从前往后查找 C 字符串 ch 首次出现的位置写入 result
注意：result 为 NULL 返回 JC_STR__NULL_RECEIVER；str 为 NULL 返回 JC_STR__NULL；缓冲区或 ch 为 NULL 返回 JC_STR__NULL_BUFFER；ch 为空串时 *result 被置为 str 的长度并返回 JC_STR__SUCCESSFUL_OP；未找到返回 JC_STR__WITHOUT_CH
```
> jc_String jcstr_subInLengthFrontWithInst(const JC_String* const str, size_t start, size_t length)
```text
此为库对外接口，返回一个取 str 从 start 开始、长度为 length 的新字符串实例，原实例不变
注意：start 越界时返回状态为 JC_STR__INVALID_START 的空串实例；length 超过剩余长度时自动截断；空串或 length 为 0 时返回空串实例；返回的实例需通过 jcstr_delete 释放
```
> jc_String jcstr_subInLengthBackWithInst(const JC_String* const str, size_t end, size_t length)
```text
此为库对外接口，返回一个取 str 以 end 为末位、向前长度为 length 的新字符串实例，原实例不变
注意：end 越界时返回状态为 JC_STR__INVALID_END 的空串实例；length 超过 end+1 时自动截断到开头；空串或 length 为 0 时返回空串实例；返回的实例需通过 jcstr_delete 释放
```
> jc_String jcstr_subInRangeWithInst(const JC_String* const str, size_t start, size_t end)
```text
此为库对外接口，返回一个取 str 区间 [start, end) 的新字符串实例，原实例不变
注意：start > end 时返回状态为 JC_STR__INVALID_IDX 的空串实例；start 或 end 越界时返回状态为 JC_STR__IDX_OUT_BOUNDS 的空串实例；start == end 时返回空串实例；返回的实例需通过 jcstr_delete 释放
```
> jc_String jcstr_subInLengthFrontWithCh(const char* const ch, size_t start, size_t length)
```text
此为库对外接口，返回一个取 C 字符串 ch 从 start 开始、长度为 length 的新字符串实例
注意：ch 为 NULL 时返回状态为 JC_STR__NULL_BUFFER 的实例；start 越界时返回状态为 JC_STR__INVALID_START 的空串实例；length 超过剩余长度时自动截断；返回的实例需通过 jcstr_delete 释放
```
> jc_String jcstr_subInLengthBackWithCh(const char* const ch, size_t end, size_t length)
```text
此为库对外接口，返回一个取 C 字符串 ch 以 end 为末位、向前长度为 length 的新字符串实例
注意：ch 为 NULL 时返回状态为 JC_STR__NULL_BUFFER 的实例；end 越界时返回状态为 JC_STR__INVALID_END 的空串实例；length 超过 end+1 时自动截断到开头；返回的实例需通过 jcstr_delete 释放
```
> jc_String jcstr_subInRangeWithCh(const char* const ch, size_t start, size_t end)
```text
此为库对外接口，返回一个取 C 字符串 ch 区间 [start, end) 的新字符串实例
注意：ch 为 NULL 时返回状态为 JC_STR__NULL_BUFFER 的实例；start > end 时返回状态为 JC_STR__INVALID_IDX 的空串实例；越界时返回状态为 JC_STR__IDX_OUT_BOUNDS 的空串实例；返回的实例需通过 jcstr_delete 释放
```
> jc_String jcstr_replaceFirstWithInst(const JC_String* const str, const char* const target, const JC_String* const replacement)
```text
此为库对外接口，返回一个将 str 中 target 第一次出现替换为 replacement 内容的新字符串实例，原实例不变
注意：str 为 NULL 返回 NULL；str 缓冲区无效返回状态为 JC_STR__NULL_BUFFER 的实例；target 为 NULL 返回原内容副本且状态为 JC_STR__NULL_TARGET；replacement 为 NULL 或缓冲区无效返回原内容副本且状态为 JC_STR__INVALID_REPLACEMENT；target 为空串且 str 非空时等价于末尾追加 replacement，target 为空串且 str 为空时等价于整体设为 replacement；非空 target 在 str 为空或未找到时返回原内容副本；返回的实例需通过 jcstr_delete 释放
```
> jc_String jcstr_replaceAllWithInst(const JC_String* const str, const char* const target, const JC_String* const replacement)
```text
此为库对外接口，返回一个将 str 中 target 所有出现替换为 replacement 内容的新字符串实例，原实例不变
注意：str 为 NULL 返回 NULL；str 缓冲区无效返回状态为 JC_STR__NULL_BUFFER 的实例；target 为 NULL 返回原内容副本且状态为 JC_STR__NULL_TARGET；replacement 为 NULL 或缓冲区无效返回原内容副本且状态为 JC_STR__INVALID_REPLACEMENT；空 target 的语义同 jcstr_replaceFirstWithInst；非空 target 在 str 为空或未找到时返回原内容副本；返回的实例需通过 jcstr_delete 释放
```
> jc_String jcstr_replaceFirstWithCh(const JC_String* const str, const char* const target, const char* const replacement)
```text
此为库对外接口，返回一个将 str 中 target 第一次出现替换为 C 字符串 replacement 的新字符串实例，原实例不变
注意：str 为 NULL 返回 NULL；str 缓冲区无效返回状态为 JC_STR__NULL_BUFFER 的实例；target 为 NULL 返回原内容副本且状态为 JC_STR__NULL_TARGET；replacement 为 NULL 返回原内容副本且状态为 JC_STR__INVALID_REPLACEMENT；空 target 的语义同 jcstr_replaceFirstWithInst；非空 target 在 str 为空或未找到时返回原内容副本；返回的实例需通过 jcstr_delete 释放
```
> jc_String jcstr_replaceAllWithCh(const JC_String* const str, const char* const target, const char* const replacement)
```text
此为库对外接口，返回一个将 str 中 target 所有出现替换为 C 字符串 replacement 的新字符串实例，原实例不变
注意：str 为 NULL 返回 NULL；str 缓冲区无效返回状态为 JC_STR__NULL_BUFFER 的实例；target 为 NULL 返回原内容副本且状态为 JC_STR__NULL_TARGET；replacement 为 NULL 返回原内容副本且状态为 JC_STR__INVALID_REPLACEMENT；空 target 的语义同 jcstr_replaceAllWithInst；非空 target 在 str 为空或未找到时返回原内容副本；返回的实例需通过 jcstr_delete 释放
```
> bool jcstr_startWithCh(const JC_String* const str, const char* const prefix)
```text
此为库对外接口，返回 bool 类型结果，判断 str 是否以 C 字符串 prefix 开头
注意：str 为 NULL 或缓冲区为 NULL 时返回 false；prefix 为 NULL 时返回 false；prefix 为空串时返回 true
```
> bool jcstr_endWithCh(const JC_String* const str, const char* const suffix)
```text
此为库对外接口，返回 bool 类型结果，判断 str 是否以 C 字符串 suffix 结尾
注意：str 为 NULL 或缓冲区为 NULL 时返回 false；suffix 为 NULL 时返回 false；suffix 为空串时返回 true
```
> bool jcstr_startWithInst(const JC_String* const str, const JC_String* const prefix)
```text
此为库对外接口，返回 bool 类型结果，判断 str 是否以字符串实例 prefix 开头
注意：prefix 为 NULL 时返回 false；其余边界与 jcstr_startWithCh 一致
```
> bool jcstr_endWithInst(const JC_String* const str, const JC_String* const suffix)
```text
此为库对外接口，返回 bool 类型结果，判断 str 是否以字符串实例 suffix 结尾
注意：suffix 为 NULL 时返回 false；其余边界与 jcstr_endWithCh 一致
```
> bool jcstr_isEmpty(const JC_String* const str)
```text
此为库对外接口，返回 bool 类型结果，判断 str 是否为空串
注意：str 为 NULL、缓冲区为 NULL、或内容为空串时均返回 true
```
> bool jcstr_isBlank(const JC_String* const str, JC_SpaceMode mode)
```text
此为库对外接口，返回 bool 类型结果，判断 str 是否为空串或仅由空白字符组成
注意：空白字符判定受 mode 控制——JC_SPACE_ASCII 时使用固定 ASCII 空白字符（空格、\t、\n、\r、\v、\f），JC_SPACE_LOCALE 或未知模式时跟随当前 locale 的 isspace；str 为 NULL 或缓冲区为 NULL 时返回 true
```
> bool jcstr_equalsWithInst(const JC_String* const str1, const JC_String* const str2)
```text
此为库对外接口，返回 bool 类型结果，判断两个字符串实例是否相等
注意：两个参数均为 NULL 时返回 true；仅一个为 NULL 时返回 false；两个实例的缓冲区均为 NULL 时返回 true；仅一个缓冲区为 NULL 时返回 false；两个均为空串时返回 true；仅一个为空串时返回 false；否则按内容比较
```
> bool jcstr_equalsIgnoreCaseWithInst(const JC_String* const str1, const JC_String* const str2, JC_SpaceMode mode)
```text
此为库对外接口，返回 bool 类型结果，判断两个字符串实例是否在忽略大小写后相等
注意：mode 语义同 jcstr_isBlank，决定大小写转换使用固定 ASCII 还是跟随 locale；NULL/缓冲区/空串的判定同 jcstr_equalsWithInst；内容比较前先比长度
```
> bool jcstr_equalsWithCh(const JC_String* const str, const char* const ch)
```text
此为库对外接口，返回 bool 类型结果，判断字符串实例内容与 C 字符串 ch 是否相等
注意：str 为 NULL 返回 false；str 缓冲区与 ch 同时为 NULL 返回 true；仅一方为 NULL 返回 false；两者均为空串返回 true；仅一方为空串返回 false；否则按内容比较
```
> bool jcstr_equalsIgnoreCaseWithCh(const JC_String* const str, const char* const ch, JC_SpaceMode mode)
```text
此为库对外接口，返回 bool 类型结果，判断字符串实例内容与 C 字符串 ch 是否在忽略大小写后相等
注意：mode 语义同 jcstr_isBlank；NULL/缓冲区/空串的判定同 jcstr_equalsWithCh；内容比较前先比长度
```
> bool jcstr_containsWithInst(const JC_String* const str1, const JC_String* const str2)
```text
此为库对外接口，返回 bool 类型结果，判断 str1 是否包含子串 str2
注意：两个参数均为 NULL 时返回 true；仅一个为 NULL 时返回 false；两个实例缓冲区均为 NULL 时返回 true；仅一个缓冲区为 NULL 时返回 false；其余情况交给 strstr，空串 needle 视为任何串的子串
```
> bool jcstr_containsIgnoreCaseWithInst(const JC_String* const str1, const JC_String* const str2, JC_SpaceMode mode)
```text
此为库对外接口，返回 bool 类型结果，判断 str1 是否在忽略大小写后包含子串 str2
注意：mode 语义同 jcstr_isBlank；NULL/缓冲区判定同 jcstr_containsWithInst；空串 needle 视为任何串的子串
```
> bool jcstr_containsWithCh(const JC_String* const str, const char* const ch)
```text
此为库对外接口，返回 bool 类型结果，判断字符串实例是否包含 C 字符串 ch
注意：str 为 NULL 返回 false；str 缓冲区与 ch 同时为 NULL 返回 true；仅一方为 NULL 返回 false；其余情况交给 strstr，空串 ch 视为任何串的子串
```
> bool jcstr_containsIgnoreCaseWithCh(const JC_String* const str, const char* const ch, JC_SpaceMode mode)
```text
此为库对外接口，返回 bool 类型结果，判断字符串实例是否在忽略大小写后包含 C 字符串 ch
注意：mode 语义同 jcstr_isBlank；NULL/缓冲区判定同 jcstr_containsWithCh；空串 ch 视为任何串的子串
```
> int jcstr_compareWithInst(const JC_String* const str1, const JC_String* const str2, int* result)
```text
此为库对外接口，返回 int 类型的状态码，按字典序比较两个字符串实例，将比较结果写入 result
注意：result 为 NULL 返回 JC_STR__NULL_RECEIVER；两个参数均为 NULL 时 *result 被置为 0 并返回 JC_STR__SUCCESSFUL_OP；仅一个为 NULL 时返回 JC_STR__NULL；两个实例缓冲区均为 NULL 时 *result 被置为 0 并返回 JC_STR__SUCCESSFUL_OP；仅一个缓冲区为 NULL 时返回 JC_STR__NULL_BUFFER；其余按 strcmp 结果写入 *result 并返回 JC_STR__SUCCESSFUL_OP
```
> int jcstr_compareIgnoreCaseWithInst(const JC_String* const str1, const JC_String* const str2, int* result, JC_SpaceMode mode)
```text
此为库对外接口，返回 int 类型的状态码，按字典序在忽略大小写后比较两个字符串实例，将比较结果写入 result
注意：mode 语义同 jcstr_isBlank；NULL/缓冲区判定同 jcstr_compareWithInst；其余按忽略大小写比较的结果写入 *result 并返回 JC_STR__SUCCESSFUL_OP
```
> int jcstr_compareWithCh(const JC_String* const str, const char* const ch, int* result)
```text
此为库对外接口，返回 int 类型的状态码，按字典序比较字符串实例与 C 字符串 ch，将比较结果写入 result
注意：result 为 NULL 返回 JC_STR__NULL_RECEIVER；str 为 NULL 返回 JC_STR__NULL；str 缓冲区与 ch 同时为 NULL 时返回 JC_STR__SUCCESSFUL_OP（*result 不被修改）；仅一方为 NULL 返回 JC_STR__NULL_BUFFER；其余按 strcmp 结果写入 *result 并返回 JC_STR__SUCCESSFUL_OP
```
> int jcstr_compareIgnoreCaseWithCh(const JC_String* const str, const char* const ch, int* result, JC_SpaceMode mode)
```text
此为库对外接口，返回 int 类型的状态码，按字典序在忽略大小写后比较字符串实例与 C 字符串 ch，将比较结果写入 result
注意：mode 语义同 jcstr_isBlank；NULL/缓冲区判定同 jcstr_compareWithCh；其余按忽略大小写比较的结果写入 *result 并返回 JC_STR__SUCCESSFUL_OP
```
> char* jcstr_toCharArray(const JC_String* const str)
```text
此为库对外接口，返回一个新分配的 C 字符串副本，内容与 str 相同
注意：调用者负责 free 返回值；str 为 NULL 或缓冲区为 NULL 时返回 NULL；空串会返回一个可 free 的非 NULL 空串
```
> int jcstr_toInt(const JC_String* const str, int* result, JC_SpaceMode mode)
```text
此为库对外接口，返回 int 类型的状态码，将 str 按十进制解析为 int 写入 result
注意：mode 控制前后空白跳过使用固定 ASCII 还是跟随 locale；允许正负号；空串或缓冲区为 NULL 返回 JC_STR__EMPTY；实例状态异常返回 str->status；result 为 NULL 返回 JC_STR__NULL_INT；格式非法返回 JC_STR__NOT_INT；溢出返回 JC_STR__OVERFLOW_INT；成功返回 JC_STR__SUCCESSFUL_OP
```
> int jcstr_toLong(const JC_String* const str, long* result, JC_SpaceMode mode)
```text
此为库对外接口，返回 int 类型的状态码，将 str 按十进制解析为 long 写入 result
注意：mode 语义同 jcstr_toInt；空串或缓冲区为 NULL 返回 JC_STR__EMPTY；实例状态异常返回 str->status；result 为 NULL 返回 JC_STR__NULL_LONG；格式非法返回 JC_STR__NOT_LONG；溢出返回 JC_STR__OVERFLOW_LONG；成功返回 JC_STR__SUCCESSFUL_OP
```
> int jcstr_toLongLong(const JC_String* const str, long long* result, JC_SpaceMode mode)
```text
此为库对外接口，返回 int 类型的状态码，将 str 按十进制解析为 long long 写入 result
注意：mode 语义同 jcstr_toInt；空串或缓冲区为 NULL 返回 JC_STR__EMPTY；实例状态异常返回 str->status；result 为 NULL 返回 JC_STR__NULL_LONGLONG；格式非法返回 JC_STR__NOT_LONGLONG；溢出返回 JC_STR__OVERFLOW_LONGLONG；成功返回 JC_STR__SUCCESSFUL_OP
```
> int jcstr_toFloat(const JC_String* const str, float* result, JC_SpaceMode mode)
```text
此为库对外接口，返回 int 类型的状态码，将 str 解析为 float 写入 result
注意：mode 语义同 jcstr_toInt；底层使用 strtod，支持小数、指数、以及 C 标准允许的格式（包括十六进制浮点与 inf/nan）；空串或缓冲区为 NULL 返回 JC_STR__EMPTY；实例状态异常返回 str->status；result 为 NULL 返回 JC_STR__NULL_FLOAT；格式非法返回 JC_STR__NOT_FLOAT；超出 float 范围返回 JC_STR__OVERFLOW_FLOAT；成功返回 JC_STR__SUCCESSFUL_OP
```
> int jcstr_toDouble(const JC_String* const str, double* result, JC_SpaceMode mode)
```text
此为库对外接口，返回 int 类型的状态码，将 str 解析为 double 写入 result
注意：mode 语义同 jcstr_toInt；底层使用 strtod，支持小数、指数、以及 C 标准允许的格式（包括十六进制浮点与 inf/nan）；空串或缓冲区为 NULL 返回 JC_STR__EMPTY；实例状态异常返回 str->status；result 为 NULL 返回 JC_STR__NULL_DOUBLE；格式非法返回 JC_STR__NOT_DOUBLE；超出 double 范围返回 JC_STR__OVERFLOW_DOUBLE；成功返回 JC_STR__SUCCESSFUL_OP
```
> jc_String jcstr_toLowerCase(const JC_String* const str, JC_SpaceMode mode)
```text
此为库对外接口，返回一个内容为 str 转小写后的新字符串实例，原实例不变
注意：mode 语义同 jcstr_isBlank，决定大小写转换使用固定 ASCII 还是跟随 locale；str 为 NULL 返回 NULL；缓冲区为 NULL 时返回状态为 JC_STR__NULL_BUFFER 的实例；空串返回空串实例；返回的实例需通过 jcstr_delete 释放
```
> jc_String jcstr_toUpperCase(const JC_String* const str, JC_SpaceMode mode)
```text
此为库对外接口，返回一个内容为 str 转大写后的新字符串实例，原实例不变
注意：mode 语义同 jcstr_isBlank，决定大小写转换使用固定 ASCII 还是跟随 locale；str 为 NULL 返回 NULL；缓冲区为 NULL 时返回状态为 JC_STR__NULL_BUFFER 的实例；空串返回空串实例；返回的实例需通过 jcstr_delete 释放
```
> jc_MutableString jcstr_toMutableString(const JC_String* const str)
```text
此为库对外接口，返回一个内容与 str 相同的可变字符串实例，原实例不变
注意：str 为 NULL 时返回 NULL；str 缓冲区为 NULL 时会转成状态为 NULL_BUFFER 的可变字符串实例；返回的实例需通过对应的可变字符串释放接口释放
```