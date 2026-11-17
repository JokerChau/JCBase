# IO
## 其他语言md
### English
[IO.md English version](IO_EN.md)
## 接口介绍

> jc_String jcio_readLineString(FILE* fp)
```text
从文件指针 fp 读取一行，返回一个带状态的不可变字符串实例
注意：支持跨平台换行符（\n、\r、\r\n），返回的内容不包含换行符；若读到文件末尾且无任何字符，返回的实例状态为 JC_STR__EOF；若读到的为空行，返回可用（AVAILABLE）但长度为 0 的实例；分配的底层内存会自动收缩至刚好容纳内容；返回的实例需通过 jcstr_delete 释放
```
> jc_MutableString jcio_readLineMutableString(FILE* fp)
```text
从文件指针 fp 读取一行，返回一个带状态的可变字符串实例
注意：换行符处理和 EOF 语义同 jcio_readLineString；与不可变版不同的是，可变版会直接接管底层分配的内存，且不会进行内存收缩（保留冗余容量 cap，方便调用方后续追加）；返回的实例需通过 jcms_delete 释放
```
> int jcio_writeLineString(FILE* fp, jc_String str)
```text
将不可变字符串实例 str 的内容写入文件指针 fp，并在末尾自动追加一个换行符（\n）
注意：若 fp 或 str 为 NULL，或实例状态异常，返回对应的错误码；若写入文件失败，返回 JC_IO__WRITE_FAILED；成功返回 JC_IO__SUCCESSFUL_OP
```
> int jcio_writeLineMutableString(FILE* fp, jc_MutableString ms)
```text
将可变字符串实例 ms 的内容写入文件指针 fp，并在末尾自动追加一个换行符（\n）
注意：若 fp 或 ms 为 NULL，或实例状态异常，返回对应的错误码；若写入文件失败，返回 JC_IO__WRITE_FAILED；成功返回 JC_IO__SUCCESSFUL_OP
```