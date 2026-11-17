#include "JC_String.h"
#include "JC_StringStructures.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include <stdint.h>
#include <float.h>
#include <math.h>
#include "JC_MutableString.h"

// 默认构造器，返回一个内容为\0的实例
static jc_String jcstr_new(void) {
	return jcstr_newFrom("");
}

// 将已知的char*、状态和长度封装成一个string实例，不成功则返回NULL
static jc_String wrapString(char* buffer, int status, size_t length) {
	jc_String temp = (jc_String)malloc(sizeof(JC_String));
	if (!temp)return NULL;
	temp->buffer = buffer;
	temp->status = status;
	temp->cachedLen = length;
	return temp;
}

static jc_String pkgInStatus(char* content, size_t len, int status) {
	char* tempBuf = (char*)malloc(len + 1);
	if (!tempBuf)return wrapString(tempBuf, JC_STR__NULL_BUFFER, 0);
	memcpy(tempBuf, content, len);
	tempBuf[len] = '\0';
	jc_String result = wrapString(tempBuf, status, len);
	if (!result)free(tempBuf);
	return result;
}

// 将内容复制后带着长度打包成string实例，len为有效长度
static jc_String pkgString(char* content, size_t len) {
	return pkgInStatus(content, len, JC_STR__AVAILABLE);
}

// 通过两个char*和两个长度组装一个string实例
static jc_String appendCore(char* str1, char* str2, size_t len1, size_t len2) {
	if (len1 > SIZE_MAX - len2 - 1)return wrapString(NULL, JC_STR__LEN_OUT_OF_TYPE_MAX, 0);

	char* combined = (char*)malloc(len1 + len2 + 1);
	// 如果内容结合指针申请内存失败，则返回一个状态为JC_STR__COMBINATION_FAILED的实例
	if (!combined)return wrapString(combined, JC_STR__COMBINATION_FAILED, 0);

	// 将内容复制到combined
	memcpy(combined, str1, len1);
	memcpy(combined + len1, str2, len2 + 1);

	jc_String result = wrapString(combined, JC_STR__AVAILABLE, len1 + len2);
	if (!result)free(combined);
	return result;
}

// 通过从前往后依据长度的sub
static jc_String subLenFrontCore(char* base, size_t startIdx, size_t givenLen, size_t realLen) {
	if (startIdx >= realLen) {
		char* tempBuf = (char*)malloc(1);
		if (!tempBuf) return wrapString(tempBuf, JC_STR__NULL_BUFFER, 0);   // 分配失败才是 NULL buffer
		tempBuf[0] = '\0';
		jc_String temp = wrapString(tempBuf, JC_STR__INVALID_START, 0);
		if (!temp) {
			free(tempBuf);
		}
		return temp;
	}

	if (givenLen > realLen - startIdx)givenLen = realLen - startIdx;

	return pkgString(base + startIdx, givenLen);
}

// 通过从后往前依据长度的sub
static jc_String subLenBackCore(char* base, size_t endIdx, size_t givenLen, size_t realLen) {
	if (endIdx >= realLen) {
		char* tempBuf = (char*)malloc(1);
		if (!tempBuf)return wrapString(tempBuf, JC_STR__NULL_BUFFER, 0);
		tempBuf[0] = '\0';
		jc_String temp = wrapString(tempBuf, JC_STR__INVALID_END, 0);
		if (!temp) {
			free(tempBuf);
		}
		return temp;
	}

	if (endIdx + 1 < givenLen)givenLen = endIdx + 1;

	return pkgString(base + (endIdx - givenLen + 1), givenLen);
}

// 通过左闭右开取范围的sub
static jc_String subRangeCore(char* str, size_t start, size_t end, size_t realLen) {
	if (start > end) {
		char* tempBuf = (char*)malloc(1);
		if (!tempBuf)return wrapString(tempBuf, JC_STR__NULL_BUFFER, 0);
		tempBuf[0] = '\0';
		jc_String temp = wrapString(tempBuf, JC_STR__INVALID_IDX, 0);
		if (!temp) {
			free(tempBuf);
		}
		return temp;
	}
	if (start >= realLen || end > realLen) {
		char* tempBuf = (char*)malloc(1);
		if (!tempBuf)return wrapString(tempBuf, JC_STR__NULL_BUFFER, 0);
		tempBuf[0] = '\0';
		jc_String temp = wrapString(tempBuf, JC_STR__IDX_OUT_BOUNDS, 0);
		if (!temp) {
			free(tempBuf);
		}
		return temp;
	}

	size_t length = end - start;

	return pkgString(str + start, length);
}

