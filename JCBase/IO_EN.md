# IO
## Other Languages md
### 简体中文
[IO.md 简中版](IO.md)
## Interface Description

> jc_String jcio_readLineString(FILE* fp)
```text
Reads a single line from the file pointer fp, returning a stateful immutable string instance.
Note: Supports cross-platform line endings (\n, \r, \r\n); the returned content does not include the line ending; if the end of the file is reached with no characters read, the returned instance has the status JC_STR__EOF; if an empty line is read, an instance with status AVAILABLE and length 0 is returned; the underlying allocated memory is automatically shrunk to exactly fit the content; the returned instance must be released via jcstr_delete.
```
> jc_MutableString jcio_readLineMutableString(FILE* fp)
```text
Reads a single line from the file pointer fp, returning a stateful mutable string instance.
Note: Line ending handling and EOF semantics are identical to jcio_readLineString; unlike the immutable version, the mutable version directly takes over the allocated memory and does not shrink it (retaining redundant capacity cap for future appends); the returned instance must be released via jcms_delete.
```
> int jcio_writeLineString(FILE* fp, jc_String str)
```text
Writes the content of the immutable string instance str to the file pointer fp, and automatically appends a newline character (\n) at the end.
Note: If fp or str is NULL, or the instance status is abnormal, the corresponding error code is returned; if writing to the file fails, JC_IO__WRITE_FAILED is returned; on success, JC_IO__SUCCESSFUL_OP is returned.
```
> int jcio_writeLineMutableString(FILE* fp, jc_MutableString ms)
```text
Writes the content of the mutable string instance ms to the file pointer fp, and automatically appends a newline character (\n) at the end.
Note: If fp or ms is NULL, or the instance status is abnormal, the corresponding error code is returned; if writing to the file fails, JC_IO__WRITE_FAILED is returned; on success, JC_IO__SUCCESSFUL_OP is returned.
```