# MutableString
## Other Languages md
### 简体中文
[MutableString.md 简中版](MutableString.md)
## Interface Description
> jc_MutableString jcms_new(void)
```text
This is a public library interface. It returns an empty mutable string instance with capacity INITIAL_MS_SIZE + 1.
Note: The returned instance must be released via jcms_delete.
```
> jc_MutableString jcms_newFrom(const char* const content)
```text
This is a public library interface. It creates a mutable string instance from C string content.
Note: If content is NULL, the returned instance has status JC_MS__NULL_BUFFER; the interface copies content and does not take ownership; capacity is max(strlen(content), INITIAL_MS_SIZE) + 1; the returned instance must be released via jcms_delete.
```
> void jcms_delete(jc_MutableString ms)
```text
This is a public library interface with no return value. It releases the given mutable string instance and its internal buffer.
Note: Passing in NULL will not crash, but do not pass in a non-jc_MutableString variable that has been cast to "jc_MutableString".
```
> int jcms_getStatus(const JC_MutableString* const ms)
```text
This is a public library interface. It returns a status code of type int. A mutable string instance must be passed in.
Note: When ms is NULL, it returns JC_MS__NULL; otherwise it returns the instance's current status.
```
> size_t jcms_getLength(const JC_MutableString* const ms)
```text
This is a public library interface. It returns the string length as size_t (excluding the trailing '\0'). A mutable string instance must be passed in.
Note: When ms is NULL, it returns 0; otherwise it returns the cached length of the instance, not the capacity.
```
> const char* jcms_viewBuffer(const JC_MutableString* const ms)
```text
This is a public library interface. It directly returns the internal buffer pointer of the instance for read-only inspection.
Note: Read-only is recommended; do not modify the content through this pointer; any unexpected errors caused by modification are not the library's responsibility; returns NULL when ms is NULL.
```
> int jcms_setContent(const jc_MutableString ms, const char* const content)
```text
This is a public library interface. It returns a status code of type int and sets the instance content to content.
Note: If content is NULL, the behavior is the same as passing an empty string - sets the first byte of the buffer to '\0' and len to 0, but does not free the buffer; this interface cannot be used to free memory; if the instance status is abnormal, ms->status is returned; the buffer is reallocated automatically if capacity is insufficient.
```
> int jcms_changeAt(const jc_MutableString ms, size_t index, char ch)
```text
This is a public library interface. It returns a status code of type int and replaces the character at index in the instance with ch.
Note: index is 0-based; if ms is NULL, JC_MS__NULL is returned; if the buffer is NULL, JC_MS__NULL_BUFFER is returned; if the status is abnormal, ms->status is returned; if index is out of bounds, JC_MS__INDEX_OUT_OF_BOUNDS is returned; ch must not be '\0', otherwise JC_MS__INVALID_CHARACTER_FOR_CHANGE is returned; on success, JC_MS__SUCCESSFUL_OP is returned.
```
> const char* jcms_statusToCharArray(const int status)
```text
This is a public library interface. It returns a human-readable description of the given status code.
Note: The returned string is static; the caller must not and need not free it.
```
> int jcms_appendWithInst(const jc_MutableString ms, const JC_MutableString* const src)
```text
This is a public library interface. It returns a status code of type int and appends the content of src to the end of ms.
Note: If ms is NULL, JC_MS__NULL is returned; if ms's buffer is NULL, JC_MS__NULL_BUFFER is returned; if src is NULL, JC_MS__NULL_FOLLOWER is returned; if src's buffer is NULL, JC_MS__NULL_FOLLOWER_BUFFER is returned; if src's length is 0, success is returned without modification; if ms's length is 0, this is equivalent to setContent; the buffer is reallocated automatically if capacity is insufficient.
```
> int jcms_appendWithCh(const jc_MutableString ms, const char* const ch)
```text
This is a public library interface. It returns a status code of type int and appends the C string ch to the end of ms.
Note: If ms is NULL, JC_MS__NULL is returned; if ms's buffer is NULL, JC_MS__NULL_BUFFER is returned; if ch is NULL, JC_MS__NULL_FOLLOWER is returned; if ch points into ms->buffer, JC_MS__INVALID_SOURCE is returned; if ch is an empty string, success is returned without modification; the buffer is reallocated automatically if capacity is insufficient.
```
> int jcms_reverse(jc_MutableString ms)
```text
This is a public library interface. It returns a status code of type int and reverses the content of ms in place.
Note: If ms is NULL, its buffer is NULL, or its content is an empty string, JC_MS__SUCCESSFUL_OP is returned directly (no modification is made).
```
> int jcms_split(const JC_MutableString* const ms, char delimiter, jc_MutableString** outArray, size_t* count, bool removeEmptyEntries)
```text
This is a public library interface. It returns a status code of type int and splits ms by the given delimiter into sub-string instances, outputting the array via outArray and the element count via count.
Note: outArray and count must be non-NULL; when removeEmptyEntries is true, empty sub-strings are dropped; the caller is responsible for releasing each instance in outArray (jcms_delete) and outArray itself (free); if some elements fail to be constructed, JC_MS__PARTIAL_SPLIT is returned and the corresponding positions may hold NULL or instances with abnormal status; if all segments are filtered out, JC_MS__SUCCESSFUL_OP is returned with *outArray == NULL and *count == 0.
```
> int jcms_trim(const jc_MutableString ms, JC_SpaceMode mode)
```text
This is a public library interface. It returns a status code of type int and trims leading and trailing whitespace from ms in place.
Note: Whitespace classification is controlled by mode - JC_SPACE_ASCII uses fixed ASCII whitespace (space, \t, \n, \r, \v, \f); JC_SPACE_LOCALE or unknown modes follow the current locale's isspace; if ms is NULL, its buffer is NULL, or its content is empty, JC_MS__SUCCESSFUL_OP is returned directly; if the content is entirely whitespace, the content is cleared and len is set to 0.
```
> int jcms_trimFront(const jc_MutableString ms, JC_SpaceMode mode)
```text
This is a public library interface. It returns a status code of type int and trims leading whitespace from ms in place.
Note: mode semantics are the same as jcms_trim; boundary handling matches jcms_trim.
```
> int jcms_trimBack(const jc_MutableString ms, JC_SpaceMode mode)
```text
This is a public library interface. It returns a status code of type int and trims trailing whitespace from ms in place.
Note: mode semantics are the same as jcms_trim; boundary handling matches jcms_trim.
```
> jc_MutableString jcms_clone(const JC_MutableString* const ms)
```text
This is a public library interface. It returns a deep-copied new instance with the same content as ms.
Note: If ms is NULL, NULL is returned; if ms's buffer is NULL, a new instance with status JC_MS__NULL_BUFFER is returned; the returned instance must be released via jcms_delete.
```
> int jcms_charAt(const JC_MutableString* const ms, size_t index, char* result)
```text
This is a public library interface. It returns a status code of type int and writes the character at index in ms to result.
Note: index is 0-based; if result is NULL, JC_MS__NULL_CHAR is returned; if ms is NULL, JC_MS__NULL is returned; if the buffer is NULL, JC_MS__NULL_BUFFER is returned; if index is out of bounds, JC_MS__INDEX_OUT_OF_BOUNDS is returned; on success, JC_MS__SUCCESSFUL_OP is returned.
```
> int jcms_indexOfFront(const JC_MutableString* const ms, char ch, size_t* result)
```text
This is a public library interface. It returns a status code of type int and writes to result the first position of character ch, searching from the front.
Note: If result is NULL, JC_MS__NULL_RESULT is returned; if ms is NULL, JC_MS__NULL is returned; if the buffer is NULL, JC_MS__NULL_BUFFER is returned; if not found, JC_MS__WITHOUT_CHAR is returned and *result is not modified; if found, JC_MS__SUCCESSFUL_OP is returned.
```
> int jcms_indexOfBack(const JC_MutableString* const ms, char ch, size_t* result)
```text
This is a public library interface. It returns a status code of type int and writes to result the first position of character ch, searching from the back.
Note: If result is NULL, JC_MS__NULL_RESULT is returned; if ms is NULL, JC_MS__NULL is returned; if the buffer is NULL, JC_MS__NULL_BUFFER is returned; if not found, JC_MS__WITHOUT_CHAR is returned and *result is not modified; if found, JC_MS__SUCCESSFUL_OP is returned.
```
> int jcms_indexOfFrontWithInst(const JC_MutableString* const ms, const JC_MutableString* const sub, size_t* result)
```text
This is a public library interface. It returns a status code of type int and writes to result the first position of sub-string instance sub, searching from the front.
Note: If result is NULL, JC_MS__NULL_RESULT is returned; if either parameter is NULL, JC_MS__NULL is returned; if either buffer is NULL, JC_MS__NULL_BUFFER is returned; if sub is an empty string, *result is set to the length of ms and JC_MS__SUCCESSFUL_OP is returned; if not found, JC_MS__WITHOUT_CHAR is returned.
```
> int jcms_indexOfFrontWithCh(const JC_MutableString* const ms, const char* const ch, size_t* result)
```text
This is a public library interface. It returns a status code of type int and writes to result the first position of C string ch, searching from the front.
Note: If result is NULL, JC_MS__NULL_RESULT is returned; if ms is NULL, JC_MS__NULL is returned; if the buffer or ch is NULL, JC_MS__NULL_BUFFER is returned; if ch is an empty string, *result is set to the length of ms and JC_MS__SUCCESSFUL_OP is returned; if not found, JC_MS__WITHOUT_CHAR is returned.
```
> jc_MutableString jcms_subInLengthFrontWithInst(const JC_MutableString* const ms, size_t start, size_t length)
```text
This is a public library interface. It returns a new mutable string instance taking length characters starting at start from ms.
Note: If ms is NULL, NULL is returned; if its buffer is NULL, a new instance with status NULL_BUFFER is returned; if ms is empty or length is 0, an empty string instance is returned; if start is out of bounds, an empty string instance with status JC_MS__INVALID_START is returned; if length exceeds the remaining length, it is truncated automatically; the returned instance must be released via jcms_delete.
```
> jc_MutableString jcms_subInLengthBackWithInst(const JC_MutableString* const ms, size_t end, size_t length)
```text
This is a public library interface. It returns a new mutable string instance taking length characters ending at end from ms.
Note: If ms is NULL, NULL is returned; if its buffer is NULL, a new instance with status NULL_BUFFER is returned; if ms is empty or length is 0, an empty string instance is returned; if end is out of bounds, an empty string instance with status JC_MS__INVALID_END is returned; if length exceeds end+1, it is truncated to the beginning; the returned instance must be released via jcms_delete.
```
> jc_MutableString jcms_subInRangeWithInst(const JC_MutableString* const ms, size_t start, size_t end)
```text
This is a public library interface. It returns a new mutable string instance taking the range [start, end) from ms.
Note: If ms is NULL, NULL is returned; if its buffer is NULL, a new instance with status NULL_BUFFER is returned; if ms is empty or start == end, an empty string instance is returned; if start > end, an empty string instance with status JC_MS__INVALID_INDEX is returned; if start or end is out of bounds, an empty string instance with status JC_MS__INDEX_OUT_OF_BOUNDS is returned; the returned instance must be released via jcms_delete.
```
> jc_MutableString jcms_subInLengthFrontWithCh(const char* const ch, size_t start, size_t length)
```text
This is a public library interface. It returns a new mutable string instance taking length characters starting at start from the C string ch.
Note: If ch is NULL, an instance with status JC_MS__NULL_BUFFER is returned; if ch is empty or length is 0, an empty string instance is returned; if start is out of bounds, an empty string instance with status JC_MS__INVALID_START is returned; if length exceeds the remaining length, it is truncated automatically; the returned instance must be released via jcms_delete.
```
> jc_MutableString jcms_subInLengthBackWithCh(const char* const ch, size_t end, size_t length)
```text
This is a public library interface. It returns a new mutable string instance taking length characters ending at end from the C string ch.
Note: If ch is NULL, an instance with status JC_MS__NULL_BUFFER is returned; if ch is empty or length is 0, an empty string instance is returned; if end is out of bounds, an empty string instance with status JC_MS__INVALID_END is returned; if length exceeds end+1, it is truncated to the beginning; the returned instance must be released via jcms_delete.
```
> jc_MutableString jcms_subInRangeWithCh(const char* const ch, size_t start, size_t end)
```text
This is a public library interface. It returns a new mutable string instance taking the range [start, end) from the C string ch.
Note: If ch is NULL, an instance with status JC_MS__NULL_BUFFER is returned; if ch is empty or start == end, an empty string instance is returned; if start > end, an empty string instance with status JC_MS__INVALID_INDEX is returned; if out of bounds, an empty string instance with status JC_MS__INDEX_OUT_OF_BOUNDS is returned; the returned instance must be released via jcms_delete.
```
> int jcms_replaceFirstWithInst(JC_MutableString* ms, const char* const target, const JC_MutableString* const replacement)
```text
This is a public library interface. It returns a status code of type int and replaces the first occurrence of target in ms in place with the content of replacement.
Note: If ms is NULL, JC_MS__NULL is returned; if the status is abnormal, ms->status is returned; if target is NULL, JC_MS__NULL_TARGET is returned; if replacement is NULL or its buffer is NULL, JC_MS__INVALID_REPLACEMENT is returned; if replacement points into ms->buffer, JC_MS__INVALID_REPLACEMENT is returned (to prevent dangling after realloc); empty-target semantics: empty content + empty target sets the whole content to replacement, empty content + non-empty target returns NO_TARGET_MATCHED, non-empty content + empty target appends at the end; a non-empty target not found returns JC_MS__NO_TARGET_MATCHED.
```
> int jcms_replaceFirstWithCh(JC_MutableString* ms, const char* const target, const char* const replacement)
```text
This is a public library interface. It returns a status code of type int and replaces the first occurrence of target in ms in place with the C string replacement.
Note: If ms is NULL, JC_MS__NULL is returned; if the status is abnormal, ms->status is returned; if target is NULL, JC_MS__NULL_TARGET is returned; if replacement is NULL, JC_MS__INVALID_REPLACEMENT is returned; empty-target semantics match jcms_replaceFirstWithInst; a non-empty target not found returns JC_MS__NO_TARGET_MATCHED.
```
> int jcms_replaceAllWithInst(JC_MutableString* ms, const char* const target, const JC_MutableString* const replacement)
```text
This is a public library interface. It returns a status code of type int and replaces all non-overlapping occurrences of target in ms in place with the content of replacement.
Note: If ms is NULL, JC_MS__NULL is returned; if the status is abnormal, ms->status is returned; if target is NULL, JC_MS__NULL_TARGET is returned; if replacement is NULL or its buffer is NULL, JC_MS__INVALID_REPLACEMENT is returned; if replacement points into ms->buffer, JC_MS__INVALID_REPLACEMENT is returned; empty-target semantics match jcms_replaceFirstWithInst; a non-empty target not found returns JC_MS__NO_TARGET_MATCHED.
```
> int jcms_replaceAllWithCh(JC_MutableString* ms, const char* const target, const char* const replacement)
```text
This is a public library interface. It returns a status code of type int and replaces all non-overlapping occurrences of target in ms in place with the C string replacement.
Note: If ms is NULL, JC_MS__NULL is returned; if the status is abnormal, ms->status is returned; if target is NULL, JC_MS__NULL_TARGET is returned; if replacement is NULL, JC_MS__INVALID_REPLACEMENT is returned; empty-target semantics match jcms_replaceFirstWithInst; a non-empty target not found returns JC_MS__NO_TARGET_MATCHED.
```
> bool jcms_startWithCh(const JC_MutableString* const ms, const char* const prefix)
```text
This is a public library interface. It returns a bool indicating whether ms starts with the C string prefix.
Note: If ms is NULL or its buffer is NULL, false is returned; if prefix is NULL, false is returned; if prefix is an empty string, true is returned.
```
> bool jcms_startWithInst(const JC_MutableString* const ms, const JC_MutableString* const prefix)
```text
This is a public library interface. It returns a bool indicating whether ms starts with the string instance prefix.
Note: If prefix is NULL, false is returned; other boundary cases are the same as jcms_startWithCh.
```
> bool jcms_endWithCh(const JC_MutableString* const ms, const char* const suffix)
```text
This is a public library interface. It returns a bool indicating whether ms ends with the C string suffix.
Note: If ms is NULL or its buffer is NULL, false is returned; if suffix is NULL, false is returned; if suffix is an empty string, true is returned.
```
> bool jcms_endWithInst(const JC_MutableString* const ms, const JC_MutableString* const suffix)
```text
This is a public library interface. It returns a bool indicating whether ms ends with the string instance suffix.
Note: If suffix is NULL, false is returned; other boundary cases are the same as jcms_endWithCh.
```
> bool jcms_isEmpty(const JC_MutableString* const ms)
```text
This is a public library interface. It returns a bool indicating whether ms is an empty string.
Note: It returns true when ms is NULL, when its buffer is NULL, or when its content is an empty string.
```
> bool jcms_isBlank(const JC_MutableString* const ms, JC_SpaceMode mode)
```text
This is a public library interface. It returns a bool indicating whether ms is an empty string or consists solely of whitespace characters.
Note: Whitespace classification is controlled by mode - JC_SPACE_ASCII uses fixed ASCII whitespace (space, \t, \n, \r, \v, \f); JC_SPACE_LOCALE or unknown modes follow the current locale's isspace; it returns true when ms is NULL or its buffer is NULL.
```
> bool jcms_equalsWithInst(const JC_MutableString* const ms1, const JC_MutableString* const ms2)
```text
This is a public library interface. It returns a bool indicating whether two mutable string instances are equal.
Note: Both parameters NULL returns true; exactly one NULL returns false; both buffers NULL returns true; exactly one buffer NULL returns false; both empty strings returns true; exactly one empty string returns false; otherwise contents are compared.
```
> bool jcms_equalsIgnoreCaseWithInst(const JC_MutableString* const ms1, const JC_MutableString* const ms2, JC_SpaceMode mode)
```text
This is a public library interface. It returns a bool indicating whether two mutable string instances are equal ignoring case.
Note: mode semantics are the same as jcms_isBlank, controlling whether case folding uses fixed ASCII or follows the locale; NULL/buffer/empty-string handling matches jcms_equalsWithInst; lengths are compared before content comparison.
```
> bool jcms_equalsWithCh(const JC_MutableString* const ms, const char* const ch)
```text
This is a public library interface. It returns a bool indicating whether the content of ms equals the C string ch.
Note: If ms is NULL, false is returned; if both ms's buffer and ch are NULL, true is returned; if exactly one is NULL, false is returned; if both are empty strings, true is returned; if exactly one is empty, false is returned; otherwise contents are compared.
```
> bool jcms_equalsIgnoreCaseWithCh(const JC_MutableString* const ms, const char* const ch, JC_SpaceMode mode)
```text
This is a public library interface. It returns a bool indicating whether the content of ms equals the C string ch ignoring case.
Note: mode semantics are the same as jcms_isBlank; NULL/buffer/empty-string handling matches jcms_equalsWithCh; lengths are compared before content comparison.
```
> bool jcms_containsWithInst(const JC_MutableString* const ms1, const JC_MutableString* const ms2)
```text
This is a public library interface. It returns a bool indicating whether ms1 contains the sub-string ms2.
Note: Both parameters NULL returns true; exactly one NULL returns false; both buffers NULL returns true; exactly one buffer NULL returns false; otherwise strstr is used, where an empty needle matches every string.
```
> bool jcms_containsIgnoreCaseWithInst(const JC_MutableString* const ms1, const JC_MutableString* const ms2, JC_SpaceMode mode)
```text
This is a public library interface. It returns a bool indicating whether ms1 contains the sub-string ms2 ignoring case.
Note: mode semantics are the same as jcms_isBlank; NULL/buffer handling matches jcms_containsWithInst; an empty needle matches every string.
```
> bool jcms_containsWithCh(const JC_MutableString* const ms, const char* const ch)
```text
This is a public library interface. It returns a bool indicating whether ms contains the C string ch.
Note: If ms is NULL, false is returned; if both ms's buffer and ch are NULL, true is returned; if exactly one is NULL, false is returned; otherwise strstr is used, where an empty ch matches every string.
```
> bool jcms_containsIgnoreCaseWithCh(const JC_MutableString* const ms, const char* const ch, JC_SpaceMode mode)
```text
This is a public library interface. It returns a bool indicating whether ms contains the C string ch ignoring case.
Note: mode semantics are the same as jcms_isBlank; NULL/buffer handling matches jcms_containsWithCh; an empty ch matches every string.
```
> int jcms_compareWithInst(const JC_MutableString* const ms1, const JC_MutableString* const ms2, int* result)
```text
This is a public library interface. It returns a status code of type int and compares two mutable string instances lexicographically, writing the result to result.
Note: If result is NULL, JC_MS__NULL_RECEIVER is returned; if both parameters are NULL, *result is set to 0 and JC_MS__SUCCESSFUL_OP is returned; if exactly one is NULL, JC_MS__NULL is returned; if both buffers are NULL, *result is set to 0 and JC_MS__SUCCESSFUL_OP is returned; if exactly one buffer is NULL, JC_MS__NULL_BUFFER is returned; otherwise the strcmp result is written to *result and JC_MS__SUCCESSFUL_OP is returned.
```
> int jcms_compareIgnoreCaseWithInst(const JC_MutableString* const ms1, const JC_MutableString* const ms2, int* result, JC_SpaceMode mode)
```text
This is a public library interface. It returns a status code of type int and compares two mutable string instances lexicographically ignoring case, writing the result to result.
Note: mode semantics are the same as jcms_isBlank; NULL/buffer handling matches jcms_compareWithInst; otherwise the case-insensitive comparison result is written to *result and JC_MS__SUCCESSFUL_OP is returned.
```
> int jcms_compareWithCh(const JC_MutableString* const ms, const char* const ch, int* result)
```text
This is a public library interface. It returns a status code of type int and compares ms with the C string ch lexicographically, writing the result to result.
Note: If result is NULL, JC_MS__NULL_RECEIVER is returned; if ms is NULL, JC_MS__NULL is returned; if both ms's buffer and ch are NULL, *result is set to 0 and JC_MS__SUCCESSFUL_OP is returned; if exactly one is NULL, JC_MS__NULL_BUFFER is returned; otherwise the strcmp result is written to *result and JC_MS__SUCCESSFUL_OP is returned.
```
> int jcms_compareIgnoreCaseWithCh(const JC_MutableString* const ms, const char* const ch, int* result, JC_SpaceMode mode)
```text
This is a public library interface. It returns a status code of type int and compares ms with the C string ch lexicographically ignoring case, writing the result to result.
Note: mode semantics are the same as jcms_isBlank; NULL/buffer handling matches jcms_compareWithCh; otherwise the case-insensitive comparison result is written to *result and JC_MS__SUCCESSFUL_OP is returned.
```
> char* jcms_toCharArray(const JC_MutableString* const ms)
```text
This is a public library interface. It returns a newly allocated C string copy with the same content as ms.
Note: The caller is responsible for freeing the returned pointer; if ms is NULL or its buffer is NULL, NULL is returned; an empty string returns a non-NULL, freeable empty string.
```
> int jcms_toInt(const JC_MutableString* const ms, int* result, JC_SpaceMode mode)
```text
This is a public library interface. It returns a status code of type int and parses ms as a decimal int, writing the result to result.
Note: mode controls whether leading/trailing whitespace is skipped using fixed ASCII or the locale; a sign is allowed; if ms is empty or its buffer is NULL, JC_MS__EMPTY is returned; if the instance status is abnormal, ms->status is returned; if result is NULL, JC_MS__NULL_INT is returned; on invalid format, JC_MS__NOT_INT is returned; on overflow, JC_MS__OVERFLOW_INT is returned; on success, JC_MS__SUCCESSFUL_OP is returned.
```
> int jcms_toLong(const JC_MutableString* const ms, long* result, JC_SpaceMode mode)
```text
This is a public library interface. It returns a status code of type int and parses ms as a decimal long, writing the result to result.
Note: mode semantics are the same as jcms_toInt; if ms is empty or its buffer is NULL, JC_MS__EMPTY is returned; if the instance status is abnormal, ms->status is returned; if result is NULL, JC_MS__NULL_LONG is returned; on invalid format, JC_MS__NOT_LONG is returned; on overflow, JC_MS__OVERFLOW_LONG is returned; on success, JC_MS__SUCCESSFUL_OP is returned.
```
> int jcms_toLongLong(const JC_MutableString* const ms, long long* result, JC_SpaceMode mode)
```text
This is a public library interface. It returns a status code of type int and parses ms as a decimal long long, writing the result to result.
Note: mode semantics are the same as jcms_toInt; if ms is empty or its buffer is NULL, JC_MS__EMPTY is returned; if the instance status is abnormal, ms->status is returned; if result is NULL, JC_MS__NULL_LONGLONG is returned; on invalid format, JC_MS__NOT_LONGLONG is returned; on overflow, JC_MS__OVERFLOW_LONGLONG is returned; on success, JC_MS__SUCCESSFUL_OP is returned.
```
> int jcms_toFloat(const JC_MutableString* const ms, float* result, JC_SpaceMode mode)
```text
This is a public library interface. It returns a status code of type int and parses ms as a float, writing the result to result.
Note: mode semantics are the same as jcms_toInt; the implementation uses strtod, so decimals, exponents and other formats allowed by the C standard (including hexadecimal floats and inf/nan) are accepted; if ms is empty or its buffer is NULL, JC_MS__EMPTY is returned; if the instance status is abnormal, ms->status is returned; if result is NULL, JC_MS__NULL_FLOAT is returned; on invalid format, JC_MS__NOT_FLOAT is returned; when it exceeds the float range, JC_MS__OVERFLOW_FLOAT is returned; on success, JC_MS__SUCCESSFUL_OP is returned.
```
> int jcms_toDouble(const JC_MutableString* const ms, double* result, JC_SpaceMode mode)
```text
This is a public library interface. It returns a status code of type int and parses ms as a double, writing the result to result.
Note: mode semantics are the same as jcms_toInt; the implementation uses strtod, so decimals, exponents and other formats allowed by the C standard (including hexadecimal floats and inf/nan) are accepted; if ms is empty or its buffer is NULL, JC_MS__EMPTY is returned; if the instance status is abnormal, ms->status is returned; if result is NULL, JC_MS__NULL_DOUBLE is returned; on invalid format, JC_MS__NOT_DOUBLE is returned; when it exceeds the double range, JC_MS__OVERFLOW_DOUBLE is returned; on success, JC_MS__SUCCESSFUL_OP is returned.
```
> int jcms_toLowerCase(JC_MutableString* ms, JC_SpaceMode mode)
```text
This is a public library interface. It returns a status code of type int and converts the content of ms to lower case in place.
Note: mode semantics are the same as jcms_isBlank, controlling whether case folding uses fixed ASCII or follows the locale; if ms is NULL, JC_MS__NULL is returned; if its buffer is NULL, JC_MS__NULL_BUFFER is returned; an empty string is not modified and success is returned; on success, JC_MS__SUCCESSFUL_OP is returned.
```
> int jcms_toUpperCase(JC_MutableString* ms, JC_SpaceMode mode)
```text
This is a public library interface. It returns a status code of type int and converts the content of ms to upper case in place.
Note: mode semantics are the same as jcms_isBlank, controlling whether case folding uses fixed ASCII or follows the locale; if ms is NULL, JC_MS__NULL is returned; if its buffer is NULL, JC_MS__NULL_BUFFER is returned; an empty string is not modified and success is returned; on success, JC_MS__SUCCESSFUL_OP is returned.
```
> jc_String jcms_toString(const JC_MutableString* const ms)
```text
This is a public library interface. It returns a new immutable string instance with the same content as ms.
Note: If ms is NULL, an immutable string instance with status JC_STR__NULL_BUFFER is returned; if ms's buffer is NULL, it is handled the same way; the returned instance must be released via jcstr_delete.
```