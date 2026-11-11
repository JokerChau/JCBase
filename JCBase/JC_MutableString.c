#include "JC_MutableString.h"
#include "JC_StringStructures.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <float.h>
#include "JC_String.h"

// 直接接管各种属性
static jc_MutableString wrapString(char* buffer, int status, size_t len, size_t cap) {
	jc_MutableString str = (jc_MutableString)malloc(sizeof(JC_MutableString));
	if (!str)return NULL;
	str->buffer = buffer;
	str->status = status;
	str->len = len;
	str->cap = cap;
	return str;
}

static void setString(jc_MutableString ms, char* buffer, int status, size_t len, size_t cap) {
	ms->buffer = buffer;
	ms->status = status;
	ms->len = len;
	ms->cap = cap;
}

// 调用方 保证 content 有效, 但是接口会使用副本而不是接管 content， len 已算出
static jc_MutableString pkgMS(const char* content, size_t len) {
	size_t base = len > (size_t)INITIAL_MS_SIZE ? len : (size_t)INITIAL_MS_SIZE;
	size_t cap = base + 1;                       // 与 jcms_newFrom 同口径

	char* tempBuf = (char*)malloc(cap);
	if (!tempBuf) return wrapString(NULL, JC_MS__NULL_BUFFER, 0, 0);

	memcpy(tempBuf, content, len);
	tempBuf[len] = '\0';

	jc_MutableString result = wrapString(tempBuf, JC_MS__AVAILABLE, len, cap);
	if (!result) free(tempBuf);
	return result;
}

static size_t ceilAlignInitSize(size_t needed) {
	if (needed > SIZE_MAX - INITIAL_MS_SIZE + 1)return needed;
	return ((needed + INITIAL_MS_SIZE - 1) / INITIAL_MS_SIZE) * INITIAL_MS_SIZE;
}

// 调用方 保证实例和 content 有效，但接口不让实例接管 content，而是使用副本
static int setContent(jc_MutableString ms, const char* const content, size_t contentLen) {
	size_t needed = contentLen + 1;

	if (ms->cap < needed) {
		size_t newCap = ceilAlignInitSize(needed);
		char* newBuf = (char*)realloc(ms->buffer, newCap);
		if (!newBuf)return JC_MS__REALLOCATION_FAILED;

		ms->buffer = newBuf;
		ms->cap = newCap;
	}
	memmove(ms->buffer, content, needed);
	ms->len = contentLen;
	return JC_MS__SUCCESSFUL_OP;
}

// 拷贝src
static int appendCore(jc_MutableString ms, char** src, size_t srcLen) {
	if (ms->len > SIZE_MAX - srcLen - 1)return JC_MS__LENGTH_OUT_OF_BOUNDS;
	size_t needed = ms->len + srcLen + 1;
	// 扩容
	if (needed > ms->cap) {
		size_t newCap;

		if (srcLen >= ms->cap) {
			newCap = ceilAlignInitSize(needed);
		}
		else if (srcLen < ms->cap / 4) {
			if (ms->cap > SIZE_MAX - ms->cap / 4)newCap = ceilAlignInitSize(needed);
			else {
				newCap = ms->cap + ms->cap / 4;
				if (newCap < needed)newCap = ceilAlignInitSize(needed);
				else newCap = ceilAlignInitSize(newCap);
			}
		}
		else {
			if (ms->cap > SIZE_MAX / 2)newCap = ceilAlignInitSize(needed);
			else {
				newCap = ms->cap * 2;
				if (newCap < needed)newCap = ceilAlignInitSize(needed);
				else newCap = ceilAlignInitSize(newCap);
			}
		}
		char* tempBuf = (char*)realloc(ms->buffer, newCap);
		if (!tempBuf)return JC_MS__REALLOCATION_FAILED;
		ms->buffer = tempBuf;
		ms->cap = newCap;
	}
	memmove(ms->buffer + ms->len, *src, srcLen);
	ms->len = needed - 1;
	ms->buffer[ms->len] = '\0';
	return JC_MS__SUCCESSFUL_OP;
}

static int replaceFirstCore(jc_MutableString ms, const char* const target, const char* const replacement) {
	// replacement 指向 ms->buffer 内部时，后续 realloc 会让它悬空，直接拒绝
	{
		uintptr_t b = (uintptr_t)ms->buffer;
		uintptr_t s = (uintptr_t)replacement;
		if (s >= b && s <= b + ms->len) return JC_MS__INVALID_REPLACEMENT;
	}

	// 空内容 + 空 target：整体设为 replacement
	if (!ms->len && target[0] == '\0') return setContent(ms, replacement, strlen(replacement));
	// 空内容 + 非空 target：找不到
	if (!ms->len) return JC_MS__NO_TARGET_MATCHED;
	// 非空内容 + 空 target：末尾追加
	if (target[0] == '\0') {
		char* localReplacement = (char*)replacement;
		return appendCore(ms, &localReplacement, strlen(replacement));
	}

	char* indexChar = strstr(ms->buffer, target);
	if (!indexChar) return JC_MS__NO_TARGET_MATCHED;

	size_t index = (size_t)(indexChar - ms->buffer);
	size_t targetLen = strlen(target);
	size_t replaceLen = strlen(replacement);
	size_t strLen = ms->len;

	// 等长：直接覆盖，不需要移动后缀
	if (targetLen == replaceLen) {
		memmove(ms->buffer + index, replacement, replaceLen);
		return JC_MS__SUCCESSFUL_OP;
	}

	// 不等长：算新长度，按需扩容
	size_t preLen = index;
	size_t sufLen = strLen - preLen - targetLen;
	size_t newLen = preLen + replaceLen + sufLen;
	size_t needed = newLen + 1;

	if (ms->cap < needed) {
		char* newBuffer = (char*)realloc(ms->buffer, needed);
		if (!newBuffer) return JC_MS__REALLOCATION_FAILED;
		ms->buffer = newBuffer;
		ms->cap = needed;
	}

	// 先移后缀（含 '\0'），再写 replacement
	memmove(ms->buffer + index + replaceLen, ms->buffer + index + targetLen, sufLen + 1);
	memmove(ms->buffer + index, replacement, replaceLen);

	ms->len = newLen;
	return JC_MS__SUCCESSFUL_OP;
}

