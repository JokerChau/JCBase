#include "JC_String.h"
#include "structures.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include <stdint.h>
#include <float.h>
#include "JC_MutableString.h"

// 将已知的char*、状态和长度封装成一个string实例，不成功则返回NULL
static string wrapString(char* buffer, int status, size_t length) {
	string temp = (string)malloc(sizeof(String));
	if (!temp)return NULL;
	temp->buffer = buffer;
	temp->status = status;
	temp->cachedLen = length;
	return temp;
}

// 将内容复制后带着长度打包成string实例，len为有效长度
static string pkgString(char* content, size_t len) {
	char* tempBuf = (char*)malloc(len + 1);
	if (!tempBuf)return wrapString(tempBuf, STR_NULLBUFFER, 0);
	memcpy(tempBuf, content, len);
	tempBuf[len] = '\0';
	string result = wrapString(tempBuf, STR_AVAILABLE, len);
	if (!result)free(tempBuf);
	return result;
}

// 通过两个char*和两个长度组装一个string实例
static string appendCore(char* str1, char* str2, size_t len1, size_t len2) {
	if (len1 > SIZE_MAX - len2 - 1)return wrapString(NULL, STR_LENOUTOFTYPEMAX, 0);

	char* combined = (char*)malloc(len1 + len2 + 1);
	// 如果内容结合指针申请内存失败，则返回一个状态为STR_FAILEDTOCOMBINE的实例
	if (!combined)return wrapString(combined, STR_FAILEDTOCOMBINE, 0);

	// 将内容复制到combined
	memcpy(combined, str1, len1);
	memcpy(combined + len1, str2, len2 + 1);

	string result = wrapString(combined, STR_AVAILABLE, len1 + len2);
	if (!result)free(combined);
	return result;
}

// 通过从前往后依据长度的sub
static string subLenFrontCore(char* base, size_t startIdx, size_t givenLen, size_t realLen) {
	if (startIdx >= realLen) {
		char* tempBuf = (char*)malloc(1);
		if (!tempBuf) return wrapString(tempBuf, STR_NULLBUFFER, 0);   // 分配失败才是 NULL buffer
		tempBuf[0] = '\0';
		return wrapString(tempBuf, STR_INVALIDSTART, 0);
	}

	if (givenLen > realLen - startIdx)givenLen = realLen - startIdx;

	return pkgString(base + startIdx, givenLen);
}

// 通过从后往前依据长度的sub
static string subLenBackCore(char* base, size_t endIdx, size_t givenLen, size_t realLen) {
	if (endIdx >= realLen) {
		char* tempBuf = (char*)malloc(1);
		if (!tempBuf)return wrapString(tempBuf, STR_NULLBUFFER, 0);
		tempBuf[0] = '\0';
		return wrapString(tempBuf, STR_INVALIDEND, 0);
	}

	if (endIdx + 1 < givenLen)givenLen = endIdx + 1;

	return pkgString(base + (endIdx - givenLen + 1), givenLen);
}

// 通过左闭右开取范围的sub
static string subRangeCore(char* str, size_t start, size_t end, size_t realLen) {
	if (start > end) {
		char* tempBuf = (char*)malloc(1);
		if (!tempBuf)return wrapString(tempBuf, STR_NULLBUFFER, 0);
		tempBuf[0] = '\0';
		return wrapString(tempBuf, STR_INVALIDIDX, 0);
	}
	if (start >= realLen || end > realLen) {
		char* tempBuf = (char*)malloc(1);
		if (!tempBuf)return wrapString(tempBuf, STR_NULLBUFFER, 0);
		tempBuf[0] = '\0';
		return wrapString(tempBuf, STR_IDXOUTBOUNDS, 0);
	}

	size_t length = end - start;

	return pkgString(str + start, length);
}

// 忽略大小写的子串查找——needle 保证非空
static bool containsIgnoreCase(const char* haystack, const char* needle) {
	size_t hayLen = strlen(haystack);
	size_t needleLen = strlen(needle);
	if (needleLen > hayLen) return false;

	for (size_t i = 0; i + needleLen <= hayLen; i++) {
		bool match = true;
		for (size_t j = 0; j < needleLen; j++) {
			if (tolower((unsigned char)haystack[i + j]) !=
				tolower((unsigned char)needle[j])) {
				match = false;
				break;
			}
		}
		if (match) return true;
	}
	return false;
}