// 前置条件：str、str->buffer、target、replacement 均非 NULL
// 语义与可变版 replaceFirstCore 对齐：
//   空 target → 末尾追加（等价于 replacement 拼在 str 后）
//   空内容 + 非空 target → 原样返回
static jc_String replaceFirstStrCore(const JC_String* const str,
	const char* const target,
	const char* const replacement) {
	// 空 target → 末尾追加
	if (target[0] == '\0') {
		size_t strLen = str->cachedLen;
		size_t replaceLen = strlen(replacement);
		char* newBuffer = (char*)malloc(strLen + replaceLen + 1);
		if (!newBuffer) return wrapString(NULL, JC_STR__NULL_BUFFER, 0);
		memcpy(newBuffer, str->buffer, strLen);
		memcpy(newBuffer + strLen, replacement, replaceLen);
		newBuffer[strLen + replaceLen] = '\0';
		jc_String result = wrapString(newBuffer, JC_STR__AVAILABLE, strLen + replaceLen);
		if (!result) free(newBuffer);
		return result;
	}

	// 空内容 + 非空 target → 原样返回（可变版返回 NO_TARGET_MATCHED，实例不变）
	if (str->cachedLen == 0) {
		return jcstr_newFrom(str->buffer);
	}

	char* indexChar = strstr(str->buffer, target);
	if (!indexChar) return jcstr_newFrom(str->buffer);

	size_t index = (size_t)(indexChar - str->buffer);
	size_t targetLen = strlen(target);
	size_t replaceLen = strlen(replacement);
	size_t strLen = str->cachedLen;
	size_t sufLen = strLen - index - targetLen;
	size_t newLen = index + replaceLen + sufLen;

	char* newBuffer = (char*)malloc(newLen + 1);
	if (!newBuffer) return wrapString(NULL, JC_STR__NULL_BUFFER, 0);

	memcpy(newBuffer, str->buffer, index);
	memcpy(newBuffer + index, replacement, replaceLen);
	memcpy(newBuffer + index + replaceLen, indexChar + targetLen, sufLen + 1);

	jc_String result = wrapString(newBuffer, JC_STR__AVAILABLE, newLen);
	if (!result) free(newBuffer);
	return result;
}

// 前置条件：str、str->buffer、target、replacement 均非 NULL
// 语义与可变版 replaceAllCore 对齐：
//   空内容 + 空 target     → 整体设为 replacement
//   空内容 + 非空 target   → 原样返回（可变版返回 NO_TARGET_MATCHED，实例不变）
//   非空内容 + 空 target   → 末尾追加
//   其余                   → 扫描所有非重叠匹配并替换
static jc_String replaceAllStrCore(const JC_String* const str,
	const char* const target,
	const char* const replacement) {
	size_t strLen = str->cachedLen;
	size_t targetLen = strlen(target);
	size_t replaceLen = strlen(replacement);

	// 空内容 + 空 target：整体设为 replacement
	if (strLen == 0 && target[0] == '\0') return jcstr_newFrom(replacement);
	// 空内容 + 非空 target：找不到，返回原内容副本
	if (strLen == 0) return jcstr_newFrom(str->buffer);
	// 非空内容 + 空 target：末尾追加
	if (target[0] == '\0') {
		char* newBuffer = (char*)malloc(strLen + replaceLen + 1);
		if (!newBuffer) return wrapString(NULL, JC_STR__NULL_BUFFER, 0);
		memcpy(newBuffer, str->buffer, strLen);
		memcpy(newBuffer + strLen, replacement, replaceLen);
		newBuffer[strLen + replaceLen] = '\0';
		jc_String result = wrapString(newBuffer, JC_STR__AVAILABLE, strLen + replaceLen);
		if (!result) free(newBuffer);
		return result;
	}

	char* src = str->buffer;
	size_t* position = NULL;
	size_t capacity = 0;
	size_t count = 0;
	char* scan = src;

	while ((scan = strstr(scan, target)) != NULL) {
		if (count >= capacity) {
			capacity += 16;
			size_t* newPos = (size_t*)realloc(position, capacity * sizeof(size_t));
			if (!newPos) {
				free(position);
				return wrapString(NULL, JC_STR__NULL_BUFFER, 0);
			}
			position = newPos;
		}
		position[count++] = (size_t)(scan - src);
		scan += targetLen;
	}

	if (!count) {
		free(position);
		return jcstr_newFrom(src);
	}

	size_t newLen;
	if (replaceLen >= targetLen) {
		newLen = strLen + count * (replaceLen - targetLen);
	}
	else {
		newLen = strLen - count * (targetLen - replaceLen);
	}
	char* newBuffer = (char*)malloc(newLen + 1);
	if (!newBuffer) {
		free(position);
		return wrapString(NULL, JC_STR__NULL_BUFFER, 0);
	}

	char* ptr = newBuffer;
	size_t lastPos = 0;

	for (size_t i = 0; i < count; i++) {
		size_t pos = position[i];
		size_t copyLen = pos - lastPos;
		memcpy(ptr, src + lastPos, copyLen);
		ptr += copyLen;
		memcpy(ptr, replacement, replaceLen);
		ptr += replaceLen;
		lastPos = pos + targetLen;
	}
	memcpy(ptr, src + lastPos, strLen - lastPos + 1);

	free(position);

	jc_String result = wrapString(newBuffer, JC_STR__AVAILABLE, newLen);
	if (!result) free(newBuffer);
	return result;
}


// 解析 buffer 的 [0, len) 为一个 long long
// 返回 STR_SUCCESSFULOP / STR_BLANK / STR_NOTLONGLONG / STR_OVERFLOWLONGLONG
static int parseToLL(const char* buf, size_t len, long long* out, JC_SpaceMode mode) {
	size_t start = 0;
	while (start < len && isSpaceInternal((unsigned char)buf[start], mode)) start++;
	size_t end = len;
	while (end > start && isSpaceInternal((unsigned char)buf[end - 1], mode)) end--;
	if (start == end) return JC_STR__BLANK;

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
			if (temp < (limit + d) / 10) return JC_STR__OVERFLOW_LONGLONG;
			temp = 10 * temp - d;
			count++;
		}
		else {
			return JC_STR__NOT_LONGLONG;
		}
	}

	if (!count) return JC_STR__NOT_LONGLONG;
	*out = isNeg ? temp : -temp;
	return JC_STR__SUCCESSFUL_OP;
}