static int replaceAllCore(jc_MutableString ms, const char* const target, const char* const replacement) {
	// replacement 指向 ms->buffer 内部时，后续 realloc 会让它悬空，直接拒绝
	{
		uintptr_t b = (uintptr_t)ms->buffer;
		uintptr_t s = (uintptr_t)replacement;
		if (s >= b && s <= b + ms->len) return JC_MS__INVALID_REPLACEMENT;
	}

	// 空内容 + 空 target：整体设为 replacement
	if (!ms->len && target[0] == '\0') return setContent(ms, replacement, strlen(replacement));
	// 空内容 + 非空 target：找不到
	if (!ms->len) return JC_MS__NO_TARGET_MATCHED;
	// 非空内容 + 空 target：末尾追加
	if (target[0] == '\0') {
		char* localReplacement = (char*)replacement;
		return appendCore(ms, &localReplacement, strlen(replacement));
	}

	size_t strLen = ms->len;
	size_t targetLen = strlen(target);
	size_t replaceLen = strlen(replacement);

	// 第一遍：扫描所有匹配位置
	size_t capacity = 16;
	size_t* positions = (size_t*)malloc(capacity * sizeof(size_t));
	if (!positions) return JC_MS__ALLOCATION_FAILED;

	size_t count = 0;
	size_t scan = 0;
	while (scan + targetLen <= strLen) {
		if (memcmp(ms->buffer + scan, target, targetLen) == 0) {
			if (count == capacity) {
				if (capacity > SIZE_MAX / 2 / sizeof(size_t)) { 
					free(positions);
					return JC_MS__REALLOCATION_FAILED;
				}
				capacity *= 2;
				size_t* newPositions = (size_t*)realloc(positions, capacity * sizeof(size_t));
				if (!newPositions) {
					free(positions);
					return JC_MS__REALLOCATION_FAILED;
				}
				positions = newPositions;
			}
			positions[count++] = scan;
			scan += targetLen;
		}
		else {
			scan++;
		}
	}

	if (count == 0) {
		free(positions);
		return JC_MS__NO_TARGET_MATCHED;
	}

	// 计算最终长度
	size_t newLen;
	if (replaceLen >= targetLen) {
		newLen = strLen + count * (replaceLen - targetLen);
	}
	else {
		newLen = strLen - count * (targetLen - replaceLen);
	}

	// 需要时一次扩容到位
	if (newLen + 1 > ms->cap) {
		char* newBuffer = (char*)realloc(ms->buffer, newLen + 1);
		if (!newBuffer) {
			free(positions);
			return JC_MS__REALLOCATION_FAILED;
		}
		ms->buffer = newBuffer;
		ms->cap = newLen + 1;
	}

	// 从后往前替换
	size_t currentLen = strLen;
	for (size_t k = count; k > 0; k--) {
		size_t pos = positions[k - 1];
		size_t suffixStart = pos + targetLen;
		size_t suffixLen = currentLen - suffixStart;

		// 先移后缀，再写 replacement
		memmove(ms->buffer + pos + replaceLen,
			ms->buffer + suffixStart,
			suffixLen + 1);
		memmove(ms->buffer + pos, replacement, replaceLen);

		if (replaceLen >= targetLen) {
			currentLen += replaceLen - targetLen;
		}
		else {
			currentLen -= targetLen - replaceLen;
		}
	}

	free(positions);
	ms->len = newLen;
	ms->buffer[newLen] = '\0';
	return JC_MS__SUCCESSFUL_OP;
}

// 解析 buffer 的 [0, len) 为一个 long long
static int parseToLL(const char* buf, size_t len, long long* out, JC_SpaceMode mode) {
	size_t start = 0;
	while (start < len && isSpaceInternal((unsigned char)buf[start], mode)) start++;
	size_t end = len;
	while (end > start && isSpaceInternal((unsigned char)buf[end - 1], mode)) end--;
	if (start == end) return JC_MS__BLANK;

	bool isNeg = false;
	size_t i = start;
	if (buf[i] == '-' || buf[i] == '+') {
		if (buf[i] == '-') isNeg = true;
		i++;
	}

	long long limit = isNeg ? LLONG_MIN : LLONG_MIN + 1;
	long long temp = 0;
	int count = 0;

	for (; i < end; i++) {
		if (buf[i] >= '0' && buf[i] <= '9') {
			int d = buf[i] - '0';
			if (temp < (limit + d) / 10) return JC_MS__OVERFLOW_LONGLONG;
			temp = 10 * temp - d;
			count++;
		}
		else {
			return JC_MS__NOT_LONGLONG;
		}
	}

	if (!count) return JC_MS__NOT_LONGLONG;
	*out = isNeg ? temp : -temp;
	return JC_MS__SUCCESSFUL_OP;
}

static int parseToDouble(const char* buf, size_t len, double* out, JC_SpaceMode mode) {
	size_t start = 0;
	while (start < len && isSpaceInternal((unsigned char)buf[start], mode)) start++;
	size_t end = len;
	while (end > start && isSpaceInternal((unsigned char)buf[end - 1], mode)) end--;
	if (start == end) return JC_MS__BLANK;

	char* endptr = NULL;
	errno = 0;
	double value = strtod(buf + start, &endptr);

	if (endptr != buf + end) return JC_MS__NOT_DOUBLE;
	if (errno == ERANGE)     return JC_MS__OVERFLOW_DOUBLE;

	*out = value;
	return JC_MS__SUCCESSFUL_OP;
}