// 忽略大小写的字符串比较——返回 <0 / 0 / >0
static int compareIgnoreCase(const char* a, const char* b) {
	while (*a && *b) {
		int ca = tolower((unsigned char)*a);
		int cb = tolower((unsigned char)*b);
		if (ca != cb) return ca - cb;
		a++;
		b++;
	}
	return (unsigned char)*a - (unsigned char)*b;
}

// 解析 buffer 的 [0, len) 为一个 long long
// 返回 STR_SUCCESSFULOP / STR_BLANK / STR_NOTLONGLONG / STR_OVERFLOWLONGLONG
static int parseToLL(const char* buf, size_t len, long long* out) {
	size_t start = 0;
	while (start < len && isspace((unsigned char)buf[start])) start++;
	size_t end = len;
	while (end > start && isspace((unsigned char)buf[end - 1])) end--;
	if (start == end) return STR_BLANK;

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
			if (temp < (limit + d) / 10) return STR_OVERFLOWLONGLONG;
			temp = 10 * temp - d;
			count++;
		}
		else {
			return STR_NOTLONGLONG;
		}
	}

	if (!count) return STR_NOTLONGLONG;
	*out = isNeg ? temp : -temp;
	return STR_SUCCESSFULOP;
}

static int parseToDouble(const char* buf, size_t len, double* out) {
	size_t start = 0;
	while (start < len && isspace((unsigned char)buf[start])) start++;
	size_t end = len;
	while (end > start && isspace((unsigned char)buf[end - 1])) end--;
	if (start == end) return STR_BLANK;

	char* endptr = NULL;
	errno = 0;
	double value = strtod(buf + start, &endptr);

	if (endptr != buf + end) return STR_NOTDOUBLE;
	if (errno == ERANGE)     return STR_OVERFLOWDOUBLE;

	*out = value;
	return STR_SUCCESSFULOP;
}

/* =============================================================== */
// 默认构造器，返回一个内容为\0的实例
string jcstr_new(void) {
	return jcstr_newFrom("");
}

// 通过char*构造string
string jcstr_newFrom(const char* const content) {
	if (!content)return wrapString(NULL, STR_NULLBUFFER, 0);

	size_t len = strlen(content);
	return pkgString(content, len);
}
/* =============================================================== */

/* =============================================================== */
// 析构，可传NULL
void jcstr_delete(string str) {
	if (!str)return;
	if (str->buffer) {
		free(str->buffer);
	}
	free(str);
}
/* =============================================================== */

/* =============================================================== */
// 状态返回器，在使用实例前一定调用此函数检查实例状态是否可用，使用方法：if(getStringStatus(str)!=STR_AVAILABLE){ // 该实例状态异常}
int jcstr_getStatus(const String* const str) {
	if (!str)return STR_NULL;
	return str->status;
}
/* =============================================================== */

/* =============================================================== */
// 获取实例长度
size_t jcstr_getLength(const String* const str) {
	if (!str)return 0;
	return str->cachedLen;
}
/* =============================================================== */