static int parseToDouble(const char* buf, size_t len, double* out, JC_SpaceMode mode) {
	size_t start = 0;
	while (start < len && isSpaceInternal((unsigned char)buf[start], mode)) start++;
	size_t end = len;
	while (end > start && isSpaceInternal((unsigned char)buf[end - 1], mode)) end--;
	if (start == end) return JC_STR__BLANK;

	char* endptr = NULL;
	errno = 0;
	double value = strtod(buf + start, &endptr);

	if (endptr != buf + end) return JC_STR__NOT_DOUBLE;
	if (errno == ERANGE)     return JC_STR__OVERFLOW_DOUBLE;

	*out = value;
	return JC_STR__SUCCESSFUL_OP;
}

/* =============================================================== */
// 通过char*构造string
jc_String jcstr_newFrom(const char* const content) {
	if (!content)return wrapString(NULL, JC_STR__NULL_BUFFER, 0);

	size_t len = strlen(content);
	return pkgString(content, len);
}
/* =============================================================== */

/* =============================================================== */
// 析构，可传NULL
void jcstr_delete(jc_String str) {
	if (!str)return;
	if (str->buffer) {
		free(str->buffer);
	}
	free(str);
}
/* =============================================================== */

/* =============================================================== */
// 状态返回器，在使用实例前一定调用此函数检查实例状态是否可用，使用方法：if(getStringStatus(str)!=JC_STR__AVAILABLE){ // 该实例状态异常}
int jcstr_getStatus(const JC_String* const str) {
	if (!str)return JC_STR__NULL;
	return str->status;
}
/* =============================================================== */

/* =============================================================== */
// 获取实例长度
size_t jcstr_getLength(const JC_String* const str) {
	if (!str || !str->buffer)return 0;
	return str->cachedLen;
}
/* =============================================================== */

/* =============================================================== */
const char* jcstr_statusToCharArray(const int status) {
	switch (status) {
		// 实例状态
	case JC_STR__NULL:                      return "null string";
	case JC_STR__AVAILABLE:                 return "available string";
	case JC_STR__NULL_BUFFER:               return "null buffer";
	case JC_STR__EMPTY:                     return "empty string";
	case JC_STR__BLANK:                     return "blank string";

		// 操作成功
	case JC_STR__SUCCESSFUL_OP:             return "successful operation";
	case JC_STR__PARTIAL_SPLIT:             return "string partial split";

		// 内存/组合错误
	case JC_STR__ALLOCATION_FAILED:         return "failed to alloc";
	case JC_STR__REALLOCATION_FAILED:       return "failed to realloc";
	case JC_STR__COMBINATION_FAILED:        return "failed to combine";
	case JC_STR__LEN_OUT_OF_TYPE_MAX:       return "length out of max size of some type";

	case JC_STR__READ_ERROR:				return "got fileread error";
	case JC_STR__EOF:						return "met end of file";

		// 参数错误
	case JC_STR__NULL_CHAR:                 return "null char pointer";
	case JC_STR__NULL_ARRAY:                return "null string array";
	case JC_STR__NULL_RECEIVER:             return "receiver is null";
	case JC_STR__NULL_TARGET:               return "target is null";
	case JC_STR__NULL_COUNT:                return "count ptr null";
	case JC_STR__INVALID_FOLLOWER:          return "follower instance is null";
	case JC_STR__INVALID_FOLLOWER_BUFFER:   return "follower buffer is null";
	case JC_STR__INVALID_REPLACEMENT:       return "replacement is invalid";
	case JC_STR__INVALID_START:             return "invalid start index";
	case JC_STR__INVALID_END:               return "invalid end index";
	case JC_STR__INVALID_IDX:               return "invalid index";
	case JC_STR__IDX_OUT_BOUNDS:            return "index out of bounds";
	case JC_STR__WITHOUT_CH:                return "there is no char you want here";
	case JC_STR__INVALID_FILE_POINTER:		return "invalid file pointer";

		// 数字解析：输出指针为 NULL
	case JC_STR__NULL_INT:                  return "null int pointer";
	case JC_STR__NULL_LONG:                 return "null long int pointer";
	case JC_STR__NULL_LONGLONG:             return "null long long int pointer";
	case JC_STR__NULL_FLOAT:                return "null float pointer";
	case JC_STR__NULL_DOUBLE:               return "null double pointer";

		// 数字解析：格式错误
	case JC_STR__NOT_INT:                   return "not int";
	case JC_STR__NOT_LONG:                  return "not long int";
	case JC_STR__NOT_LONGLONG:              return "not long long int";
	case JC_STR__NOT_FLOAT:                 return "not float";
	case JC_STR__NOT_DOUBLE:                return "not double";

		// 数字解析：溢出
	case JC_STR__OVERFLOW_INT:              return "overflow int range";
	case JC_STR__OVERFLOW_LONG:             return "overflow long int range";
	case JC_STR__OVERFLOW_LONGLONG:         return "overflow long long int range";
	case JC_STR__OVERFLOW_FLOAT:            return "overflow float range";
	case JC_STR__OVERFLOW_DOUBLE:           return "overflow double range";

	default:                                return "Unknown status";
	}
}
/* =============================================================== */




/* =============================================================== */
jc_String jcstr_appendWithInst(const JC_String* const str1, const JC_String* const str2) {
	if (!str1)return NULL;
	if (!str1->buffer)return wrapString(NULL, JC_STR__NULL_BUFFER, 0);
	if (!str2)return pkgInStatus(str1->buffer, str1->cachedLen, JC_STR__INVALID_FOLLOWER);
	if (!str2->buffer)return pkgInStatus(str1->buffer, str1->cachedLen, JC_STR__INVALID_FOLLOWER_BUFFER);

	return appendCore(str1->buffer, str2->buffer, str1->cachedLen, str2->cachedLen);
}