// mutableString构造器
// enable
jc_MutableString jcms_new(void) {
	char* tempBuf = (char*)calloc(INITIAL_MS_SIZE + 1, sizeof(char));
	if (!tempBuf)return wrapString(tempBuf, JC_MS__NULL_BUFFER, 0, 0);
	jc_MutableString result = wrapString(tempBuf, JC_MS__AVAILABLE, 0, INITIAL_MS_SIZE + 1);
	if (!result)free(tempBuf);
	return result;
}

// mutableString构造器
// enable
jc_MutableString jcms_newFrom(const char* const content) {
	if (!content)return wrapString(content, JC_MS__NULL_BUFFER, 0, 0);
	return pkgMS(content, strlen(content));
}

// enable
void jcms_delete(jc_MutableString ms) {
	if (!ms)return;
	if (ms->buffer) {
		free(ms->buffer);
	}
	free(ms);
}

// enable
int jcms_getStatus(const JC_MutableString* const ms) {
	if (!ms)return JC_MS__NULL;
	return ms->status;
}

// enable
size_t jcms_getLength(const JC_MutableString* const ms) {
	if (!ms)return 0;
	return ms->len;
}

// enable
const char* jcms_viewBuffer(const JC_MutableString* const ms) {
	if (!ms)return NULL;
	return ms->buffer;
}

// enable
int jcms_setContent(const jc_MutableString ms, const char* const content) {
	if (!ms)return JC_MS__NULL;
	if (ms->status != JC_MS__AVAILABLE)return ms->status;

	// 内容为 NULL → 清空，不释放 buffer
	if (!content) {
		ms->buffer[0] = '\0';
		ms->len = 0;
		return JC_MS__SUCCESSFUL_OP;
	}

	return setContent(ms, content, strlen(content));
}

// enable
int jcms_changeAt(const jc_MutableString ms, size_t index, char ch) {
	if (!ms)return JC_MS__NULL;
	if (!ms->buffer)return JC_MS__NULL_BUFFER;
	if (ms->status != JC_MS__AVAILABLE) return ms->status;
	if (index >= ms->len)return JC_MS__INDEX_OUT_OF_BOUNDS;
	if (ch == '\0')return JC_MS__INVALID_CHARACTER_FOR_CHANGE;

	ms->buffer[index] = ch;

	return JC_MS__SUCCESSFUL_OP;
}

const char* jcms_statusToCharArray(const int status) {
	switch (status) {
		// 实例状态
	case JC_MS__NULL:                       return "null mutableString";
	case JC_MS__AVAILABLE:                  return "available mutableString";
	case JC_MS__NULL_BUFFER:                return "null buffer";
	case JC_MS__EMPTY:                      return "empty mutableString";
	case JC_MS__BLANK:                      return "blank mutableString";

		// 操作成功
	case JC_MS__SUCCESSFUL_OP:              return "successful operation";
	case JC_MS__PARTIAL_SPLIT:              return "mutableString partial split";

		// 内存/操作错误
	case JC_MS__ALLOCATION_FAILED:          return "failed to alloc";
	case JC_MS__REALLOCATION_FAILED:        return "failed to realloc";
	case JC_MS__LENGTH_OUT_OF_BOUNDS:       return "length out of bounds";
	case JC_MS__WITHOUT_CHAR:               return "there is no char you want here";
	case JC_MS__NO_TARGET_MATCHED:          return "target not matched";

		// 参数错误
	case JC_MS__INVALID_START:              return "invalid start index";
	case JC_MS__INVALID_END:                return "invalid end index";
	case JC_MS__INVALID_INDEX:              return "invalid index";
	case JC_MS__INDEX_OUT_OF_BOUNDS:        return "index out of bounds";
	case JC_MS__INVALID_CHARACTER_FOR_CHANGE: return "invalid char for change";
	case JC_MS__NULL_CHAR:                  return "null char pointer";
	case JC_MS__INVALID_SOURCE:             return "invalid source pointer";
	case JC_MS__NULL_ARRAY:                 return "null mutableString array";
	case JC_MS__NULL_COUNT:                 return "count ptr null";
	case JC_MS__NULL_RESULT:                return "result ptr null";
	case JC_MS__NULL_TARGET:                return "target is null";
	case JC_MS__INVALID_REPLACEMENT:        return "replacement is invalid";
	case JC_MS__NULL_FOLLOWER:              return "follower instance is null";
	case JC_MS__NULL_FOLLOWER_BUFFER:       return "follower buffer is null";
	case JC_MS__NULL_RECEIVER:              return "receiver is null";

		// 数字解析：输出指针为 NULL
	case JC_MS__NULL_INT:                   return "null int pointer";
	case JC_MS__NULL_LONG:                  return "null long int pointer";
	case JC_MS__NULL_LONGLONG:              return "null long long int pointer";
	case JC_MS__NULL_FLOAT:                 return "null float pointer";
	case JC_MS__NULL_DOUBLE:                return "null double pointer";

		// 数字解析：格式错误
	case JC_MS__NOT_INT:                    return "not int";
	case JC_MS__NOT_LONG:                   return "not long int";
	case JC_MS__NOT_LONGLONG:               return "not long long int";
	case JC_MS__NOT_FLOAT:                  return "not float";
	case JC_MS__NOT_DOUBLE:                 return "not double";

		// 数字解析：溢出
	case JC_MS__OVERFLOW_INT:               return "overflow int range";
	case JC_MS__OVERFLOW_LONG:              return "overflow long int range";
	case JC_MS__OVERFLOW_LONGLONG:          return "overflow long long int range";
	case JC_MS__OVERFLOW_FLOAT:             return "overflow float range";
	case JC_MS__OVERFLOW_DOUBLE:            return "overflow double range";

	default:                                return "Unknown status";
	}
}