/* =============================================================== */
const char* jcstr_statusToCharArray(const int status) {
	switch (status) {
	case STR_NULL: {
		return "null string";
	}
	case STR_AVAILABLE: {
		return "available string";
	}
	case STR_NULLBUFFER: {
		return "null buffer";
	}
	case STR_EMPTYBUFFER: {
		return "buffer content null";
	}
	case STR_EMPTY: {
		return "empty string";
	}
	case STR_BLANK: {
		return "blank string";
	}
	case STR_LENOUTOFTYPEMAX: {
		return "length of the instance is out of max size of some type";
	}

	case STR_FAILEDTOCOMBINE: {
		return "failed to combine";
	}
	case STR_WRONGSTATUS: {
		return "wrong string status";
	}
	case STR_FAILEDTOREALLOC: {
		return "failed to realloc";
	}
	case STR_FAILEDTOALLOC: {
		return "failed to alloc";
	}

	case STR_NULLINT: {
		return "null int pointer";
	}
	case STR_OVERFLOWINT: {
		return "overflow int range";
	}
	case STR_NOTINT: {
		return "not int";
	}
	case STR_SUCCEEDTOINT: {
		return "succeed to change string into int";
	}

	case STR_NULLLONG: {
		return "null long int pointer";
	}
	case STR_OVERFLOWLONG: {
		return "overflow long int range";
	}
	case STR_NOTLONG: {
		return "not long int";
	}
	case STR_SUCCEEDTOLONG: {
		return "succeed to change string into long int";
	}

	case STR_NULLLONGLONG: {
		return "null long long int pointer";
	}
	case STR_OVERFLOWLONGLONG: {
		return "overflow long long int range";
	}
	case STR_NOTLONGLONG: {
		return "not long long int";
	}
	case STR_SUCCEEDTOLONGLONG: {
		return "succeed to change string into long long int";
	}

	case STR_NULLFLOAT: {
		return "null float pointer";
	}
	case STR_OVERFLOWFLOAT: {
		return "overflow float range";
	}
	case STR_NOTFLOAT: {
		return "not float";
	}
	case STR_SUCCEEDTOFLOAT: {
		return "succeed to change string into float";
	}

	case STR_NULLDOUBLE: {
		return "null double pointer";
	}
	case STR_OVERFLOWDOUBLE: {
		return "overflow double range";
	}
	case STR_NOTDOUBLE: {
		return "not double";
	}
	case STR_SUCCEEDTODOUBLE: {
		return "succeed to change string into double";
	}

	case STR_NULLCHAR: {
		return "null char pointer";
	}
	case STR_NULLARRAY: {
		return "null string array";
	}
	case STR_NULLRECEIVER: {
		return "receiver is null";
	}

	case STR_IDXOUTBOUNDS: {
		return "index out of bounds";
	}
	case STR_SUCCESSFULIDX: {
		return "succeed to use the index";
	}
	case STR_SUCCEEDTOTRUNCATE: {
		return "succeed to truncate the string";
	}
	case STR_SUCCEEDTOFINDCH: {
		return "succeed to find the char you want";
	}
	case STR_WITHOUTCH: {
		return "there is no char you want here";
	}
	case STR_INVALIDSTART: {
		return "invalid start index";
	}
	case STR_INVALIDEND: {
		return "invalid end index";
	}
	case STR_INVALIDIDX: {
		return "invalid index";
	}
	case STR_INVALIDREPLACEMENT: {
		return "what you want to replace with is invalid";
	}

	case STR_NULLCOUNT: {
		return "count ptr null";
	}
	case STR_SUCCESSFULOP: {
		return "successful operation";
	}
	case STR_PARTIALSPLIT: {
		return "string partial split";
	}
	default: {
		return "Unknown status";
	}
	}
}
/* =============================================================== */




/* =============================================================== */
string jcstr_appendWithInst(const String* const str1, const String* const str2) {
	// 实例均为NULL，返回NULL
	if (!str1 && !str2)return NULL;

	const char* buf1 = (str1 && str1->buffer) ? str1->buffer : NULL;
	const char* buf2 = (str2 && str2->buffer) ? str2->buffer : NULL;

	if (!buf1 && !buf2)return jcstr_newFrom(NULL);
	if (!buf1)return jcstr_newFrom(buf2);
	if (!buf2)return jcstr_newFrom(buf1);
	// 如果buffer都不为NULL

	return appendCore(buf1, buf2, str1->cachedLen, str2->cachedLen);
}

string jcstr_appendWithCh(const String* const str, const char* const content) {
	if (!str && !content)return NULL;
	const char* buf = (str && str->buffer) ? str->buffer : NULL;
	if (!buf && !content)return jcstr_newFrom(NULL);
	if (!buf)return jcstr_newFrom(content);
	if (!content)return jcstr_newFrom(buf);

	return appendCore(buf, content, str->cachedLen, strlen(content));
}

string jcstr_reverse(const String* const str) {
	if (!str)return NULL;
	if (!str->buffer)return jcstr_newFrom(NULL);
	if (str->buffer[0] == '\0')return jcstr_new();

	size_t len = str->cachedLen;

	char* tempBuffer = (char*)malloc(len + 1);
	if (!tempBuffer)return wrapString(tempBuffer, STR_NULLBUFFER, 0);

	memcpy(tempBuffer, str->buffer, len + 1);

	for (size_t i = 0; i < len / 2; i++) {
		char c = tempBuffer[i];
		tempBuffer[i] = tempBuffer[len - 1 - i];
		tempBuffer[len - 1 - i] = c;
	}

	string temp = wrapString(tempBuffer, STR_AVAILABLE, len);
	if (!temp)free(tempBuffer);
	return temp;
}

int jcstr_split(const String* const str, char delimiter, string** outArray, size_t* count, bool removeEmptyEntries) {
	if (!outArray) return STR_NULLARRAY;
	if (!count) return STR_NULLCOUNT;

	*outArray = NULL;
	*count = 0;

	if (!str) return STR_NULL;
	if (!str->buffer) return STR_NULLBUFFER;

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

	if (wholeCount == 0) return STR_SUCCESSFULOP;

	string* stringArray = (string*)malloc(sizeof(string) * wholeCount);
	if (!stringArray) return STR_FAILEDTOALLOC;

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
				stringArray[i] = wrapString(temp, STR_AVAILABLE, segLen);
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
	return hasNull ? STR_PARTIALSPLIT : STR_SUCCESSFULOP;
}