jc_String jcstr_appendWithCh(const JC_String* const str, const char* const content) {
	if (!str)return NULL;
	if (!str->buffer)return wrapString(NULL, JC_STR__NULL_BUFFER, 0);
	if (!content)return pkgInStatus(str->buffer, str->cachedLen, JC_STR__INVALID_FOLLOWER);

	return appendCore(str->buffer, content, str->cachedLen, strlen(content));
}

jc_String jcstr_reverse(const JC_String* const str) {
	if (!str)return NULL;
	if (!str->buffer)return jcstr_newFrom(NULL);
	if (str->buffer[0] == '\0')return jcstr_new();

	size_t len = str->cachedLen;

	char* tempBuffer = (char*)malloc(len + 1);
	if (!tempBuffer)return wrapString(tempBuffer, JC_STR__NULL_BUFFER, 0);

	memcpy(tempBuffer, str->buffer, len + 1);

	for (size_t i = 0; i < len / 2; i++) {
		char c = tempBuffer[i];
		tempBuffer[i] = tempBuffer[len - 1 - i];
		tempBuffer[len - 1 - i] = c;
	}

	jc_String temp = wrapString(tempBuffer, JC_STR__AVAILABLE, len);
	if (!temp)free(tempBuffer);
	return temp;
}

int jcstr_split(const JC_String* const str, char delimiter, jc_String** outArray, size_t* count, bool removeEmptyEntries) {
	if (!outArray) return JC_STR__NULL_ARRAY;
	if (!count) return JC_STR__NULL_COUNT;

	*outArray = NULL;
	*count = 0;

	if (!str) return JC_STR__NULL;
	if (!str->buffer) return JC_STR__NULL_BUFFER;

	// 记录下字符串首位
	const char* buf = str->buffer;
	// 记录下字符串末位
	const char* end = buf + str->cachedLen;

	// 计数
	size_t wholeCount = 0;
	const char* p = buf;
	while (1) {
		// 指向分隔符地址
		const char* found = (const char*)memchr(p, delimiter, (size_t)(end - p));
		// 指向每段结束地址后一位
		const char* segEnd = found ? found : end;
		// 求出每段长度
		size_t segLen = (size_t)(segEnd - p);

		// 如果不清理空位，直接记录该段为一个元素
		if (segLen > 0 || !removeEmptyEntries) wholeCount++;
		// 到最后一段了
		if (!found) break;
		p = found + 1;
	}

	if (wholeCount == 0) return JC_STR__SUCCESSFUL_OP;

	jc_String* stringArray = (jc_String*)malloc(sizeof(jc_String) * wholeCount);
	if (!stringArray) return JC_STR__ALLOCATION_FAILED;

	// 填充
	bool hasNull = false;
	size_t i = 0;
	p = buf;
	while (1) {
		const char* found = (const char*)memchr(p, delimiter, (size_t)(end - p));
		const char* segEnd = found ? found : end;
		size_t segLen = (size_t)(segEnd - p);

		if (segLen > 0 || !removeEmptyEntries) {
			char* temp = (char*)malloc(segLen + 1);
			if (!temp) {
				stringArray[i] = jcstr_newFrom(NULL);
				hasNull = true;
			}
			else {
				memcpy(temp, p, segLen);
				temp[segLen] = '\0';
				stringArray[i] = wrapString(temp, JC_STR__AVAILABLE, segLen);
				if (!stringArray[i]) {
					hasNull = true;
					free(temp);
				}
			}
			i++;
		}

		if (!found) break;
		p = found + 1;
	}

	*outArray = stringArray;
	*count = wholeCount;
	return hasNull ? JC_STR__PARTIAL_SPLIT : JC_STR__SUCCESSFUL_OP;
}

jc_String jcstr_trim(const JC_String* const str, JC_SpaceMode mode) {
	if (!str)return NULL;
	if (!str->buffer)return jcstr_newFrom(NULL);
	if (str->buffer[0] == '\0')return jcstr_new();

	size_t len = str->cachedLen;
	// 设定开始位置
	size_t start = 0;
	while (start < len && isSpaceInternal((unsigned char)str->buffer[start], mode))start++;
	if (start == len)return jcstr_new();

	// 设定末位位置
	size_t end = len - 1;
	while (end > start && isSpaceInternal((unsigned char)str->buffer[end], mode))end--;

	size_t newLen = end - start + 1;

	return pkgString(str->buffer + start, newLen);
}

jc_String jcstr_trimFront(const JC_String* const str, JC_SpaceMode mode) {
	if (!str)return NULL;
	if (!str->buffer)return jcstr_newFrom(NULL);
	if (str->buffer[0] == '\0')return jcstr_new();

	size_t len = str->cachedLen;
	// 设定开始位置
	size_t start = 0;
	while (start < len && isSpaceInternal((unsigned char)str->buffer[start], mode))start++;
	if (start == len)return jcstr_new();

	size_t newLen = len - start;

	return pkgString(str->buffer + start, newLen);
}

jc_String jcstr_trimBack(const JC_String* const str, JC_SpaceMode mode) {
	if (!str)return NULL;
	if (!str->buffer)return jcstr_newFrom(NULL);
	if (str->buffer[0] == '\0')return jcstr_new();

	// 设定末位位置
	size_t newLen = str->cachedLen;
	while (newLen > 0 && isSpaceInternal((unsigned char)str->buffer[newLen - 1], mode))newLen--;

	return pkgString(str->buffer, newLen);
}