// enable
int jcms_appendWithInst(const jc_MutableString ms, const JC_MutableString* const src) {
	if (!ms)return JC_MS__NULL;
	if (!ms->buffer)return JC_MS__NULL_BUFFER;
	if (!src)return JC_MS__NULL_FOLLOWER;
	if (!src->buffer)return JC_MS__NULL_FOLLOWER_BUFFER;
	if (!src->len)return JC_MS__SUCCESSFUL_OP;
	if (!ms->len)return jcms_setContent(ms, src->buffer);
	// 如果都不为空

	return appendCore(ms, &src->buffer, src->len);
}

// enable
int jcms_appendWithCh(const jc_MutableString ms, const char* const ch) {
	if (!ms)return JC_MS__NULL;
	if (!ms->buffer)return JC_MS__NULL_BUFFER;
	if (!ch)return JC_MS__NULL_FOLLOWER;
	uintptr_t bufStart = (uintptr_t)ms->buffer;
	uintptr_t bufEnd = bufStart + ms->len;   // buffer + len 是有效地址，不溢出
	uintptr_t srcPtr = (uintptr_t)ch;
	if (srcPtr >= bufStart && srcPtr <= bufEnd) return JC_MS__INVALID_SOURCE;
	if (ch[0] == '\0')return JC_MS__SUCCESSFUL_OP;
	if (!ms->len)return jcms_setContent(ms, ch);
	char* localCh = (char*)ch;

	return appendCore(ms, &localCh, strlen(ch));
}

// enable
int jcms_reverse(jc_MutableString ms) {
	if (!ms || !ms->buffer || ms->buffer[0]=='\0')return JC_MS__SUCCESSFUL_OP;

	for (size_t i = 0; i < ms->len / 2; i++) {
		char c = ms->buffer[i];
		ms->buffer[i] = ms->buffer[ms->len - 1 - i];
		ms->buffer[ms->len - 1 - i] = c;
	}

	return JC_MS__SUCCESSFUL_OP;
}

// enable
int jcms_split(const JC_MutableString* const ms, char delimiter,
	jc_MutableString** outArray, size_t* count,
	bool removeEmptyEntries) {
	if (!outArray) return JC_MS__NULL_ARRAY;
	if (!count)    return JC_MS__NULL_COUNT;

	*outArray = NULL;
	*count = 0;

	if (!ms)         return JC_MS__NULL;
	if (!ms->buffer) return JC_MS__NULL_BUFFER;

	const char* buf = ms->buffer;
	const char* end = buf + ms->len;

	/* 第一遍：统计段数 */
	size_t wholeCount = 0;
	const char* p = buf;
	for (;;) {
		const char* found = (const char*)memchr(p, (unsigned char)delimiter,
			(size_t)(end - p));
		const char* segEnd = found ? found : end;
		size_t segLen = (size_t)(segEnd - p);

		if (segLen > 0 || !removeEmptyEntries) wholeCount++;
		if (!found) break;
		p = found + 1;
	}

	if (wholeCount == 0) return JC_MS__SUCCESSFUL_OP;

	jc_MutableString* arr = (jc_MutableString*)malloc(sizeof(jc_MutableString) * wholeCount);
	if (!arr) return JC_MS__ALLOCATION_FAILED;

	/* 第二遍：填充，每段走 pkgMS */
	bool   hasNull = false;
	size_t i = 0;
	p = buf;
	for (;;) {
		const char* found = (const char*)memchr(p, (unsigned char)delimiter,
			(size_t)(end - p));
		const char* segEnd = found ? found : end;
		size_t segLen = (size_t)(segEnd - p);

		if (segLen > 0 || !removeEmptyEntries) {
			jc_MutableString item = pkgMS(p, segLen);
			if (!item || item->status != JC_MS__AVAILABLE) hasNull = true;
			arr[i++] = item;
		}

		if (!found) break;
		p = found + 1;
	}

	*outArray = arr;
	*count = wholeCount;
	return hasNull ? JC_MS__PARTIAL_SPLIT : JC_MS__SUCCESSFUL_OP;
}

// enable
int jcms_trim(const jc_MutableString ms, JC_SpaceMode mode) {
	if (!ms || !ms->buffer || !ms->len)return JC_MS__SUCCESSFUL_OP;

	size_t len = ms->len;
	size_t start = 0;
	while (start < len && isSpaceInternal((unsigned char)ms->buffer[start], mode)) {
		start++;
	}

	if (start == len) {
		ms->buffer[0] = '\0';
		ms->len = 0;
		return JC_MS__SUCCESSFUL_OP;
	}

	size_t end = len - 1;
	while (end > start && isSpaceInternal((unsigned char)ms->buffer[end], mode)) {
		end--;
	}

	size_t newLen = end - start + 1;
	if (start == 0 && newLen == len)return JC_MS__SUCCESSFUL_OP;

	if (start > 0) {
		memmove(ms->buffer, ms->buffer + start, newLen);
	}
	ms->buffer[newLen] = '\0';
	ms->len = newLen;
	
	return JC_MS__SUCCESSFUL_OP;
}

// enable
int jcms_trimFront(const jc_MutableString ms, JC_SpaceMode mode) {
	if (!ms || !ms->buffer || !ms->len)return JC_MS__SUCCESSFUL_OP;

	size_t len = ms->len;
	size_t start = 0;
	while (start < len && isSpaceInternal((unsigned char)ms->buffer[start], mode)) {
		start++;
	}

	if (start == 0)return JC_MS__SUCCESSFUL_OP;
	if (start == len) {
		ms->buffer[0] = '\0';
		ms->len = 0;
		return JC_MS__SUCCESSFUL_OP;
	}

	size_t newLen = len - start;
	
	memmove(ms->buffer, ms->buffer + start, newLen);

	ms->buffer[newLen] = '\0';
	ms->len = newLen;

	return JC_MS__SUCCESSFUL_OP;
}