string jcstr_trim(const String* const str) {
	if (!str)return NULL;
	if (!str->buffer)return jcstr_newFrom(NULL);
	if (str->buffer[0] == '\0')return jcstr_new();

	size_t len = str->cachedLen;
	// 设定开始位置
	size_t start = 0;
	while (start < len && isspace((unsigned char)str->buffer[start]))start++;
	if (start == len)return jcstr_new();

	// 设定末位位置
	size_t end = len - 1;
	while (end > start && isspace((unsigned char)str->buffer[end]))end--;

	size_t newLen = end - start + 1;

	return pkgString(str->buffer + start, newLen);
}

string jcstr_trimFront(const String* const str) {
	if (!str)return NULL;
	if (!str->buffer)return jcstr_newFrom(NULL);
	if (str->buffer[0] == '\0')return jcstr_new();

	size_t len = str->cachedLen;
	// 设定开始位置
	size_t start = 0;
	while (start < len && isspace((unsigned char)str->buffer[start]))start++;
	if (start == len)return jcstr_new();

	size_t newLen = len - start;

	return pkgString(str->buffer + start, newLen);
}

string jcstr_trimBack(const String* const str) {
	if (!str)return NULL;
	if (!str->buffer)return jcstr_newFrom(NULL);
	if (str->buffer[0] == '\0')return jcstr_new();

	// 设定末位位置
	size_t newLen = str->cachedLen;
	while (newLen > 0 && isspace((unsigned char)str->buffer[newLen - 1]))newLen--;

	return pkgString(str->buffer, newLen);
}

string jcstr_clone(const String* const str) {
	if (!str)return NULL;
	return jcstr_newFrom(str->buffer);
}

int jcstr_charAt(const String* const str, size_t index, char* result) {
	if (!result)return STR_NULLCHAR;
	if (!str)return STR_NULL;
	if (!str->buffer)return STR_NULLBUFFER;
	if (index >= str->cachedLen)return STR_IDXOUTBOUNDS;
	*result = str->buffer[index];
	return STR_SUCCESSFULIDX;
}

int jcstr_indexOfFront(const String* const str, char ch, size_t* result) {
	if (!result)return STR_NULLRECEIVER;
	if (!str)return STR_NULL;
	if (!str->buffer)return STR_NULLBUFFER;

	const char* found = strchr(str->buffer, ch);
	if (found) {
		*result = (size_t)(found - str->buffer);
		return STR_SUCCEEDTOFINDCH;
	}
	return STR_WITHOUTCH;
}

int jcstr_indexOfBack(const String* const str, char ch, size_t* result) {
	if (!result)return STR_NULLRECEIVER;
	if (!str)return STR_NULL;
	if (!str->buffer)return STR_NULLBUFFER;
	for (ptrdiff_t i = (ptrdiff_t)str->cachedLen - 1; i >= 0; --i) {
		if (str->buffer[i] == ch) {
			*result = (size_t)i;
			return STR_SUCCEEDTOFINDCH;
		}
	}
	return STR_WITHOUTCH;
}

int jcstr_indexOfFrontWithInst(const String* const str, const String* const sub, size_t* result) {
	if (!result)return STR_NULLRECEIVER;
	if (!str || !sub)return STR_NULL;
	if (!str->buffer || !sub->buffer)return STR_NULLBUFFER;
	if (sub->buffer[0] == '\0') {
		*result = str->cachedLen;
		return STR_SUCCESSFULOP;
	}
	const char* found = strstr(str->buffer, sub->buffer);
	if (!found)return STR_WITHOUTCH;
	*result = (size_t)(found - str->buffer);
	return STR_SUCCESSFULOP;
}

int jcstr_indexOfFrontWithCh(const String* const str, const char* const ch, size_t* result) {
	if (!result)return STR_NULLRECEIVER;
	if (!str)return STR_NULL;
	if (!str->buffer || !ch)return STR_NULLBUFFER;
	if (ch[0] == '\0') {
		*result = str->cachedLen;
		return STR_SUCCESSFULOP;
	}
	const char* found = strstr(str->buffer, ch);
	if (!found)return STR_WITHOUTCH;
	*result = (size_t)(found - str->buffer);
	return STR_SUCCESSFULOP;
}