jc_String jcstr_clone(const JC_String* const str) {
	if (!str)return NULL;
	return jcstr_newFrom(str->buffer);
}

int jcstr_charAt(const JC_String* const str, size_t index, char* result) {
	if (!result)return JC_STR__NULL_CHAR;
	if (!str)return JC_STR__NULL;
	if (!str->buffer)return JC_STR__NULL_BUFFER;
	if (index >= str->cachedLen)return JC_STR__IDX_OUT_BOUNDS;
	*result = str->buffer[index];
	return JC_STR__SUCCESSFUL_OP;
}

int jcstr_indexOfFront(const JC_String* const str, char ch, size_t* result) {
	if (!result)return JC_STR__NULL_RECEIVER;
	if (!str)return JC_STR__NULL;
	if (!str->buffer)return JC_STR__NULL_BUFFER;

	const char* found = strchr(str->buffer, ch);
	if (found) {
		*result = (size_t)(found - str->buffer);
		return JC_STR__SUCCESSFUL_OP;
	}
	return JC_STR__WITHOUT_CH;
}

int jcstr_indexOfBack(const JC_String* const str, char ch, size_t* result) {
	if (!result)return JC_STR__NULL_RECEIVER;
	if (!str)return JC_STR__NULL;
	if (!str->buffer)return JC_STR__NULL_BUFFER;
	for (ptrdiff_t i = (ptrdiff_t)str->cachedLen - 1; i >= 0; --i) {
		if (str->buffer[i] == ch) {
			*result = (size_t)i;
			return JC_STR__SUCCESSFUL_OP;
		}
	}
	return JC_STR__WITHOUT_CH;
}

int jcstr_indexOfFrontWithInst(const JC_String* const str, const JC_String* const sub, size_t* result) {
	if (!result)return JC_STR__NULL_RECEIVER;
	if (!str || !sub)return JC_STR__NULL;
	if (!str->buffer || !sub->buffer)return JC_STR__NULL_BUFFER;
	if (sub->buffer[0] == '\0') {
		*result = str->cachedLen;
		return JC_STR__SUCCESSFUL_OP;
	}
	const char* found = strstr(str->buffer, sub->buffer);
	if (!found)return JC_STR__WITHOUT_CH;
	*result = (size_t)(found - str->buffer);
	return JC_STR__SUCCESSFUL_OP;
}

int jcstr_indexOfFrontWithCh(const JC_String* const str, const char* const ch, size_t* result) {
	if (!result)return JC_STR__NULL_RECEIVER;
	if (!str)return JC_STR__NULL;
	if (!str->buffer || !ch)return JC_STR__NULL_BUFFER;
	if (ch[0] == '\0') {
		*result = str->cachedLen;
		return JC_STR__SUCCESSFUL_OP;
	}
	const char* found = strstr(str->buffer, ch);
	if (!found)return JC_STR__WITHOUT_CH;
	*result = (size_t)(found - str->buffer);
	return JC_STR__SUCCESSFUL_OP;
}

jc_String jcstr_subInLengthFrontWithInst(const JC_String* const str, size_t start, size_t length) {
	if (!str)return NULL;
	if (!str->buffer)return jcstr_newFrom(NULL);
	if (str->buffer[0] == '\0' || !length)return jcstr_new();

	return subLenFrontCore(str->buffer, start, length, str->cachedLen);
}

jc_String jcstr_subInLengthBackWithInst(const JC_String* const str, size_t end, size_t length) {
	if (!str)return NULL;
	if (!str->buffer)return jcstr_newFrom(NULL);
	if (str->buffer[0] == '\0' || !length)return jcstr_new();

	return subLenBackCore(str->buffer, end, length, str->cachedLen);
}

jc_String jcstr_subInRangeWithInst(const JC_String* const str, size_t start, size_t end) {
	if (!str)return NULL;
	if (!str->buffer)return jcstr_newFrom(NULL);
	if (str->buffer[0] == '\0' || start == end)return jcstr_new();

	return subRangeCore(str->buffer, start, end, str->cachedLen);
}

jc_String jcstr_subInLengthFrontWithCh(const char* const ch, size_t start, size_t length) {
	if (!ch)return jcstr_newFrom(NULL);
	if (ch[0] == '\0' || !length)return jcstr_new();

	return subLenFrontCore(ch, start, length, strlen(ch));
}

jc_String jcstr_subInLengthBackWithCh(const char* const ch, size_t end, size_t length) {
	if (!ch)return jcstr_newFrom(NULL);
	if (ch[0] == '\0' || !length)return jcstr_new();

	return subLenBackCore(ch, end, length, strlen(ch));
}

jc_String jcstr_subInRangeWithCh(const char* const ch, size_t start, size_t end) {
	if (!ch)return jcstr_newFrom(NULL);
	if (ch[0] == '\0' || start == end)return jcstr_new();

	return subRangeCore(ch, start, end, strlen(ch));
}

jc_String jcstr_replaceFirstWithInst(const JC_String* const str,
	const char* const target,
	const JC_String* const replacement) {
	if (!str) return NULL;
	if (!str->buffer) return jcstr_newFrom(NULL);
	if (!target) return pkgInStatus(str->buffer, str->cachedLen, JC_STR__NULL_TARGET);
	if (!replacement || !replacement->buffer)
		return pkgInStatus(str->buffer, str->cachedLen, JC_STR__INVALID_REPLACEMENT);
	return replaceFirstStrCore(str, target, replacement->buffer);
}