// enable
int jcms_trimBack(const jc_MutableString ms, JC_SpaceMode mode) {
	if (!ms || !ms->buffer || !ms->len)return JC_MS__SUCCESSFUL_OP;

	size_t newLen = ms->len;
	while (newLen > 0 && isSpaceInternal((unsigned char)ms->buffer[newLen - 1], mode))newLen--;

	if (newLen == ms->len)return JC_MS__SUCCESSFUL_OP;

	ms->buffer[newLen] = '\0';
	ms->len = newLen;

	return JC_MS__SUCCESSFUL_OP;
}

// enable
jc_MutableString jcms_clone(const JC_MutableString* const ms) {
	if (!ms)return NULL;
	return jcms_newFrom(ms->buffer);
}

// enable
int jcms_charAt(const JC_MutableString* const ms, size_t index, char* result) {
	if (!result)return JC_MS__NULL_CHAR;
	if (!ms)return JC_MS__NULL;
	if (!ms->buffer)return JC_MS__NULL_BUFFER;
	if (index >= ms->len)return JC_MS__INDEX_OUT_OF_BOUNDS;

	*result = ms->buffer[index];

	return JC_MS__SUCCESSFUL_OP;
}

// enable
int jcms_indexOfFront(const JC_MutableString* const ms, char ch, size_t* result) {
	if (!result)return JC_MS__NULL_RESULT;
	if (!ms)return JC_MS__NULL;
	if (!ms->buffer)return JC_MS__NULL_BUFFER;

	const char* found = strchr(ms->buffer, ch);
	if (found) {
		*result = (size_t)(found - ms->buffer);
		return JC_MS__SUCCESSFUL_OP;
	}
	return JC_MS__WITHOUT_CHAR;
}

// enable
int jcms_indexOfBack(const JC_MutableString* const ms, char ch, size_t* result) {
	if (!result)return JC_MS__NULL_RESULT;
	if (!ms)return JC_MS__NULL;
	if (!ms->buffer)return JC_MS__NULL_BUFFER;
	for (ptrdiff_t i = (ptrdiff_t)ms->len - 1; i >= 0; --i) {
		if (ms->buffer[i] == ch) {
			*result = (size_t)i;
			return JC_MS__SUCCESSFUL_OP;
		}
	}
	return JC_MS__WITHOUT_CHAR;
}

// enable
int jcms_indexOfFrontWithInst(const JC_MutableString* const ms, const JC_MutableString* const sub, size_t* result) {
	if (!result)return JC_MS__NULL_RESULT;
	if (!ms || !sub)return JC_MS__NULL;
	if (!ms->buffer || !sub->buffer)return JC_MS__NULL_BUFFER;
	if (sub->buffer[0] == '\0') {
		*result = ms->len;
		return JC_MS__SUCCESSFUL_OP;
	}
	const char* found = strstr(ms->buffer, sub->buffer);
	if (!found)return JC_MS__WITHOUT_CHAR;
	*result = (size_t)(found - ms->buffer);
	return JC_MS__SUCCESSFUL_OP;
}

// enable
int jcms_indexOfFrontWithCh(const JC_MutableString* const ms, const char* const ch, size_t* result) {
	if (!result)return JC_MS__NULL_RESULT;
	if (!ms)return JC_MS__NULL;
	if (!ms->buffer || !ch)return JC_MS__NULL_BUFFER;
	if (ch[0] == '\0') {
		*result = ms->len;
		return JC_MS__SUCCESSFUL_OP;
	}
	const char* found = strstr(ms->buffer, ch);
	if (!found)return JC_MS__WITHOUT_CHAR;
	*result = (size_t)(found - ms->buffer);
	return JC_MS__SUCCESSFUL_OP;
}

// enable
jc_MutableString jcms_subInLengthFrontWithInst(const JC_MutableString* const ms, size_t start, size_t length) {
	if (!ms)return NULL;
	if (!ms->buffer)return jcms_newFrom(NULL);
	if (!ms->len || !length)return jcms_newFrom("");

	size_t len = ms->len;
	// 起始点超限
	if (start >= len) {
		jc_MutableString temp = jcms_newFrom("");
		if (temp && temp->status == JC_MS__AVAILABLE)temp->status = JC_MS__INVALID_START;
		return temp;
	}

	// 长度超限就整合长度
	if (length > len - start)length = len - start;

	return pkgMS(ms->buffer + start, length);
}

// enable
jc_MutableString jcms_subInLengthBackWithInst(const JC_MutableString* const ms, size_t end, size_t length) {
	if (!ms)return NULL;
	if (!ms->buffer)return jcms_newFrom(NULL);
	if (ms->buffer[0] == '\0' || !length)return jcms_newFrom("");

	size_t len = ms->len;
	if (end >= len) {
		jc_MutableString temp = jcms_newFrom("");
		if (temp && temp->status == JC_MS__AVAILABLE)temp->status = JC_MS__INVALID_END;
		return temp;
	}

	if ((ptrdiff_t)end + 1 - (ptrdiff_t)length < 0)length = end + 1;

	return pkgMS(ms->buffer + (end - length + 1), length);
}

// enable
jc_MutableString jcms_subInRangeWithInst(const JC_MutableString* const ms, size_t start, size_t end) {
	if (!ms)return NULL;
	if (!ms->buffer)return jcms_newFrom(NULL);
	if (ms->buffer[0] == '\0' || start == end)return jcms_newFrom("");

	if (start > end) {
		jc_MutableString temp = jcms_newFrom("");
		if (temp && temp->status == JC_MS__AVAILABLE)temp->status = JC_MS__INVALID_INDEX;
		return temp;
	}

	size_t len = ms->len;
	if (start >= len || end > len) {
		jc_MutableString temp = jcms_newFrom("");
		if (temp && temp->status == JC_MS__AVAILABLE)temp->status = JC_MS__INDEX_OUT_OF_BOUNDS;
		return temp;
	}

	size_t length = end - start;
	return pkgMS(ms->buffer + start, length);
}

