# String
## Other Languages md
### 简体中文
[String.md 简中版](String.md)
## Interface Description
> jc_String jcstr_newFrom(const char* const content)
```text
This is a public library interface. It creates a string instance from C string content.
Note: If content is NULL, the returned instance has status JC_STR__NULL_BUFFER; if allocation fails, an instance with status JC_STR__NULL_BUFFER may also be returned; if even the instance itself cannot be allocated, NULL is returned. The returned instance must be released via jcstr_delete.
```
> void jcstr_delete(jc_String str)
```text
This is a public library interface with no return value. It releases the given string instance and its internal buffer.
Note: Passing in NULL will not crash, but do not pass in a non-jc_String variable that has been cast to "jc_String".
```
> int jcstr_getStatus(const JC_String* const str)
```text
This is a public library interface. It returns a status code of type int. A string instance must be passed in.
Note: When str is NULL, it returns JC_STR__NULL; otherwise it returns the instance's current status.
```
> size_t jcstr_getLength(const JC_String* const str)
```text
This is a public library interface. It returns the string length as size_t (excluding the trailing '\0'). A string instance must be passed in.
Note: When str is NULL or its buffer is NULL, it returns 0; otherwise it returns the cached length of the instance.
```
> const char* jcstr_statusToCharArray(const int status)
```text
This is a public library interface. It returns a human-readable description of the given status code.
Note: The returned string is static; the caller must not and need not free it.
```
> jc_String jcstr_appendWithInst(const JC_String* const str1, const JC_String* const str2)
```text
This is a public library interface. It returns a new string instance whose content is str1 followed by str2. The original instances are unchanged.
Note: If str1 is NULL, NULL is returned; if str1's buffer is invalid, an instance with status JC_STR__NULL_BUFFER is returned; if str2 is NULL, a copy of str1 is returned carrying status JC_STR__INVALID_FOLLOWER; if str2's buffer is invalid, a copy of str1 is returned carrying status JC_STR__INVALID_FOLLOWER_BUFFER. The returned instance must be released via jcstr_delete.
```
> jc_String jcstr_appendWithCh(const JC_String* const str, const char* const content)
```text
This is a public library interface. It returns a new string instance whose content is str followed by the C string content. The original instance is unchanged.
Note: If str is NULL, NULL is returned; if str's buffer is invalid, an instance with status JC_STR__NULL_BUFFER is returned; if content is NULL, a copy of str is returned carrying status JC_STR__INVALID_FOLLOWER. The returned instance must be released via jcstr_delete.
```
> jc_String jcstr_reverse(const JC_String* const str)
```text
This is a public library interface. It returns a new string instance whose content is str reversed. The original instance is unchanged.
Note: If str is NULL, NULL is returned; if str's buffer is invalid, an instance with status JC_STR__NULL_BUFFER is returned; an empty string returns an empty string instance. The returned instance must be released via jcstr_delete.
```
> int jcstr_split(const JC_String* const str, char delimiter, jc_String** outArray, size_t* count, bool removeEmptyEntries)
```text
This is a public library interface. It returns a status code of type int and splits str by the given delimiter into sub-string instances, outputting the array via outArray and the element count via count.
Note: outArray and count must be non-NULL; when removeEmptyEntries is true, empty sub-strings are dropped; the caller is responsible for releasing each instance in outArray (jcstr_delete) and outArray itself (free); if some elements fail to be constructed, JC_STR__PARTIAL_SPLIT is returned; if all segments are filtered out, JC_STR__SUCCESSFUL_OP is returned with *outArray == NULL and *count == 0.
```
> jc_String jcstr_trim(const JC_String* const str, JC_SpaceMode mode)
```text
This is a public library interface. It returns a new string instance with leading and trailing whitespace removed. The original instance is unchanged.
Note: Whitespace classification is controlled by mode - JC_SPACE_ASCII uses fixed ASCII whitespace (space, \t, \n, \r, \v, \f); JC_SPACE_LOCALE or unknown modes follow the current locale's isspace; if the content is entirely whitespace, an empty string instance is returned. The returned instance must be released via jcstr_delete.
```
> jc_String jcstr_trimFront(const JC_String* const str, JC_SpaceMode mode)
```text
This is a public library interface. It returns a new string instance with leading whitespace removed. The original instance is unchanged.
Note: mode semantics are the same as jcstr_trim; if the content is entirely whitespace, an empty string instance is returned. The returned instance must be released via jcstr_delete.
```
> jc_String jcstr_trimBack(const JC_String* const str, JC_SpaceMode mode)
```text
This is a public library interface. It returns a new string instance with trailing whitespace removed. The original instance is unchanged.
Note: mode semantics are the same as jcstr_trim; if the content is entirely whitespace, an empty string instance is returned. The returned instance must be released via jcstr_delete.
```
> jc_String jcstr_clone(const JC_String* const str)
```text
This is a public library interface. It returns a deep-copied new string instance with the same content as str. The original instance is unchanged.
Note: If str is NULL, NULL is returned; if str's buffer is invalid, an instance with status JC_STR__NULL_BUFFER is returned. The returned instance must be released via jcstr_delete.
```
> int jcstr_charAt(const JC_String* const str, size_t index, char* result)
```text
This is a public library interface. It returns a status code of type int and writes the character at index in str to result.
Note: index is 0-based; if result is NULL, JC_STR__NULL_CHAR is returned; if str is NULL, JC_STR__NULL is returned; if the buffer is NULL, JC_STR__NULL_BUFFER is returned; if index is out of bounds, JC_STR__IDX_OUT_BOUNDS is returned; on success, JC_STR__SUCCESSFUL_OP is returned.
```
> int jcstr_indexOfFront(const JC_String* const str, char ch, size_t* result)
```text
This is a public library interface. It returns a status code of type int and writes to result the first position of character ch, searching from the front.
Note: If result is NULL, JC_STR__NULL_RECEIVER is returned; if not found, JC_STR__WITHOUT_CH is returned and *result is not modified; if found, JC_STR__SUCCESSFUL_OP is returned.
```
> int jcstr_indexOfBack(const JC_String* const str, char ch, size_t* result)
```text
This is a public library interface. It returns a status code of type int and writes to result the first position of character ch, searching from the back.
Note: If result is NULL, JC_STR__NULL_RECEIVER is returned; if not found, JC_STR__WITHOUT_CH is returned and *result is not modified; if found, JC_STR__SUCCESSFUL_OP is returned.
```
> int jcstr_indexOfFrontWithInst(const JC_String* const str, const JC_String* const sub, size_t* result)
```text
This is a public library interface. It returns a status code of type int and writes to result the first position of sub-string instance sub, searching from the front.
Note: If result is NULL, JC_STR__NULL_RECEIVER is returned; if either parameter is NULL or has a NULL buffer, the corresponding error code is returned; if sub is an empty string, *result is set to the length of str and JC_STR__SUCCESSFUL_OP is returned; if not found, JC_STR__WITHOUT_CH is returned.
```
> int jcstr_indexOfFrontWithCh(const JC_String* const str, const char* const ch, size_t* result)
```text
This is a public library interface. It returns a status code of type int and writes to result the first position of C string ch, searching from the front.
Note: If result is NULL, JC_STR__NULL_RECEIVER is returned; if str is NULL, JC_STR__NULL is returned; if the buffer or ch is NULL, JC_STR__NULL_BUFFER is returned; if ch is an empty string, *result is set to the length of str and JC_STR__SUCCESSFUL_OP is returned; if not found, JC_STR__WITHOUT_CH is returned.
```
> jc_String jcstr_subInLengthFrontWithInst(const JC_String* const str, size_t start, size_t length)
```text
This is a public library interface. It returns a new string instance taking length characters starting at start from str. The original instance is unchanged.
Note: If start is out of bounds, an empty string instance with status JC_STR__INVALID_START is returned; if length exceeds the remaining length, it is truncated automatically; an empty string or length of 0 returns an empty string instance. The returned instance must be released via jcstr_delete.
```
> jc_String jcstr_subInLengthBackWithInst(const JC_String* const str, size_t end, size_t length)
```text
This is a public library interface. It returns a new string instance taking length characters ending at end from str. The original instance is unchanged.
Note: If end is out of bounds, an empty string instance with status JC_STR__INVALID_END is returned; if length exceeds end+1, it is truncated to the beginning; an empty string or length of 0 returns an empty string instance. The returned instance must be released via jcstr_delete.
```
> jc_String jcstr_subInRangeWithInst(const JC_String* const str, size_t start, size_t end)
```text
This is a public library interface. It returns a new string instance taking the range [start, end) from str. The original instance is unchanged.
Note: If start > end, an empty string instance with status JC_STR__INVALID_IDX is returned; if start or end is out of bounds, an empty string instance with status JC_STR__IDX_OUT_BOUNDS is returned; if start == end, an empty string instance is returned. The returned instance must be released via jcstr_delete.
```
> jc_String jcstr_subInLengthFrontWithCh(const char* const ch, size_t start, size_t length)
```text
This is a public library interface. It returns a new string instance taking length characters starting at start from the C string ch.
Note: If ch is NULL, an instance with status JC_STR__NULL_BUFFER is returned; if start is out of bounds, an empty string instance with status JC_STR__INVALID_START is returned; if length exceeds the remaining length, it is truncated automatically. The returned instance must be released via jcstr_delete.
```
> jc_String jcstr_subInLengthBackWithCh(const char* const ch, size_t end, size_t length)
```text
This is a public library interface. It returns a new string instance taking length characters ending at end from the C string ch.
Note: If ch is NULL, an instance with status JC_STR__NULL_BUFFER is returned; if end is out of bounds, an empty string instance with status JC_STR__INVALID_END is returned; if length exceeds end+1, it is truncated to the beginning. The returned instance must be released via jcstr_delete.
```
> jc_String jcstr_subInRangeWithCh(const char* const ch, size_t start, size_t end)
```text
This is a public library interface. It returns a new string instance taking the range [start, end) from the C string ch.
Note: If ch is NULL, an instance with status JC_STR__NULL_BUFFER is returned; if start > end, an empty string instance with status JC_STR__INVALID_IDX is returned; if out of bounds, an empty string instance with status JC_STR__IDX_OUT_BOUNDS is returned. The returned instance must be released via jcstr_delete.
```
> jc_String jcstr_replaceFirstWithInst(const JC_String* const str, const char* const target, const JC_String* const replacement)
```text
This is a public library interface. It returns a new string instance where the first occurrence of target in str is replaced by the content of replacement. The original instance is unchanged.
Note: If str is NULL, NULL is returned; if str's buffer is invalid, an instance with status JC_STR__NULL_BUFFER is returned; if target is NULL, a copy of the original content with status JC_STR__NULL_TARGET is returned; if replacement is NULL or its buffer is invalid, a copy of the original content with status JC_STR__INVALID_REPLACEMENT is returned; an empty target with non-empty str is equivalent to appending replacement at the end, and with empty str it sets the content to replacement; a non-empty target returns a copy of the original content when str is empty or not found. The returned instance must be released via jcstr_delete.
```
> jc_String jcstr_replaceAllWithInst(const JC_String* const str, const char* const target, const JC_String* const replacement)
```text
This is a public library interface. It returns a new string instance where all occurrences of target in str are replaced by the content of replacement. The original instance is unchanged.
Note: If str is NULL, NULL is returned; if str's buffer is invalid, an instance with status JC_STR__NULL_BUFFER is returned; if target is NULL, a copy of the original content with status JC_STR__NULL_TARGET is returned; if replacement is NULL or its buffer is invalid, a copy of the original content with status JC_STR__INVALID_REPLACEMENT is returned; the empty-target semantics match jcstr_replaceFirstWithInst; a non-empty target returns a copy of the original content when str is empty or not found. The returned instance must be released via jcstr_delete.
```
> jc_String jcstr_replaceFirstWithCh(const JC_String* const str, const char* const target, const char* const replacement)
```text
This is a public library interface. It returns a new string instance where the first occurrence of target in str is replaced by the C string replacement. The original instance is unchanged.
Note: If str is NULL, NULL is returned; if str's buffer is invalid, an instance with status JC_STR__NULL_BUFFER is returned; if target is NULL, a copy of the original content with status JC_STR__NULL_TARGET is returned; if replacement is NULL, a copy of the original content with status JC_STR__INVALID_REPLACEMENT is returned; the empty-target semantics match jcstr_replaceFirstWithInst; a non-empty target returns a copy of the original content when str is empty or not found. The returned instance must be released via jcstr_delete.
```
> jc_String jcstr_replaceAllWithCh(const JC_String* const str, const char* const target, const char* const replacement)
```text
This is a public library interface. It returns a new string instance where all occurrences of target in str are replaced by the C string replacement. The original instance is unchanged.
Note: If str is NULL, NULL is returned; if str's buffer is invalid, an instance with status JC_STR__NULL_BUFFER is returned; if target is NULL, a copy of the original content with status JC_STR__NULL_TARGET is returned; if replacement is NULL, a copy of the original content with status JC_STR__INVALID_REPLACEMENT is returned; the empty-target semantics match jcstr_replaceAllWithInst; a non-empty target returns a copy of the original content when str is empty or not found. The returned instance must be released via jcstr_delete.
```
> bool jcstr_startWithCh(const JC_String* const str, const char* const prefix)
```text
This is a public library interface. It returns a bool indicating whether str starts with the C string prefix.
Note: If str is NULL or its buffer is NULL, false is returned; if prefix is NULL, false is returned; if prefix is an empty string, true is returned.
```
> bool jcstr_endWithCh(const JC_String* const str, const char* const suffix)
```text
This is a public library interface. It returns a bool indicating whether str ends with the C string suffix.
Note: If str is NULL or its buffer is NULL, false is returned; if suffix is NULL, false is returned; if suffix is an empty string, true is returned.
```
> bool jcstr_startWithInst(const JC_String* const str, const JC_String* const prefix)
```text
This is a public library interface. It returns a bool indicating whether str starts with the string instance prefix.
Note: If prefix is NULL, false is returned; other boundary cases are the same as jcstr_startWithCh.
```
> bool jcstr_endWithInst(const JC_String* const str, const JC_String* const suffix)
```text
This is a public library interface. It returns a bool indicating whether str ends with the string instance suffix.
Note: If suffix is NULL, false is returned; other boundary cases are the same as jcstr_endWithCh.
```
> bool jcstr_isEmpty(const JC_String* const str)
```text
This is a public library interface. It returns a bool indicating whether str is an empty string.
Note: It returns true when str is NULL, when its buffer is NULL, or when its content is an empty string.
```
> bool jcstr_isBlank(const JC_String* const str, JC_SpaceMode mode)
```text
This is a public library interface. It returns a bool indicating whether str is an empty string or consists solely of whitespace characters.
Note: Whitespace classification is controlled by mode - JC_SPACE_ASCII uses fixed ASCII whitespace (space, \t, \n, \r, \v, \f); JC_SPACE_LOCALE or unknown modes follow the current locale's isspace; it returns true when str is NULL or its buffer is NULL.
```
> bool jcstr_equalsWithInst(const JC_String* const str1, const JC_String* const str2)
```text
This is a public library interface. It returns a bool indicating whether two string instances are equal.
Note: Both parameters NULL returns true; exactly one NULL returns false; both buffers NULL returns true; exactly one buffer NULL returns false; both empty strings returns true; exactly one empty string returns false; otherwise contents are compared.
```
> bool jcstr_equalsIgnoreCaseWithInst(const JC_String* const str1, const JC_String* const str2, JC_SpaceMode mode)
```text
This is a public library interface. It returns a bool indicating whether two string instances are equal ignoring case.
Note: mode semantics are the same as jcstr_isBlank, controlling whether case folding uses fixed ASCII or follows the locale; NULL/buffer/empty-string handling matches jcstr_equalsWithInst; lengths are compared before content comparison.
```
> bool jcstr_equalsWithCh(const JC_String* const str, const char* const ch)
```text
This is a public library interface. It returns a bool indicating whether the content of the string instance equals the C string ch.
Note: If str is NULL, false is returned; if both str's buffer and ch are NULL, true is returned; if exactly one is NULL, false is returned; if both are empty strings, true is returned; if exactly one is empty, false is returned; otherwise contents are compared.
```
> bool jcstr_equalsIgnoreCaseWithCh(const JC_String* const str, const char* const ch, JC_SpaceMode mode)
```text
This is a public library interface. It returns a bool indicating whether the content of the string instance equals the C string ch ignoring case.
Note: mode semantics are the same as jcstr_isBlank; NULL/buffer/empty-string handling matches jcstr_equalsWithCh; lengths are compared before content comparison.
```
> bool jcstr_containsWithInst(const JC_String* const str1, const JC_String* const str2)
```text
This is a public library interface. It returns a bool indicating whether str1 contains the sub-string str2.
Note: Both parameters NULL returns true; exactly one NULL returns false; both buffers NULL returns true; exactly one buffer NULL returns false; otherwise strstr is used, where an empty needle matches every string.
```
> bool jcstr_containsIgnoreCaseWithInst(const JC_String* const str1, const JC_String* const str2, JC_SpaceMode mode)
```text
This is a public library interface. It returns a bool indicating whether str1 contains the sub-string str2 ignoring case.
Note: mode semantics are the same as jcstr_isBlank; NULL/buffer handling matches jcstr_containsWithInst; an empty needle matches every string.
```
> bool jcstr_containsWithCh(const JC_String* const str, const char* const ch)
```text
This is a public library interface. It returns a bool indicating whether the string instance contains the C string ch.
Note: If str is NULL, false is returned; if both str's buffer and ch are NULL, true is returned; if exactly one is NULL, false is returned; otherwise strstr is used, where an empty ch matches every string.
```
> bool jcstr_containsIgnoreCaseWithCh(const JC_String* const str, const char* const ch, JC_SpaceMode mode)
```text
This is a public library interface. It returns a bool indicating whether the string instance contains the C string ch ignoring case.
Note: mode semantics are the same as jcstr_isBlank; NULL/buffer handling matches jcstr_containsWithCh; an empty ch matches every string.
```
> int jcstr_compareWithInst(const JC_String* const str1, const JC_String* const str2, int* result)
```text
This is a public library interface. It returns a status code of type int and compares two string instances lexicographically, writing the result to result.
Note: If result is NULL, JC_STR__NULL_RECEIVER is returned; if both parameters are NULL, *result is set to 0 and JC_STR__SUCCESSFUL_OP is returned; if exactly one is NULL, JC_STR__NULL is returned; if both buffers are NULL, *result is set to 0 and JC_STR__SUCCESSFUL_OP is returned; if exactly one buffer is NULL, JC_STR__NULL_BUFFER is returned; otherwise the strcmp result is written to *result and JC_STR__SUCCESSFUL_OP is returned.
```
> int jcstr_compareIgnoreCaseWithInst(const JC_String* const str1, const JC_String* const str2, int* result, JC_SpaceMode mode)
```text
This is a public library interface. It returns a status code of type int and compares two string instances lexicographically ignoring case, writing the result to result.
Note: mode semantics are the same as jcstr_isBlank; NULL/buffer handling matches jcstr_compareWithInst; otherwise the case-insensitive comparison result is written to *result and JC_STR__SUCCESSFUL_OP is returned.
```
> int jcstr_compareWithCh(const JC_String* const str, const char* const ch, int* result)
```text
This is a public library interface. It returns a status code of type int and compares the string instance with the C string ch lexicographically, writing the result to result.
Note: If result is NULL, JC_STR__NULL_RECEIVER is returned; if str is NULL, JC_STR__NULL is returned; if both str's buffer and ch are NULL, JC_STR__SUCCESSFUL_OP is returned (*result is not modified); if exactly one is NULL, JC_STR__NULL_BUFFER is returned; otherwise the strcmp result is written to *result and JC_STR__SUCCESSFUL_OP is returned.
```
> int jcstr_compareIgnoreCaseWithCh(const JC_String* const str, const char* const ch, int* result, JC_SpaceMode mode)
```text
This is a public library interface. It returns a status code of type int and compares the string instance with the C string ch lexicographically ignoring case, writing the result to result.
Note: mode semantics are the same as jcstr_isBlank; NULL/buffer handling matches jcstr_compareWithCh; otherwise the case-insensitive comparison result is written to *result and JC_STR__SUCCESSFUL_OP is returned.
```
> char* jcstr_toCharArray(const JC_String* const str)
```text
This is a public library interface. It returns a newly allocated C string copy with the same content as str.
Note: The caller is responsible for freeing the returned pointer; if str is NULL or its buffer is NULL, NULL is returned; an empty string returns a non-NULL, freeable empty string.
```
> int jcstr_toInt(const JC_String* const str, int* result, JC_SpaceMode mode)
```text
This is a public library interface. It returns a status code of type int and parses str as a decimal int, writing the result to result.
Note: mode controls whether leading/trailing whitespace is skipped using fixed ASCII or the locale; a sign is allowed; if str is empty or its buffer is NULL, JC_STR__EMPTY is returned; if the instance status is abnormal, str->status is returned; if result is NULL, JC_STR__NULL_INT is returned; on invalid format, JC_STR__NOT_INT is returned; on overflow, JC_STR__OVERFLOW_INT is returned; on success, JC_STR__SUCCESSFUL_OP is returned.
```
> int jcstr_toLong(const JC_String* const str, long* result, JC_SpaceMode mode)
```text
This is a public library interface. It returns a status code of type int and parses str as a decimal long, writing the result to result.
Note: mode semantics are the same as jcstr_toInt; if str is empty or its buffer is NULL, JC_STR__EMPTY is returned; if the instance status is abnormal, str->status is returned; if result is NULL, JC_STR__NULL_LONG is returned; on invalid format, JC_STR__NOT_LONG is returned; on overflow, JC_STR__OVERFLOW_LONG is returned; on success, JC_STR__SUCCESSFUL_OP is returned.
```
> int jcstr_toLongLong(const JC_String* const str, long long* result, JC_SpaceMode mode)
```text
This is a public library interface. It returns a status code of type int and parses str as a decimal long long, writing the result to result.
Note: mode semantics are the same as jcstr_toInt; if str is empty or its buffer is NULL, JC_STR__EMPTY is returned; if the instance status is abnormal, str->status is returned; if result is NULL, JC_STR__NULL_LONGLONG is returned; on invalid format, JC_STR__NOT_LONGLONG is returned; on overflow, JC_STR__OVERFLOW_LONGLONG is returned; on success, JC_STR__SUCCESSFUL_OP is returned.
```
> int jcstr_toFloat(const JC_String* const str, float* result, JC_SpaceMode mode)
```text
This is a public library interface. It returns a status code of type int and parses str as a float, writing the result to result.
Note: mode semantics are the same as jcstr_toInt; the implementation uses strtod, so decimals, exponents and other formats allowed by the C standard (including hexadecimal floats and inf/nan) are accepted; if str is empty or its buffer is NULL, JC_STR__EMPTY is returned; if the instance status is abnormal, str->status is returned; if result is NULL, JC_STR__NULL_FLOAT is returned; on invalid format, JC_STR__NOT_FLOAT is returned; when it exceeds the float range, JC_STR__OVERFLOW_FLOAT is returned; on success, JC_STR__SUCCESSFUL_OP is returned.
```
> int jcstr_toDouble(const JC_String* const str, double* result, JC_SpaceMode mode)
```text
This is a public library interface. It returns a status code of type int and parses str as a double, writing the result to result.
Note: mode semantics are the same as jcstr_toInt; the implementation uses strtod, so decimals, exponents and other formats allowed by the C standard (including hexadecimal floats and inf/nan) are accepted; if str is empty or its buffer is NULL, JC_STR__EMPTY is returned; if the instance status is abnormal, str->status is returned; if result is NULL, JC_STR__NULL_DOUBLE is returned; on invalid format, JC_STR__NOT_DOUBLE is returned; when it exceeds the double range, JC_STR__OVERFLOW_DOUBLE is returned; on success, JC_STR__SUCCESSFUL_OP is returned.
```
> jc_String jcstr_toLowerCase(const JC_String* const str, JC_SpaceMode mode)
```text
This is a public library interface. It returns a new string instance whose content is str converted to lower case. The original instance is unchanged.
Note: mode semantics are the same as jcstr_isBlank, controlling whether case folding uses fixed ASCII or follows the locale; if str is NULL, NULL is returned; if its buffer is NULL, an instance with status JC_STR__NULL_BUFFER is returned; an empty string returns an empty string instance. The returned instance must be released via jcstr_delete.
```
> jc_String jcstr_toUpperCase(const JC_String* const str, JC_SpaceMode mode)
```text
This is a public library interface. It returns a new string instance whose content is str converted to upper case. The original instance is unchanged.
Note: mode semantics are the same as jcstr_isBlank, controlling whether case folding uses fixed ASCII or follows the locale; if str is NULL, NULL is returned; if its buffer is NULL, an instance with status JC_STR__NULL_BUFFER is returned; an empty string returns an empty string instance. The returned instance must be released via jcstr_delete.
```
> jc_MutableString jcstr_toMutableString(const JC_String* const str)
```text
This is a public library interface. It returns a mutable string instance with the same content as str. The original instance is unchanged.
Note: If str is NULL, NULL is returned; if str's buffer is NULL, a mutable string instance with status NULL_BUFFER is returned; the returned instance must be released via the corresponding mutable string release interface.
```