string jcstr_subInLengthFrontWithInst(const String* const str, size_t start, size_t length) {
	if (!str)return NULL;
	if (!str->buffer)return jcstr_newFrom(NULL);
	if (str->buffer[0] == '\0' || !length)return jcstr_new();

	return subLenFrontCore(str->buffer, start, length, str->cachedLen);
}

string jcstr_subInLengthBackWithInst(const String* const str, size_t end, size_t length) {
	if (!str)return NULL;
	if (!str->buffer)return jcstr_newFrom(NULL);
	if (str->buffer[0] == '\0' || !length)return jcstr_new();

	return subLenBackCore(str->buffer, end, length, str->cachedLen);
}

string jcstr_subInRangeWithInst(const String* const str, size_t start, size_t end) {
	if (!str)return NULL;
	if (!str->buffer)return jcstr_newFrom(NULL);
	if (str->buffer[0] == '\0' || start == end)return jcstr_new();

	return subRangeCore(str->buffer, start, end, str->cachedLen);
}

string jcstr_subInLengthFrontWithCh(const char* const ch, size_t start, size_t length) {
	if (!ch)return jcstr_newFrom(NULL);
	if (ch[0] == '\0' || !length)return jcstr_new();

	return subLenFrontCore(ch, start, length, strlen(ch));
}

string jcstr_subInLengthBackWithCh(const char* const ch, size_t end, size_t length) {
	if (!ch)return jcstr_newFrom(NULL);
	if (ch[0] == '\0' || !length)return jcstr_new();

	return subLenBackCore(ch, end, length, strlen(ch));
}

string jcstr_subInRangeWithCh(const char* const ch, size_t start, size_t end) {
	if (!ch)return jcstr_newFrom(NULL);
	if (ch[0] == '\0' || start == end)return jcstr_new();

	return subRangeCore(ch, start, end, strlen(ch));
}

string jcstr_replaceFirstWithInst(const String* const str, const char* const target, const String* const replacement) {
	if (!str)return NULL;
	if (!replacement) {
		if (!str->buffer)return jcstr_newFrom(NULL);
		char* tempBuf = (char*)malloc(str->cachedLen + 1);
		if (!tempBuf) return wrapString(tempBuf, STR_NULLBUFFER, 0);
		memcpy(tempBuf, str->buffer, str->cachedLen + 1);
		return wrapString(tempBuf, STR_INVALIDREPLACEMENT, str->cachedLen);
	}
	return jcstr_replaceFirstWithCh(str, target, replacement->buffer);
}

string jcstr_replaceAllWithInst(const String* const str, const char* const target, const String* const replacement) {
	if (!str)return NULL;
	if (!replacement) {
		char* tempBuf = (char*)malloc(str->cachedLen + 1);
		if (!tempBuf) return wrapString(NULL, STR_NULLBUFFER, 0);
		memcpy(tempBuf, str->buffer, str->cachedLen + 1);
		return wrapString(tempBuf, STR_INVALIDREPLACEMENT, str->cachedLen);
	}
	return jcstr_replaceAllWithCh(str, target, replacement->buffer);
}

string jcstr_replaceFirstWithCh(const String* const str, const char* const target, const char* const replacement) {
	if (!str)return NULL;

	bool bufEmpty = (!str->buffer || str->buffer[0] == '\0');
	bool tgtEmpty = (!target || target[0] == '\0');

	if (bufEmpty && tgtEmpty)return jcstr_newFrom(replacement);
	if (bufEmpty || tgtEmpty)return jcstr_newFrom(str->buffer);

	if (!replacement) {
		char* tempBuf = (char*)malloc(str->cachedLen + 1);
		if (!tempBuf) return wrapString(tempBuf, STR_NULLBUFFER, 0);
		memcpy(tempBuf, str->buffer, str->cachedLen + 1);
		return wrapString(tempBuf, STR_INVALIDREPLACEMENT, str->cachedLen);
	}

	char* indexChar = strstr(str->buffer, target);
	if (!indexChar)return jcstr_newFrom(str->buffer);
	size_t index = (size_t)(indexChar - str->buffer);

	size_t targetLen = strlen(target);
	size_t replaceLen = strlen(replacement);
	size_t strLen = str->cachedLen;
	size_t sufLen = strLen - index - targetLen;
	size_t newLen = index + replaceLen + sufLen;

	char* newBuffer = (char*)malloc(newLen + 1);
	if (!newBuffer)return wrapString(NULL, STR_NULLBUFFER, 0);

	memcpy(newBuffer, str->buffer, index);
	memcpy(newBuffer + index, replacement, replaceLen);
	memcpy(newBuffer + index + replaceLen, indexChar + targetLen, sufLen + 1);

	string result = wrapString(newBuffer, STR_AVAILABLE, newLen);
	if (!result)free(newBuffer);
	return result;
}