// enable
jc_MutableString jcms_subInLengthFrontWithCh(const char* const ch, size_t start, size_t length) {
	if (!ch)return jcms_newFrom(NULL);
	if (ch[0] == '\0' || !length)return jcms_newFrom("");

	size_t len = strlen(ch);
	if (start >= len) {
		jc_MutableString temp = jcms_newFrom("");
		if (temp && temp->status == JC_MS__AVAILABLE)temp->status = JC_MS__INVALID_START;
		return temp;
	}

	if (length > len - start)length = len - start;

	return pkgMS(ch + start, length);
}

// enable
jc_MutableString jcms_subInLengthBackWithCh(const char* const ch, size_t end, size_t length) {
	if (!ch)return jcms_newFrom(NULL);
	if (ch[0] == '\0' || !length)return jcms_newFrom("");

	size_t len = strlen(ch);
	if (end >= len) {
		jc_MutableString temp = jcms_newFrom("");
		if (temp && temp->status == JC_MS__AVAILABLE)temp->status = JC_MS__INVALID_END;
		return temp;
	}

	if ((ptrdiff_t)end + 1 - (ptrdiff_t)length < 0)length = end + 1;

	return pkgMS(ch + (end - length + 1), length);
}

// enable
jc_MutableString jcms_subInRangeWithCh(const char* const ch, size_t start, size_t end) {
	if (!ch)return jcms_newFrom(NULL);
	if (ch[0] == '\0' || start == end)return jcms_newFrom("");

	if (start > end) {
		jc_MutableString temp = jcms_newFrom("");
		if (temp && temp->status == JC_MS__AVAILABLE)temp->status = JC_MS__INVALID_INDEX;
		return temp;
	}

	size_t len = strlen(ch);
	if (start >= len || end > len) {
		jc_MutableString temp = jcms_newFrom("");
		if (temp && temp->status == JC_MS__AVAILABLE)temp->status = JC_MS__INDEX_OUT_OF_BOUNDS;
		return temp;
	}

	size_t length = end - start;
	return pkgMS(ch + start, length);
}

// enable
int jcms_replaceFirstWithInst(JC_MutableString* ms, const char* const target, const JC_MutableString* const replacement) {
	if (!ms)return JC_MS__NULL;
	if (ms->status != JC_MS__AVAILABLE)return ms->status;
	if (!target)return JC_MS__NULL_TARGET;
	if (!replacement || !replacement->buffer)return JC_MS__INVALID_REPLACEMENT;
	return replaceFirstCore(ms, target, replacement->buffer);
}

// enable
int jcms_replaceFirstWithCh(JC_MutableString* ms, const char* const target, const char* const replacement) {
	if (!ms)return JC_MS__NULL;
	if (ms->status != JC_MS__AVAILABLE)return ms->status;
	if (!target)return JC_MS__NULL_TARGET;
	if (!replacement)return JC_MS__INVALID_REPLACEMENT;

	return replaceFirstCore(ms, target, replacement);
}

// enable
int jcms_replaceAllWithInst(JC_MutableString* ms, const char* const target, const JC_MutableString* const replacement) {
	if (!ms) return JC_MS__NULL;
	if (ms->status != JC_MS__AVAILABLE) return ms->status;
	if (!target) return JC_MS__NULL_TARGET;
	if (!replacement || !replacement->buffer) return JC_MS__INVALID_REPLACEMENT;
	return replaceAllCore(ms, target, replacement->buffer);
}

// enable
int jcms_replaceAllWithCh(JC_MutableString* ms, const char* const target, const char* const replacement) {
	if (!ms) return JC_MS__NULL;
	if (ms->status != JC_MS__AVAILABLE) return ms->status;
	if (!target) return JC_MS__NULL_TARGET;
	if (!replacement) return JC_MS__INVALID_REPLACEMENT;

	return replaceAllCore(ms, target, replacement);
}

// enable
bool jcms_startWithCh(const JC_MutableString* const ms, const char* const prefix) {
	if (!ms || !ms->buffer)return false;

	if (!prefix)return false;
	if (prefix[0] == '\0')return true;

	size_t strLen = ms->len;
	size_t preLen = strlen(prefix);
	if (strLen < preLen)return false;

	return strncmp(ms->buffer, prefix, preLen) == 0;
}

// enable
bool jcms_startWithInst(const JC_MutableString* const ms, const JC_MutableString* const prefix) {
	if (!prefix)return false;
	return jcms_startWithCh(ms, prefix->buffer);
}

// enable
bool jcms_endWithCh(const JC_MutableString* const ms, const char* const suffix) {
	if (!ms || !ms->buffer)return false;

	if (!suffix)return false;

	if (suffix[0] == '\0')return true;

	size_t strLen = ms->len;
	size_t sufLen = strlen(suffix);
	if (strLen < sufLen)return false;

	return strncmp(ms->buffer + (strLen - sufLen), suffix, sufLen) == 0;
}

// enable
bool jcms_endWithInst(const JC_MutableString* const ms, const JC_MutableString* const suffix) {
	if (!suffix)return false;
	return jcms_endWithCh(ms, suffix->buffer);
}

// enable
bool jcms_isEmpty(const JC_MutableString* const ms) {
	return !ms || !ms->buffer || ms->buffer[0] == '\0' ? true : false;
}

bool jcms_isBlank(const JC_MutableString* const ms, JC_SpaceMode mode) {
	if (jcms_isEmpty(ms))return true;

	for (const char* p = ms->buffer; *p; p++) {
		if (!isSpaceInternal((unsigned char)*p, mode)) {
			return false;
		}
	}

	return true;
}

bool jcms_equalsWithInst(const JC_MutableString* const ms, const JC_MutableString* const obj) {
	if (!ms && !obj)return true;
	if (!ms || !obj)return false;
	if (!ms->buffer && !obj->buffer)return true;
	if (!ms->buffer || !obj->buffer)return false;
	if (!ms->len && !obj->len)return true;
	if (!ms->len || !obj->len)return false;

	return strcmp(obj->buffer, obj->buffer) == 0;
}

