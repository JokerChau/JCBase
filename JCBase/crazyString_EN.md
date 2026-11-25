# crazyString
## Other Languages md
### 简体中文
[crazyString.md 简中版](crazyString.md)
## Interface Description
> jc_czyString jccstr_new(void)
```text
This is a public library interface. It returns an empty instance whose internal buffer is 1 byte, with len = 0 and cap = 1.
Note: Returns NULL if allocation fails; the returned instance always has a non-NULL buffer and must be released via jccstr_delete.
```
> jc_czyString jccstr_newWithSize(size_t capacity)
```text
This is a public library interface. It returns an empty instance with a capacity of capacity bytes (capacity includes the trailing '\0').
Note: Returns NULL when capacity is 0; returns NULL if allocation fails; for example, capacity 201 can hold at most 200 content characters.
```
> jc_czyString jccstr_newFrom(const char* content, size_t length)
```text
This is a public library interface. It creates an instance by copying length bytes from content and appending '\0', with capacity length + 1.
Note: Returns NULL when content is NULL or length is SIZE_MAX; if length is smaller than the actual content length, the content is truncated, which is part of this interface's semantics; length must not exceed strlen(content), otherwise it is a caller violation; the returned instance must be released via jccstr_delete.
```
> void jccstr_delete(jc_czyString str)
```text
This is a public library interface with no return value. It releases the given instance and its internal buffer.
Note: Passing in NULL will not crash, but do not pass in a non-jc_czyString variable that has been cast to "jc_czyString".
```
> const char* jccstr_viewBuffer(jc_czyString str)
```text
This is a public library interface. It directly returns the internal buffer pointer of the instance for read-only inspection.
Note: Read-only is recommended; do not modify the content through this pointer; the pointer may become invalid after the instance is modified, reallocated or released; returns NULL when str is NULL.
```
> char* jccstr_toCharArray(jc_czyString str)
```text
This is a public library interface. It returns a copy of the content, terminated with '\0'.
Note: The caller must free the returned pointer; returns NULL if allocation fails or str is NULL.
```
> size_t jccstr_getLength(jc_czyString str)
```text
This is a public library interface. It returns the content length as size_t (up to the first '\0', excluding the trailing '\0').
Note: Returns 0 when str is NULL.
```
> size_t jccstr_getCapacity(jc_czyString str)
```text
This is a public library interface. It returns the capacity as size_t (in bytes, including the trailing '\0').
Note: Returns 0 when str is NULL; capacity is unrelated to content length, and the number of content characters it can hold is cap - 1.
```
> char* jccstr_charAt(jc_czyString str, size_t index)
```text
This is a public library interface. It returns char*, pointing at index in buffer.
```
> char jccstr_viewAt(jc_czyString str, size_t index)
```text
This is a public library interface. It returns content at index.
```
> jc_czyString jccstr_changeAt(jc_czyString str, size_t index, char content)
```text
This is a public library interface. It returns jc_czyString and replaces the character at index with content.
Note: When content is '\0', the behavior is the same as jccstr_endAt, i.e. the string is truncated there and len becomes index; a non-'\0' write requires index < len, while a '\0' write requires index <= len; the return value is str itself for chaining.
```
> jc_czyString jccstr_changeFrom(jc_czyString str, size_t from, size_t length, const char* content)
```text
This is a public library interface. It returns jc_czyString and overwrites the existing content starting at from with the first length bytes of content.
Note: If the written range extends past the original len, a '\0' is placed at from + length; if capacity is insufficient it tries to grow, and returns the instance unchanged on failure; when from + length + 1 overflows it returns the instance unchanged; a '\0' inside content truncates the string following C semantics; from <= len is required.
```
> jc_czyString jccstr_endAt(jc_czyString str, size_t index)
```text
This is a public library interface. It returns jc_czyString, sets the character at index to '\0' and sets len to index.
Note: index <= len is required; the return value is str itself.
```
> jc_czyString jccstr_clear(jc_czyString str)
```text
This is a public library interface. It returns jc_czyString, sets the first byte of the buffer to '\0' and sets len to 0.
Note: Capacity is left unchanged; the return value is str itself; call jccstr_shrinkToFit afterwards to release the unused memory.
```
> jc_czyString jccstr_removeAt(jc_czyString str, size_t index)
```text
This is a public library interface. It returns jc_czyString, removes the character at index, shifts the following content one position left and decrements len.
Note: index < len is required; the return value is str itself.
```
> jc_czyString jccstr_removeInRange(jc_czyString str, size_t from, size_t to)
```text
This is a public library interface. It returns jc_czyString, removes the characters in the half-open range [from, to) and subtracts to - from from len.
Note: from <= to <= len is required; when from equals to the range is empty and the content is unchanged; the return value is str itself.
```
> jc_czyString jccstr_insertAt(jc_czyString str, size_t index, char content)
```text
This is a public library interface. It returns jc_czyString, inserts one character at index and shifts [index, len] (including the terminator) one position right.
Note: index <= len is required, and index equal to len is equivalent to appending; when content is '\0' the behavior is the same as jccstr_endAt; capacity grows by only 1 byte at a time, so call jccstr_reserve first when inserting many characters in a row; the return value is str itself.
```
> jc_czyString jccstr_insertBytes(jc_czyString str, size_t index, const char* content, size_t length)
```text
This is a public library interface. It returns jc_czyString and inserts the first length bytes of content at index, shifting the existing content right.
Note: It does nothing when length is 0; it returns the instance unchanged when str->len + length + 1 overflows; if capacity is insufficient it tries to grow, and returns the instance unchanged on failure; index <= len is required, and index equal to len is equivalent to appending; content must not point into str->buffer.
```
> jc_czyString jccstr_setContent(jc_czyString str, const char* content, size_t length)
```text
This is a public library interface. It returns jc_czyString and replaces the entire content with the first length bytes of content.
Note: If capacity is insufficient it tries to grow, and returns the instance unchanged on failure; when length is SIZE_MAX it is treated as SIZE_MAX - 1, growth will inevitably fail and the instance is returned unchanged; content must not be NULL; the return value is str itself.
```
> jc_czyString jccstr_reserve(jc_czyString str, size_t wholeCapacity)
```text
This is a public library interface. It returns jc_czyString and pre-allocates space for the instance; when cap is smaller than wholeCapacity, it grows to at least wholeCapacity bytes.
Note: It only grows, never shrinks; the original capacity is kept if growth fails; the return value is str itself.
```
> jc_czyString jccstr_shrinkToFit(jc_czyString str)
```text
This is a public library interface. It returns jc_czyString and shrinks the capacity to exactly len + 1 bytes, releasing everything after the first '\0'.
Note: The original capacity is kept if the shrink fails; the content is unchanged; the return value is str itself.
```
> jc_czyString jccstr_reallocate(jc_czyString str, size_t capacity)
```text
This is a public library interface. It returns jc_czyString and sets the capacity exactly to capacity bytes; if capacity is less than or equal to len, the content is truncated to capacity - 1 characters.
Note: When the shrink succeeds and the content no longer fits, it is truncated and terminated with '\0'; when capacity is 0 the behavior depends on the implementation-defined behavior of realloc and is the caller's responsibility; on allocation failure both capacity and content stay unchanged; the return value is str itself.
```
> jc_czyString jccstr_clone(jc_czyString str)
```text
This is a public library interface. It returns a full clone of the instance, copying capacity, length and the residual content after the first '\0'.
Note: Returns NULL when str is NULL or allocation fails; the returned instance must be released via jccstr_delete; for a clean content copy, use jccstr_newFrom(jccstr_viewBuffer(str), jccstr_getLength(str)).
```
> jc_czyString jccstr_appendBytes(jc_czyString str, const char* content, size_t length)
```text
This is a public library interface. It returns jc_czyString and appends the first length bytes of content to the end, growing the buffer to exactly the required size when needed.
Note: It returns the instance unchanged on length overflow or grow failure; a '\0' inside content truncates the appended part following C semantics; content must not be NULL; the return value is str itself.
```
> jc_czyString jccstr_append(jc_czyString str, jc_czyString other)
```text
This is a public library interface. It returns jc_czyString and appends the content of other to the end of str; other itself is not modified.
Note: str and other may be the same instance (self-append); it does nothing when other has length 0; it returns the instance unchanged on length overflow or grow failure; the return value is str itself.
```
> jc_czyString jccstr_concat(jc_czyString a, jc_czyString b)
```text
This is a public library interface. It returns a new instance containing a followed by b; a and b are not modified.
Note: Returns NULL when either argument is NULL, on length overflow or when allocation fails; the returned instance must be released via jccstr_delete.
```