string jcstr_replaceAllWithCh(const String* const str, const char* const target, const char* const replacement) {
	if (!str)return NULL;

	bool bufEmpty = (!str->buffer || str->buffer[0] == '\0');
	bool tgtEmpty = (!target || target[0] == '\0');

	if (bufEmpty && tgtEmpty) return jcstr_newFrom(replacement);
	if (bufEmpty || tgtEmpty) return jcstr_newFrom(str->buffer);

	if (!replacement) {
		char* tempBuf = (char*)malloc(str->cachedLen + 1);
		if (!tempBuf) return wrapString(tempBuf, STR_NULLBUFFER, 0);
		memcpy(tempBuf, str->buffer, str->cachedLen + 1);
		return wrapString(tempBuf, STR_INVALIDREPLACEMENT, str->cachedLen);
	}

	char* src = str->buffer;
	size_t strLen = str->cachedLen;
	size_t targetLen = strlen(target);
	size_t replaceLen = strlen(replacement);

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
				return wrapString(NULL, STR_NULLBUFFER, 0);
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
		return wrapString(NULL, STR_NULLBUFFER, 0);
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

	string result = wrapString(newBuffer, STR_AVAILABLE, newLen);
	if (!result)free(newBuffer);
	return result;
}

bool jcstr_startWithCh(const String* const str, const char* const prefix) {
	if (!str)return false;
	if (!str->buffer)return !prefix ? true : false;
	if (!prefix)return false;
	if (prefix[0] == '\0')return true;

	size_t strLen = str->cachedLen;
	size_t preLen = strlen(prefix);
	if (strLen < preLen)return false;

	return strncmp(str->buffer, prefix, preLen) == 0;
}

bool jcstr_endWithCh(const String* const str, const char* const suffix) {
	if (!str)return false;
	if (!str->buffer)return !suffix ? true : false;
	if (!suffix)return false;
	if (suffix[0] == '\0')return true;

	size_t strLen = str->cachedLen;
	size_t sufLen = strlen(suffix);
	if (strLen < sufLen)return false;

	return strncmp(str->buffer + (strLen - sufLen), suffix, sufLen) == 0;
}

bool jcstr_startWithInst(const String* const str, const String* const prefix) {
	if (!prefix)return false;
	return jcstr_startWithCh(str, prefix->buffer);
}

bool jcstr_endWithInst(const String* const str, const String* const suffix) {
	if (!suffix)return false;
	return jcstr_endWithCh(str, suffix->buffer);
}

bool jcstr_isEmpty(const String* const str) {
	return !str || !str->buffer || str->buffer[0] == '\0';
}

bool jcstr_isBlank(const String* const str) {
	if (!str || !str->buffer)return true;

	for (const char* p = str->buffer; *p; p++) {
		if (!isspace((unsigned char)*p)) {
			return false;
		}
	}

	return true;
}

bool jcstr_equalsWithInst(const String* const str1, const String* const str2) {
	if (str1 == str2)return true;
	if (!str1 || !str2)return false;

	if ((!str1->buffer || str1->buffer[0] == '\0') && (!str2->buffer || str2->buffer[0] == '\0'))return true;
	if ((!str1->buffer || str1->buffer[0] == '\0') || (!str2->buffer || str2->buffer[0] == '\0'))return false;

	return strcmp(str1->buffer, str2->buffer) == 0;
}

bool jcstr_equalsIgnoreCaseWithInst(const String* const str1, const String* const str2) {
	if (str1 == str2)return true;
	if (!str1 || !str2)return false;
	if ((!str1->buffer || str1->buffer[0] == '\0') && (!str2->buffer || str2->buffer[0] == '\0'))return true;
	if ((!str1->buffer || str1->buffer[0] == '\0') || (!str2->buffer || str2->buffer[0] == '\0'))return false;

	if (str1->cachedLen != str2->cachedLen)return false;

	for (size_t i = 0; i < str1->cachedLen; i++)if (tolower((unsigned char)str1->buffer[i]) != tolower((unsigned char)str2->buffer[i]))return false;
	return true;
}

bool jcstr_equalsWithCh(const String* const str, const char* const ch) {
	if ((!str || !str->buffer) && !ch)return true;
	if (((!str || !str->buffer) && ch) || ((str && str->buffer) && !ch))return false;
	return strcmp(str->buffer, ch) == 0;
}