bool jcms_equalsIgnoreCaseWithInst(const JC_MutableString* const ms, const JC_MutableString* const obj, JC_SpaceMode mode) {
	if (!ms && !obj)return true;
	if (!ms || !obj)return false;
	if (!ms->buffer && !obj->buffer)return true;
	if (!ms->buffer || !obj->buffer)return false;
	if (!ms->len && !obj->len)return true;
	if (!ms->len || !obj->len)return false;

	if (ms->len != obj->len)return false;
	for (size_t i = 0; i < ms->len; i++)if (toLowerInternal((unsigned char)ms->buffer[i], mode) != toLowerInternal((unsigned char)obj->buffer[i], mode))return false;
	return true;
}

bool jcms_equalsWithCh(const JC_MutableString* const ms, const char* const obj) {
	if (!ms)return false;
	if (!ms->buffer && !obj)return true;
	if (!ms->buffer || !obj)return false;
	if (!ms->buffer[0] == '\0' && obj[0] == '\0')return true;
	if (!ms->buffer[0] == '\0' || obj[0] == '\0')return false;
	return strcmp(ms->buffer, obj) == 0;
}

bool jcms_equalsIgnoreCaseWithCh(const JC_MutableString* const ms, const char* const obj, JC_SpaceMode mode) {
	if (!ms)return false;
	if (!ms->buffer && !obj)return true;
	if (!ms->buffer || !obj)return false;
	if (ms->buffer[0] == '\0' && obj[0] == '\0')return true;
	if (ms->buffer[0] == '\0' || obj[0] == '\0')return false;

	size_t len = strlen(obj);
	if (ms->len != len)return false;
	for (size_t i = 0; i < len; i++) {
		if (toLowerInternal((unsigned char)ms->buffer[i], mode) !=
			toLowerInternal((unsigned char)obj[i], mode))return false;
	}
	return true;
}

bool jcms_containsWithInst(const JC_MutableString* const ms, const JC_MutableString* const obj) {
	if (!ms && !obj)return true;              // NULL 包含 NULL
	if (!ms || !obj)return false;
	if (!ms->buffer && !obj->buffer)return true;
	if (!ms->buffer || !obj->buffer)return false;

	return strstr(ms->buffer, obj->buffer) != NULL;
}

bool jcms_containsIgnoreCaseWithInst(const JC_MutableString* const ms, const JC_MutableString* const obj, JC_SpaceMode mode) {
	if (!ms && !obj)return true;
	if (!ms || !obj)return false;
	if (!ms->buffer && !obj->buffer)return true;
	if (!ms->buffer || !obj->buffer)return false;

	return containsIgnoreCase(ms->buffer, obj->buffer, mode);
}

bool jcms_containsWithCh(const JC_MutableString* const ms, const char* const ch) {
	if (!ms)return false;
	if (!ms->buffer && !ch)return true;
	if (!ms->buffer || !ch)return false;

	return strstr(ms->buffer, ch) != NULL;
}

bool jcms_containsIgnoreCaseWithCh(const JC_MutableString* const ms, const char* const ch, JC_SpaceMode mode) {
	if (!ms)return false;
	if (!ms->buffer && !ch)return true;
	if (!ms->buffer || !ch)return false;

	return containsIgnoreCase(ms->buffer, ch, mode);
}

int jcms_compareWithInst(const JC_MutableString* const ms1, const JC_MutableString* const ms2, int* result) {
	if (!result)return JC_MS__NULL_RECEIVER;
	if (!ms1 && !ms2) {
		*result = 0;
		return JC_MS__SUCCESSFUL_OP;
	}
	if (!ms1 || !ms2)return JC_MS__NULL;
	if (!ms1->buffer && !ms2->buffer) {
		*result = 0;
		return JC_MS__SUCCESSFUL_OP;
	}
	if (!ms1->buffer || !ms2->buffer)return JC_MS__NULL_BUFFER;
	*result = strcmp(ms1->buffer, ms2->buffer);
	return JC_MS__SUCCESSFUL_OP;
}

int jcms_compareIgnoreCaseWithInst(const JC_MutableString* const ms1, const JC_MutableString* const ms2, int* result, JC_SpaceMode mode) {
	if (!result)return JC_MS__NULL_RECEIVER;
	if (!ms1 && !ms2) {
		*result = 0;
		return JC_MS__SUCCESSFUL_OP;
	}
	if (!ms1 || !ms2)return JC_MS__NULL;
	if (!ms1->buffer && !ms2->buffer) {
		*result = 0;
		return JC_MS__SUCCESSFUL_OP;
	}
	if (!ms1->buffer || !ms2->buffer)return JC_MS__NULL_BUFFER;
	*result = compareIgnoreCase(ms1->buffer, ms2->buffer, mode);
	return JC_MS__SUCCESSFUL_OP;
}

int jcms_compareWithCh(const JC_MutableString* const ms, const char* const ch, int* result) {
	if (!result)return JC_MS__NULL_RECEIVER;
	if (!ms)return JC_MS__NULL;
	if (!ms->buffer && !ch) {
		*result = 0;
		return JC_MS__SUCCESSFUL_OP;
	}
	if (!ms->buffer || !ch)return JC_MS__NULL_BUFFER;
	*result = strcmp(ms->buffer, ch);
	return JC_MS__SUCCESSFUL_OP;
}

int jcms_compareIgnoreCaseWithCh(const JC_MutableString* const ms, const char* const ch, int* result, JC_SpaceMode mode) {
	if (!result)return JC_MS__NULL_RECEIVER;
	if (!ms)return JC_MS__NULL;
	if (!ms->buffer && !ch) {
		*result = 0;
		return JC_MS__SUCCESSFUL_OP;
	}
	if (!ms->buffer || !ch)return JC_MS__NULL_BUFFER;
	*result = compareIgnoreCase(ms->buffer, ch, mode);
	return JC_MS__SUCCESSFUL_OP;
}