jc_String jcstr_replaceAllWithInst(const JC_String* const str,
	const char* const target,
	const JC_String* const replacement) {
	if (!str) return NULL;
	if (!str->buffer) return jcstr_newFrom(NULL);
	if (!target) return pkgInStatus(str->buffer, str->cachedLen, JC_STR__NULL_TARGET);
	if (!replacement || !replacement->buffer)
		return pkgInStatus(str->buffer, str->cachedLen, JC_STR__INVALID_REPLACEMENT);
	return replaceAllStrCore(str, target, replacement->buffer);
}

jc_String jcstr_replaceFirstWithCh(const JC_String* const str,
	const char* const target,
	const char* const replacement) {
	if (!str) return NULL;
	if (!str->buffer) return jcstr_newFrom(NULL);
	if (!target) return pkgInStatus(str->buffer, str->cachedLen, JC_STR__NULL_TARGET);
	if (!replacement) return pkgInStatus(str->buffer, str->cachedLen, JC_STR__INVALID_REPLACEMENT);
	return replaceFirstStrCore(str, target, replacement);
}

jc_String jcstr_replaceAllWithCh(const JC_String* const str,
	const char* const target,
	const char* const replacement) {
	if (!str) return NULL;
	if (!str->buffer) return jcstr_newFrom(NULL);
	if (!target) return pkgInStatus(str->buffer, str->cachedLen, JC_STR__NULL_TARGET);
	if (!replacement) return pkgInStatus(str->buffer, str->cachedLen, JC_STR__INVALID_REPLACEMENT);
	return replaceAllStrCore(str, target, replacement);
}

bool jcstr_startWithCh(const JC_String* const str, const char* const prefix) {
	bool empStr = !str || !str->buffer;
	if (empStr)return false;
	if (!prefix)return false;
	if (prefix[0] == '\0')return true;

	size_t strLen = str->cachedLen;
	size_t preLen = strlen(prefix);
	if (strLen < preLen)return false;

	return strncmp(str->buffer, prefix, preLen) == 0;
}

bool jcstr_endWithCh(const JC_String* const str, const char* const suffix) {
	bool empStr = !str || !str->buffer;
	if (empStr)return false;
	if (!suffix)return false;
	if (suffix[0] == '\0')return true;

	size_t strLen = str->cachedLen;
	size_t sufLen = strlen(suffix);
	if (strLen < sufLen)return false;

	return strncmp(str->buffer + (strLen - sufLen), suffix, sufLen) == 0;
}

bool jcstr_startWithInst(const JC_String* const str, const JC_String* const prefix) {
	if (!prefix)return false;
	return jcstr_startWithCh(str, prefix->buffer);
}

bool jcstr_endWithInst(const JC_String* const str, const JC_String* const suffix) {
	if (!suffix)return false;
	return jcstr_endWithCh(str, suffix->buffer);
}

bool jcstr_isEmpty(const JC_String* const str) {
	return !str || !str->buffer || str->buffer[0] == '\0';
}

bool jcstr_isBlank(const JC_String* const str, JC_SpaceMode mode) {
	if (jcstr_isEmpty(str))return true;

	for (const char* p = str->buffer; *p; p++) {
		if (!isSpaceInternal((unsigned char)*p, mode)) {
			return false;
		}
	}

	return true;
}

bool jcstr_equalsWithInst(const JC_String* const str1, const JC_String* const str2) {
	if (!str1 && !str2)return true;
	if (!str1 || !str2)return false;
	if (!str1->buffer && !str2->buffer)return true;
	if (!str1->buffer || !str2->buffer)return false;
	if (str1->buffer[0] == '\0' && str2->buffer[0] == '\0')return true;
	if (str1->buffer[0] == '\0' || str2->buffer[0] == '\0')return false;
	return strcmp(str1->buffer, str2->buffer) == 0;
}

bool jcstr_equalsIgnoreCaseWithInst(const JC_String* const str1, const JC_String* const str2, JC_SpaceMode mode) {
	if (!str1 && !str2)return true;
	if (!str1 || !str2)return false;
	if (!str1->buffer && !str2->buffer)return true;
	if (!str1->buffer || !str2->buffer)return false;
	if (str1->buffer[0] == '\0' && str2->buffer[0] == '\0')return true;
	if (str1->buffer[0] == '\0' || str2->buffer[0] == '\0')return false;

	if (str1->cachedLen != str2->cachedLen)return false;
	for (size_t i = 0; i < str1->cachedLen; i++)if (toLowerInternal((unsigned char)str1->buffer[i], mode) != toLowerInternal((unsigned char)str2->buffer[i], mode))return false;
	return true;
}

bool jcstr_equalsWithCh(const JC_String* const str, const char* const ch) {
	if (!str)return false;
	if (!str->buffer && !ch)return true;
	if (!str->buffer || !ch)return false;
	if (str->buffer[0] == '\0' && ch[0] == '\0')return true;
	if (str->buffer[0] == '\0' || ch[0] == '\0')return false;
	return strcmp(str->buffer, ch) == 0;
}