bool jcstr_equalsIgnoreCaseWithCh(const String* const str, const char* const ch) {
	if (!str || !str->buffer || !ch)return false;
	for (size_t i = 0; i < str->cachedLen; i++)if (tolower((unsigned char)str->buffer[i]) != tolower((unsigned char)ch[i]))return false;
	return true;
}

bool jcstr_containsWithInst(const String* const str1, const String* const str2) {
	if (!str2)return true;
	if (!str2->buffer)return str1 != NULL;
	if (str2->buffer[0] == '\0') {
		if (!str1 || !str1->buffer)return false;
		return true;
	}

	if (!str1 || !str1->buffer || str1->buffer[0] == '\0')return false;

	return strstr(str1->buffer, str2->buffer) != NULL;
}

bool jcstr_containsIgnoreCaseWithInst(const String* const str1, const String* const str2) {
	if (!str2) return true;
	if (!str2->buffer) return str1 != NULL;
	if (str2->buffer[0] == '\0') {
		if (!str1 || !str1->buffer) return false;
		return true;
	}

	if (!str1 || !str1->buffer || str1->buffer[0] == '\0') return false;

	return containsIgnoreCase(str1->buffer, str2->buffer);
}

bool jcstr_containsWithCh(const String* const str, const char* ch) {
	if (!ch)return str != NULL;
	if (ch[0] == '\0') {
		if (!str || !str->buffer)return false;
		return true;
	}

	if (!str || !str->buffer || str->buffer[0] == '\0')return false;

	return strstr(str->buffer, ch) != NULL;
}

bool jcstr_containsIgnoreCaseWithCh(const String* const str, const char* const ch) {
	if (!ch)return str != NULL;
	if (ch[0] == '\0') {
		if (!str || !str->buffer)return false;
		return true;
	}

	if (!str || !str->buffer || str->buffer[0] == '\0')return false;

	return containsIgnoreCase(str->buffer, ch);
}

int jcstr_compareWithInst(const String* const str1, const String* const str2, int* result) {
	if (!str1 || !str2)return STR_NULL;
	if (!str1->buffer || !str2->buffer)return STR_NULLBUFFER;
	if (!result)return STR_NULLRECEIVER;
	*result = strcmp(str1->buffer, str2->buffer);
	return STR_SUCCESSFULOP;
}

int jcstr_compareIgnoreCaseWithInst(const String* const str1, const String* const str2, int* result) {
	if (!str1 || !str2)return STR_NULL;
	if (!str1->buffer || !str2->buffer)return STR_NULLBUFFER;
	if (!result)return STR_NULLRECEIVER;
	*result = compareIgnoreCase(str1->buffer, str2->buffer);
	return STR_SUCCESSFULOP;
}

int jcstr_compareWithCh(const String* const str, const char* const ch, int* result) {
	if (!str)return STR_NULL;
	if (!str->buffer || !ch)return STR_NULLBUFFER;
	if (!result)return STR_NULLRECEIVER;
	*result = strcmp(str->buffer, ch);
	return STR_SUCCESSFULOP;
}

int jcstr_compareIgnoreCaseWithCh(const String* const str, const char* const ch, int* result) {
	if (!str)return STR_NULL;
	if (!str->buffer || !ch)return STR_NULLBUFFER;
	if (!result)return STR_NULLRECEIVER;
	*result = compareIgnoreCase(str->buffer, ch);
	return STR_SUCCESSFULOP;
}


char* jcstr_toCharArray(const String* const str) {
	if (!str || !str->buffer)return NULL;
	if (str->buffer[0] == '\0') {
		char* temp = (char*)calloc(1, sizeof(char));
		if (!temp)return NULL;
		temp[0] = '\0';
		return temp;
	}

	size_t len = str->cachedLen;
	char* content = (char*)malloc(len + 1);
	return content ? memcpy(content, str->buffer, len) : NULL;
}

int jcstr_toInt(const String* const str, int* result) {
	if (!str || !str->buffer || str->buffer[0] == '\0') return STR_EMPTY;
	if (str->status != STR_AVAILABLE) return STR_WRONGSTATUS;
	if (!result) return STR_NULLINT;

	long long v;
	int s = parseToLL(str->buffer, str->cachedLen, &v);
	if (s != STR_SUCCESSFULOP) {
		if (s == STR_BLANK)       return STR_BLANK;
		if (s == STR_NOTLONGLONG) return STR_NOTINT;
		return STR_OVERFLOWINT;
	}
	if (v < INT_MIN || v > INT_MAX) return STR_OVERFLOWINT;   // ← 关键：范围检查
	*result = (int)v;
	return STR_SUCCEEDTOINT;
}