// enable
char* jcms_toCharArray(const JC_MutableString* const ms) {
	if (!ms || !ms->buffer)return NULL;
	if (ms->buffer[0] == '\0') {
		char* temp = (char*)malloc(1, sizeof(char));
		if (!temp)return NULL;
		temp[0] = '\0';
		return temp;
	}

	size_t len = ms->len + 1;
	char* content = (char*)malloc(len);
	return content ? memcpy(content, ms->buffer, len) : NULL;
}

int jcms_toInt(const JC_MutableString* const ms, int* result, JC_SpaceMode mode) {
	if (!ms || !ms->buffer || ms->buffer[0] == '\0') return JC_MS__EMPTY;
	if (ms->status != JC_MS__AVAILABLE) return ms->status;
	if (!result) return JC_MS__NULL_INT;

	long long v;
	int s = parseToLL(ms->buffer, ms->len, &v, mode);
	if (s != JC_MS__SUCCESSFUL_OP) {
		if (s == JC_MS__BLANK)          return JC_MS__BLANK;
		if (s == JC_MS__NOT_LONGLONG)   return JC_MS__NOT_INT;
		return JC_MS__OVERFLOW_INT;
	}
	if (v < INT_MIN || v > INT_MAX) return JC_MS__OVERFLOW_INT;
	*result = (int)v;
	return JC_MS__SUCCESSFUL_OP;
}

int jcms_toLong(const JC_MutableString* const ms, long* result, JC_SpaceMode mode) {
	if (!ms || !ms->buffer || ms->buffer[0] == '\0') return JC_MS__EMPTY;
	if (ms->status != JC_MS__AVAILABLE) return ms->status;
	if (!result) return JC_MS__NULL_LONG;

	long long v;
	int s = parseToLL(ms->buffer, ms->len, &v, mode);
	if (s != JC_MS__SUCCESSFUL_OP) {
		if (s == JC_MS__BLANK)          return JC_MS__BLANK;
		if (s == JC_MS__NOT_LONGLONG)   return JC_MS__NOT_LONG;
		return JC_MS__OVERFLOW_LONG;
	}
	if (v < LONG_MIN || v > LONG_MAX) return JC_MS__OVERFLOW_LONG;
	*result = (long)v;
	return JC_MS__SUCCESSFUL_OP;
}

int jcms_toLongLong(const JC_MutableString* const ms, long long* result, JC_SpaceMode mode) {
	if (!ms || !ms->buffer || ms->buffer[0] == '\0') return JC_MS__EMPTY;
	if (ms->status != JC_MS__AVAILABLE) return ms->status;
	if (!result) return JC_MS__NULL_LONGLONG;

	int s = parseToLL(ms->buffer, ms->len, result, mode);
	if (s == JC_MS__SUCCESSFUL_OP)      return JC_MS__SUCCESSFUL_OP;
	if (s == JC_MS__BLANK)              return JC_MS__BLANK;
	if (s == JC_MS__NOT_LONGLONG)       return JC_MS__NOT_LONGLONG;
	return JC_MS__OVERFLOW_LONGLONG;
}

int jcms_toFloat(const JC_MutableString* const ms, float* result, JC_SpaceMode mode) {
	if (!ms || !ms->buffer || ms->buffer[0] == '\0') return JC_MS__EMPTY;
	if (ms->status != JC_MS__AVAILABLE) return ms->status;
	if (!result) return JC_MS__NULL_FLOAT;

	double v;
	int s = parseToDouble(ms->buffer, ms->len, &v, mode);
	if (s != JC_MS__SUCCESSFUL_OP) {
		if (s == JC_MS__BLANK)          return JC_MS__BLANK;
		if (s == JC_MS__NOT_DOUBLE)     return JC_MS__NOT_FLOAT;
		return JC_MS__OVERFLOW_FLOAT;
	}
	if (v > FLT_MAX || v < -FLT_MAX) return JC_MS__OVERFLOW_FLOAT;
	*result = (float)v;
	return JC_MS__SUCCESSFUL_OP;
}

int jcms_toDouble(const JC_MutableString* const ms, double* result, JC_SpaceMode mode) {
	if (!ms || !ms->buffer || ms->buffer[0] == '\0') return JC_MS__EMPTY;
	if (ms->status != JC_MS__AVAILABLE) return ms->status;
	if (!result) return JC_MS__NULL_DOUBLE;

	double v;
	int s = parseToDouble(ms->buffer, ms->len, &v, mode);
	if (s == JC_MS__SUCCESSFUL_OP) { *result = v; return JC_MS__SUCCESSFUL_OP; }
	if (s == JC_MS__BLANK)              return JC_MS__BLANK;
	if (s == JC_MS__NOT_DOUBLE)         return JC_MS__NOT_DOUBLE;
	return JC_MS__OVERFLOW_DOUBLE;
}

int jcms_toLowerCase(JC_MutableString* ms, JC_SpaceMode mode) {
	if (!ms)return JC_MS__NULL;
	if (!ms->buffer)return JC_MS__NULL_BUFFER;

	for (size_t i = 0; i < ms->len; i++) {
		ms->buffer[i] = (char)toLowerInternal((unsigned char)ms->buffer[i], mode);
	}

	return JC_MS__SUCCESSFUL_OP;
}

int jcms_toUpperCase(JC_MutableString* ms, JC_SpaceMode mode) {
	if (!ms)return JC_MS__NULL;
	if (!ms->buffer)return JC_MS__NULL_BUFFER;

	for (size_t i = 0; i < ms->len; i++) {
		ms->buffer[i] = (char)toUpperInternal((unsigned char)ms->buffer[i], mode);
	}

	return JC_MS__SUCCESSFUL_OP;
}

jc_String jcms_toString(const JC_MutableString* const ms) {
	if (!ms)return jcstr_newFrom(NULL);
	return jcstr_newFrom(ms->buffer);
}