bool jcstr_equalsIgnoreCaseWithCh(const JC_String* const str, const char* const ch, JC_SpaceMode mode) {
	if (!str)return false;
	if (!str->buffer && !ch)return true;
	if (!str->buffer || !ch)return false;
	if (str->buffer[0] == '\0' && ch[0] == '\0')return true;
	if (str->buffer[0] == '\0' || ch[0] == '\0')return false;

	size_t len = strlen(ch);
	if (str->cachedLen != len)return false;
	for (size_t i = 0; i < len; i++) {
		if (toLowerInternal((unsigned char)str->buffer[i], mode) !=
			toLowerInternal((unsigned char)ch[i], mode))return false;
	}
	return true;
}

bool jcstr_containsWithInst(const JC_String* const str1, const JC_String* const str2) {
	if (!str1 && !str2)return true; // NULL 包含 NULL
	if (!str1 || !str2)return false;
	if (!str1->buffer && !str2->buffer)return true;
	if (!str1->buffer || !str2->buffer)return false;

	return strstr(str1->buffer, str2->buffer) != NULL;
}

bool jcstr_containsIgnoreCaseWithInst(const JC_String* const str1, const JC_String* const str2, JC_SpaceMode mode) {
	if (!str1 && !str2)return true; // NULL 包含 NULL
	if (!str1 || !str2)return false;
	if (!str1->buffer && !str2->buffer)return true;
	if (!str1->buffer || !str2->buffer)return false;

	return containsIgnoreCase(str1->buffer, str2->buffer, mode);
}

bool jcstr_containsWithCh(const JC_String* const str, const char* ch) {
	if (!str)return false;
	if (!str->buffer && !ch)return true;
	if (!str->buffer || !ch)return false;

	return strstr(str->buffer, ch) != NULL;
}

bool jcstr_containsIgnoreCaseWithCh(const JC_String* const str, const char* const ch, JC_SpaceMode mode) {
	if (!str)return false;
	if (!str->buffer && !ch)return true;
	if (!str->buffer || !ch)return false;

	return containsIgnoreCase(str->buffer, ch, mode);
}

int jcstr_compareWithInst(const JC_String* const str1, const JC_String* const str2, int* result) {
	if (!result)return JC_STR__NULL_RECEIVER;
	if (!str1 && !str2) {
		*result = 0;
		return JC_STR__SUCCESSFUL_OP;
	}
	if (!str1 || !str2)return JC_STR__NULL;
	if (!str1->buffer && !str2->buffer) {
		*result = 0;
		return JC_STR__SUCCESSFUL_OP;
	}
	if (!str1->buffer || !str2->buffer)return JC_STR__NULL_BUFFER;
	*result = strcmp(str1->buffer, str2->buffer);
	return JC_STR__SUCCESSFUL_OP;
}

int jcstr_compareIgnoreCaseWithInst(const JC_String* const str1, const JC_String* const str2, int* result, JC_SpaceMode mode) {
	if (!result)return JC_STR__NULL_RECEIVER;
	if (!str1 && !str2) {
		*result = 0;
		return JC_STR__SUCCESSFUL_OP;
	}
	if (!str1 || !str2)return JC_STR__NULL;
	if (!str1->buffer && !str2->buffer) {
		*result = 0;
		return JC_STR__SUCCESSFUL_OP;
	}
	if (!str1->buffer || !str2->buffer)return JC_STR__NULL_BUFFER;
	*result = compareIgnoreCase(str1->buffer, str2->buffer, mode);
	return JC_STR__SUCCESSFUL_OP;
}

int jcstr_compareWithCh(const JC_String* const str, const char* const ch, int* result) {
	if (!result)return JC_STR__NULL_RECEIVER;
	if (!str)return JC_STR__NULL;
	if (!str->buffer && !ch) {
		*result = 0;
		return JC_STR__SUCCESSFUL_OP;
	}
	if (!str->buffer || !ch)return JC_STR__NULL_BUFFER;
	*result = strcmp(str->buffer, ch);
	return JC_STR__SUCCESSFUL_OP;
}

int jcstr_compareIgnoreCaseWithCh(const JC_String* const str, const char* const ch, int* result, JC_SpaceMode mode) {
	if (!result)return JC_STR__NULL_RECEIVER;
	if (!str)return JC_STR__NULL;
	if (!str->buffer && !ch) {
		*result = 0;
		return JC_STR__SUCCESSFUL_OP;
	}
	if (!str->buffer || !ch)return JC_STR__NULL_BUFFER;
	*result = compareIgnoreCase(str->buffer, ch, mode);
	return JC_STR__SUCCESSFUL_OP;
}


char* jcstr_toCharArray(const JC_String* const str) {
	if (!str || !str->buffer)return NULL;
	if (str->buffer[0] == '\0') {
		char* temp = (char*)calloc(1, sizeof(char));
		if (!temp)return NULL;
		temp[0] = '\0';
		return temp;
	}

	size_t len = str->cachedLen;
	char* content = (char*)malloc(len + 1);
	if (content) {
		memcpy(content, str->buffer, len);
		content[len] = '\0';
	}
	return content;
}

int jcstr_toInt(const JC_String* const str, int* result, JC_SpaceMode mode) {
	if (!str || !str->buffer || str->buffer[0] == '\0') return JC_STR__EMPTY;
	if (str->status != JC_STR__AVAILABLE) return str->status;
	if (!result) return JC_STR__NULL_INT;

	long long v;
	int s = parseToLL(str->buffer, str->cachedLen, &v, mode);
	if (s != JC_STR__SUCCESSFUL_OP) {
		if (s == JC_STR__BLANK)       return JC_STR__BLANK;
		if (s == JC_STR__NOT_LONGLONG) return JC_STR__NOT_INT;
		return JC_STR__OVERFLOW_INT;
	}
	if (v < INT_MIN || v > INT_MAX) return JC_STR__OVERFLOW_INT;   // ← 关键：范围检查
	*result = (int)v;
	return JC_STR__SUCCESSFUL_OP;
}