int jcstr_toLong(const String* const str, long* result) {
	if (!str || !str->buffer || str->buffer[0] == '\0') return STR_EMPTY;
	if (str->status != STR_AVAILABLE) return STR_WRONGSTATUS;
	if (!result) return STR_NULLLONG;

	long long v;
	int s = parseToLL(str->buffer, str->cachedLen, &v);
	if (s != STR_SUCCESSFULOP) {
		if (s == STR_BLANK)       return STR_BLANK;
		if (s == STR_NOTLONGLONG) return STR_NOTLONG;
		return STR_OVERFLOWLONG;
	}
	if (v < LONG_MIN || v > LONG_MAX) return STR_OVERFLOWLONG;   // ← 关键：范围检查
	*result = (long)v;
	return STR_SUCCEEDTOLONG;
}

int jcstr_toLongLong(const String* const str, long long* result) {
	if (!str || !str->buffer || str->buffer[0] == '\0') return STR_EMPTY;
	if (str->status != STR_AVAILABLE) return STR_WRONGSTATUS;
	if (!result) return STR_NULLLONGLONG;

	int s = parseToLL(str->buffer, str->cachedLen, result);
	if (s == STR_SUCCESSFULOP)      return STR_SUCCEEDTOLONGLONG;
	if (s == STR_BLANK)             return STR_BLANK;
	if (s == STR_NOTLONGLONG)       return STR_NOTLONGLONG;
	return STR_OVERFLOWLONGLONG;
}

int jcstr_toFloat(const String* const str, float* result) {
	if (!str || !str->buffer || str->buffer[0] == '\0') return STR_EMPTY;
	if (str->status != STR_AVAILABLE) return STR_WRONGSTATUS;
	if (!result) return STR_NULLFLOAT;

	double v;
	int s = parseToDouble(str->buffer, str->cachedLen, &v);
	if (s != STR_SUCCESSFULOP) {
		if (s == STR_BLANK)     return STR_BLANK;
		if (s == STR_NOTDOUBLE) return STR_NOTFLOAT;
		return STR_OVERFLOWFLOAT;
	}
	if (v > FLT_MAX || v < -FLT_MAX) return STR_OVERFLOWFLOAT;   // ← float 范围检查
	*result = (float)v;
	return STR_SUCCEEDTOFLOAT;
}

int jcstr_toDouble(const String* const str, double* result) {
	if (!str || !str->buffer || str->buffer[0] == '\0') return STR_EMPTY;
	if (str->status != STR_AVAILABLE) return STR_WRONGSTATUS;
	if (!result) return STR_NULLDOUBLE;

	double v;
	int s = parseToDouble(str->buffer, str->cachedLen, &v);
	if (s == STR_SUCCESSFULOP) { *result = v; return STR_SUCCEEDTODOUBLE; }
	if (s == STR_BLANK)           return STR_BLANK;
	if (s == STR_NOTDOUBLE)       return STR_NOTDOUBLE;
	return STR_OVERFLOWDOUBLE;
}

string jcstr_toLowerCase(const String* const str) {
	if (!str)return NULL;
	if (!str->buffer)return jcstr_newFrom(NULL);
	if (str->buffer[0] == '\0')return jcstr_new();

	size_t len = str->cachedLen;

	char* newBuf = (char*)malloc(len + 1);
	if (!newBuf)return wrapString(newBuf, STR_NULLBUFFER, 0);
	for (size_t i = 0; i < len; i++)newBuf[i] = (char*)tolower((unsigned char)str->buffer[i]);
	newBuf[len] = '\0';
	string result = wrapString(newBuf, STR_AVAILABLE, len);
	if (!result)free(newBuf);
	return result;
}

string jcstr_toUpperCase(const String* const str) {
	if (!str)return NULL;
	if (!str->buffer)return jcstr_newFrom(NULL);
	if (str->buffer[0] == '\0')return jcstr_new();

	size_t len = str->cachedLen;

	char* newBuf = (char*)malloc(len + 1);
	if (!newBuf)return wrapString(newBuf, STR_NULLBUFFER, 0);
	for (size_t i = 0; i < len; i++)newBuf[i] = (char*)toupper((unsigned char)str->buffer[i]);
	newBuf[len] = '\0';
	string result = wrapString(newBuf, STR_AVAILABLE, len);
	if (!result)free(newBuf);
	return result;
}

mutableString jcstr_toMutableString(const String* const str) {
	if (!str)return NULL;
	const char* temp = str->buffer;
	return jcms_newFrom(temp);
}