int jcstr_toLong(const JC_String* const str, long* result, JC_SpaceMode mode) {
	if (!str || !str->buffer || str->buffer[0] == '\0') return JC_STR__EMPTY;
	if (str->status != JC_STR__AVAILABLE) return str->status;
	if (!result) return JC_STR__NULL_LONG;

	long long v;
	int s = parseToLL(str->buffer, str->cachedLen, &v, mode);
	if (s != JC_STR__SUCCESSFUL_OP) {
		if (s == JC_STR__BLANK)       return JC_STR__BLANK;
		if (s == JC_STR__NOT_LONGLONG) return JC_STR__NOT_LONG;
		return JC_STR__OVERFLOW_LONG;
	}
	if (v < LONG_MIN || v > LONG_MAX) return JC_STR__OVERFLOW_LONG;   // ← 关键：范围检查
	*result = (long)v;
	return JC_STR__SUCCESSFUL_OP;
}

int jcstr_toLongLong(const JC_String* const str, long long* result, JC_SpaceMode mode) {
	if (!str || !str->buffer || str->buffer[0] == '\0') return JC_STR__EMPTY;
	if (str->status != JC_STR__AVAILABLE) return str->status;
	if (!result) return JC_STR__NULL_LONGLONG;

	int s = parseToLL(str->buffer, str->cachedLen, result, mode);
	if (s == JC_STR__SUCCESSFUL_OP)      return JC_STR__SUCCESSFUL_OP;
	if (s == JC_STR__BLANK)             return JC_STR__BLANK;
	if (s == JC_STR__NOT_LONGLONG)       return JC_STR__NOT_LONGLONG;
	return JC_STR__OVERFLOW_LONGLONG;
}

int jcstr_toFloat(const JC_String* const str, float* result, JC_SpaceMode mode) {
	if (!str || !str->buffer || str->buffer[0] == '\0') return JC_STR__EMPTY;
	if (str->status != JC_STR__AVAILABLE) return str->status;
	if (!result) return JC_STR__NULL_FLOAT;

	double v;
	int s = parseToDouble(str->buffer, str->cachedLen, &v, mode);
	if (s != JC_STR__SUCCESSFUL_OP) {
		if (s == JC_STR__BLANK)     return JC_STR__BLANK;
		if (s == JC_STR__NOT_DOUBLE) return JC_STR__NOT_FLOAT;
		return JC_STR__OVERFLOW_FLOAT;
	}
	if (isinf(v) || isnan(v)) {
		*result = (float)v;
		return JC_STR__SUCCESSFUL_OP;
	}
	if (v > FLT_MAX || v < -FLT_MAX) return JC_STR__OVERFLOW_FLOAT;   // ← float 范围检查
	*result = (float)v;
	return JC_STR__SUCCESSFUL_OP;
}

int jcstr_toDouble(const JC_String* const str, double* result, JC_SpaceMode mode) {
	if (!str || !str->buffer || str->buffer[0] == '\0') return JC_STR__EMPTY;
	if (str->status != JC_STR__AVAILABLE) return str->status;
	if (!result) return JC_STR__NULL_DOUBLE;

	double v;
	int s = parseToDouble(str->buffer, str->cachedLen, &v, mode);
	if (s == JC_STR__SUCCESSFUL_OP) { *result = v; return JC_STR__SUCCESSFUL_OP; }
	if (s == JC_STR__BLANK)           return JC_STR__BLANK;
	if (s == JC_STR__NOT_DOUBLE)       return JC_STR__NOT_DOUBLE;
	return JC_STR__OVERFLOW_DOUBLE;
}

jc_String jcstr_toLowerCase(const JC_String* const str, JC_SpaceMode mode) {
	if (!str)return NULL;
	if (!str->buffer)return jcstr_newFrom(NULL);
	if (str->buffer[0] == '\0')return jcstr_new();

	size_t len = str->cachedLen;

	char* newBuf = (char*)malloc(len + 1);
	if (!newBuf)return wrapString(newBuf, JC_STR__NULL_BUFFER, 0);
	for (size_t i = 0; i < len; i++)newBuf[i] = (char)toLowerInternal((unsigned char)str->buffer[i], mode);
	newBuf[len] = '\0';
	jc_String result = wrapString(newBuf, JC_STR__AVAILABLE, len);
	if (!result)free(newBuf);
	return result;
}

jc_String jcstr_toUpperCase(const JC_String* const str, JC_SpaceMode mode) {
	if (!str)return NULL;
	if (!str->buffer)return jcstr_newFrom(NULL);
	if (str->buffer[0] == '\0')return jcstr_new();

	size_t len = str->cachedLen;

	char* newBuf = (char*)malloc(len + 1);
	if (!newBuf)return wrapString(newBuf, JC_STR__NULL_BUFFER, 0);
	for (size_t i = 0; i < len; i++)newBuf[i] = (char)toUpperInternal((unsigned char)str->buffer[i], mode);
	newBuf[len] = '\0';
	jc_String result = wrapString(newBuf, JC_STR__AVAILABLE, len);
	if (!result)free(newBuf);
	return result;
}

jc_MutableString jcstr_toMutableString(const JC_String* const str) {
	if (!str)return NULL;
	const char* temp = str->buffer;
	return jcms_newFrom(temp);
}