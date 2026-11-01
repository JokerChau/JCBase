#include "JC_MutableString.h"
#include "structures.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include "JC_String.h"

// 直接接管各种属性
static mutableString wrapString(char* buffer, int status, size_t len, size_t cap) {
	mutableString str = (mutableString)malloc(sizeof(MutableString));
	if (!str)return NULL;
	str->buffer = buffer;
	str->status = status;
	str->len = len;
	str->cap = cap;
	return str;
}

static void setString(mutableString ms, char* buffer, int status, size_t len, size_t cap) {
	ms->buffer = buffer;
	ms->status = status;
	ms->len = len;
	ms->cap = cap;
}

// caller 保证 content 有效, 但是接口会使用副本而不是接管 content， len 已算出
static mutableString pkgMS(const char* content, size_t len) {
	size_t base = len > (size_t)INITIAL_MS_SIZE ? len : (size_t)INITIAL_MS_SIZE;
	size_t cap = base + 1;                       // 与 jcms_newFrom 同口径

	char* tempBuf = (char*)malloc(cap);
	if (!tempBuf) return wrapString(NULL, MS_NULLBUFFER, 0, 0);

	memcpy(tempBuf, content, len);
	tempBuf[len] = '\0';

	mutableString result = wrapString(tempBuf, MS_AVAILABLE, len, cap);
	if (!result) free(tempBuf);
	return result;
}

static size_t ceilAlignInitSize(size_t needed) {
	if (needed > SIZE_MAX - INITIAL_MS_SIZE + 1)return needed;
	return ((needed + INITIAL_MS_SIZE - 1) / INITIAL_MS_SIZE) * INITIAL_MS_SIZE;
}

static int appendCore(mutableString ms, char** src, size_t srcLen) {
	if (ms->len > SIZE_MAX - srcLen - 1)return MS_LENOUTOFTYPEMAX;
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
		if (!tempBuf)return MS_FAILEDTOREALLOC;
		ms->buffer = tempBuf;
		ms->cap = newCap;
	}
	memmove(ms->buffer + ms->len, *src, srcLen);
	ms->len = needed - 1;
	ms->buffer[ms->len] = '\0';
	return MS_SUCCESSFULOP;
}

// mutableString构造器
mutableString jcms_new(void) {
	char* tempBuf = (char*)calloc(INITIAL_MS_SIZE + 1, sizeof(char));
	if (!tempBuf)return wrapString(tempBuf, MS_NULLBUFFER, 0, 0);
	mutableString result = wrapString(tempBuf, MS_AVAILABLE, 0, INITIAL_MS_SIZE + 1);
	if (!result)free(tempBuf);
	return result;
}

// mutableString构造器
mutableString jcms_newFrom(const char* const content) {
	if (!content)return wrapString(content, MS_NULLBUFFER, 0, 0);
	return pkgMS(content, strlen(content));
}

void jcms_delete(mutableString ms) {
	if (!ms)return;
	if (ms->buffer) {
		free(ms->buffer);
	}
	free(ms);
}

int jcms_getStatus(const MutableString* const ms) {
	if (!ms)return MS_NULL;
	return ms->status;
}

size_t jcms_getLength(const MutableString* const ms) {
	if (!ms)return 0;
	return ms->len;
}

const char* jcms_viewBuffer(const MutableString* const ms) {
	if (!ms)return NULL;
	return ms->buffer;
}

int jcms_setContent(const mutableString ms, const char* const content) {
	if (!ms)return MS_NULL;
	if (ms->status != MS_AVAILABLE)return MS_WRONGSTATUS;

	// 内容为 NULL → 清空，不释放 buffer
	if (!content) {
		ms->buffer[0] = '\0';
		ms->len = 0;
		return MS_SUCCESSFULOP;
	}

	size_t len = strlen(content);
	size_t needed = len + 1;

	if (ms->cap < needed) {
		size_t newCap = ceilAlignInitSize(needed);
		char* newBuf = (char*)realloc(ms->buffer, newCap);
		if (!newBuf)return MS_FAILEDTOREALLOC;

		ms->buffer = newBuf;
		ms->cap = newCap;
	}
	memmove(ms->buffer, content, needed);
	ms->len = len;
	return MS_SUCCESSFULOP;
}

int jcms_changeAt(const mutableString ms, size_t index, char ch) {
	if (!ms)return MS_NULL;
	if (!ms->buffer)return MS_NULLBUFFER;
	if (ms->status != MS_AVAILABLE) return MS_WRONGSTATUS;
	if (index >= ms->len)return MS_IDXOUTBOUNDS;
	if (ch == '\0')return MS_INVALIDCHFORCHANGE;

	ms->buffer[index] = ch;

	return MS_SUCCESSFULOP;
}

const char* jcms_statusToCharArray(const int status) {
	switch (status) {
	case MS_NULL: {
		return "null mutableString";
	}
	case MS_AVAILABLE: {
		return "available mutableString";
	}
	case MS_NULLBUFFER: {
		return "null buffer";
	}
	case MS_EMPTY: {
		return "empty mutableString";
	}
	case MS_BLANK: {
		return "blank mutableString";
	}

	case MS_FAILEDTOCOMBINE: {
		return "failed to combine";
	}
	case MS_WRONGSTATUS: {
		return "wrong mutableString status";
	}
	case MS_FAILEDTOREALLOC: {
		return "failed to realloc";
	}
	case MS_FAILEDTOALLOC: {
		return "failed to alloc";
	}

	case MS_NULLINT: {
		return "null int pointer";
	}
	case MS_OVERFLOWINT: {
		return "overflow int range";
	}
	case MS_NOTINT: {
		return "not int";
	}
	case MS_SUCCEEDTOINT: {
		return "succeed to change mutableString into int";
	}

	case MS_NULLLONG: {
		return "null long int pointer";
	}
	case MS_OVERFLOWLONG: {
		return "overflow long int range";
	}
	case MS_NOTLONG: {
		return "not long int";
	}
	case MS_SUCCEEDTOLONG: {
		return "succeed to change mutableString into long int";
	}

	case MS_NULLLONGLONG: {
		return "null long long int pointer";
	}
	case MS_OVERFLOWLONGLONG: {
		return "overflow long long int range";
	}
	case MS_NOTLONGLONG: {
		return "not long long int";
	}
	case MS_SUCCEEDTOLONGLONG: {
		return "succeed to change mutableString into long long int";
	}

	case MS_NULLFLOAT: {
		return "null float pointer";
	}
	case MS_OVERFLOWFLOAT: {
		return "overflow float range";
	}
	case MS_NOTFLOAT: {
		return "not float";
	}
	case MS_SUCCEEDTOFLOAT: {
		return "succeed to change mutableString into float";
	}

	case MS_NULLDOUBLE: {
		return "null double pointer";
	}
	case MS_OVERFLOWDOUBLE: {
		return "overflow double range";
	}
	case MS_NOTDOUBLE: {
		return "not double";
	}
	case MS_SUCCEEDTODOUBLE: {
		return "succeed to change mutableString into double";
	}

	case MS_NULLCHAR: {
		return "null char pointer";
	}
	case MS_NULLARRAY: {
		return "null mutableString array";
	}
	case MS_NULLCOUNT: {
		return "null counter";
	}
	case MS_NULLTARGET: {
		return "null target pointer";
	}

	case MS_IDXOUTBOUNDS: {
		return "index out of bounds";
	}
	case MS_SUCCESSFULIDX: {
		return "succeed to use the index";
	}
	case MS_SUCCEEDTOTRUNCATE: {
		return "succeed to truncate the mutableString";
	}
	case MS_SUCCEEDTOFINDCH: {
		return "succeed to find the char you want";
	}
	case MS_WITHOUTCH: {
		return "there is no char you want here";
	}
	case MS_INVALIDSTART: {
		return "invalid start index";
	}
	case MS_INVALIDEND: {
		return "invalid end index";
	}
	case MS_INVALIDIDX: {
		return "invalid index";
	}
	case MS_INVALIDREPLACEMENT: {
		return "what you want to replace with is invalid";
	}
	case MS_LENOUTOFTYPEMAX: {
		return "length out of type max";
	}
	case MS_INVALIDSRC: {
		return "invalid source pointer";
	}
	case MS_INVALIDCHFORCHANGE: {
		return "invalid char for change";
	}

	case MS_SUCCESSFULOP: {
		return "successful operation";
	}
	case MS_PARTIALSPLIT: {
		return "mutableString partial split";
	}
	case MS_NOTARGETMATCHED: {
		return "target not matched";
	}
	default: {
		return "Unknown status";
	}
	}
}

int jcms_appendWithInst(const mutableString ms, const MutableString* const src) {
	int tempStatus = jcms_getStatus(ms);
	if (tempStatus != MS_AVAILABLE)return tempStatus;
	tempStatus = jcms_getStatus(src);
	if (tempStatus != MS_AVAILABLE)return tempStatus;
	if (!src->len)return MS_SUCCESSFULOP;
	if (!ms->len)return jcms_setContent(ms, src->buffer);
	// 如果都不为空

	return appendCore(ms, &src->buffer, src->len);
}

int jcms_appendWithCh(const mutableString ms, const char* const ch) {
	int tempStatus = jcms_getStatus(ms);
	if (tempStatus != MS_AVAILABLE)return tempStatus;
	if (!ch)return MS_NULLCHAR;
	uintptr_t bufStart = (uintptr_t)ms->buffer;
	uintptr_t bufEnd = bufStart + ms->len;   // buffer + len 是有效地址，不溢出
	uintptr_t srcPtr = (uintptr_t)ch;
	if (srcPtr >= bufStart && srcPtr <= bufEnd) return MS_INVALIDSRC;
	if (ch[0] == '\0')return MS_SUCCESSFULOP;
	if (!ms->len)return jcms_setContent(ms, ch);
	char* localCh = (char*)ch;

	return appendCore(ms, &localCh, strlen(ch));
}

int jcms_reverse(mutableString ms) {
	if (!ms || !ms->buffer || ms->buffer[0]=='\0')return MS_SUCCESSFULOP;

	for (size_t i = 0; i < ms->len / 2; i++) {
		char c = ms->buffer[i];
		ms->buffer[i] = ms->buffer[ms->len - 1 - i];
		ms->buffer[ms->len - 1 - i] = c;
	}

	return MS_SUCCESSFULOP;
}

int jcms_split(const MutableString* const ms, char delimiter,
	mutableString** outArray, size_t* count,
	bool removeEmptyEntries) {
	if (!outArray) return MS_NULLARRAY;
	if (!count)    return MS_NULLCOUNT;

	*outArray = NULL;
	*count = 0;

	if (!ms)         return MS_NULL;
	if (!ms->buffer) return MS_NULLBUFFER;

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

	if (wholeCount == 0) return MS_SUCCESSFULOP;

	mutableString* arr = (mutableString*)malloc(sizeof(mutableString) * wholeCount);
	if (!arr) return MS_FAILEDTOALLOC;

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
			mutableString item = pkgMS(p, segLen);
			if (!item || item->status != MS_AVAILABLE) hasNull = true;
			arr[i++] = item;
		}

		if (!found) break;
		p = found + 1;
	}

	*outArray = arr;
	*count = wholeCount;
	return hasNull ? MS_PARTIALSPLIT : MS_SUCCESSFULOP;
}

int jcms_trim(const mutableString ms) {
	if (!ms || !ms->buffer || !ms->len)return MS_SUCCESSFULOP;

	size_t len = ms->len;
	size_t start = 0;
	while (start < len && isspace((unsigned char)ms->buffer[start])) {
		start++;
	}

	if (start == len) {
		ms->buffer[0] = '\0';
		ms->len = 0;
		return MS_SUCCESSFULOP;
	}

	size_t end = len - 1;
	while (end > start && isspace((unsigned char)ms->buffer[end])) {
		end--;
	}

	size_t newLen = end - start + 1;
	if (start == 0 && newLen == len)return MS_SUCCESSFULOP;

	if (start > 0) {
		memmove(ms->buffer, ms->buffer + start, newLen);
	}
	ms->buffer[newLen] = '\0';
	ms->len = newLen;
	
	return MS_SUCCESSFULOP;
}

int jcms_trimFront(const mutableString ms) {
	if (!ms || !ms->buffer || !ms->len)return MS_SUCCESSFULOP;

	size_t len = ms->len;
	size_t start = 0;
	while (start < len && isspace((unsigned char)ms->buffer[start])) {
		start++;
	}

	if (start == 0)return MS_SUCCESSFULOP;
	if (start == len) {
		ms->buffer[0] = '\0';
		ms->len = 0;
		return MS_SUCCESSFULOP;
	}

	size_t newLen = len - start;
	
	memmove(ms->buffer, ms->buffer + start, newLen);

	ms->buffer[newLen] = '\0';
	ms->len = newLen;

	return MS_SUCCESSFULOP;
}

int jcms_trimBack(const mutableString ms) {
	if (!ms || !ms->buffer || !ms->len)return MS_SUCCESSFULOP;

	size_t newLen = ms->len;
	while (newLen > 0 && isspace((unsigned char)ms->buffer[newLen - 1]))newLen--;

	if (newLen == ms->len)return MS_SUCCESSFULOP;

	ms->buffer[newLen] = '\0';
	ms->len = newLen;

	return MS_SUCCESSFULOP;
}

mutableString jcms_clone(const MutableString* const ms) {
	if (!ms)return NULL;
	return jcms_newFrom(ms->buffer);
}

int jcms_charAt(const MutableString* const ms, size_t index, char* result) {
	if (!result)return MS_NULLCHAR;
	if (!ms)return MS_NULL;
	if (!ms->buffer)return MS_NULLBUFFER;
	if (index >= ms->len)return MS_IDXOUTBOUNDS;

	*result = ms->buffer[index];

	return MS_SUCCESSFULIDX;
}

int jcms_indexOfFront(const MutableString* const ms, char ch, size_t* result) {
	if (!result)return MS_NULLINT;
	if (!ms)return MS_NULL;
	if (!ms->buffer)return MS_NULLBUFFER;

	const char* found = strchr(ms->buffer, ch);
	if (found) {
		*result = (size_t)(found - ms->buffer);
		return MS_SUCCEEDTOFINDCH;
	}
	return MS_WITHOUTCH;
}

int jcms_indexOfBack(const MutableString* const ms, char ch, size_t* result) {
	if (!result)return MS_NULLINT;
	if (!ms)return MS_NULL;
	if (!ms->buffer)return MS_NULLBUFFER;
	for (ptrdiff_t i = (ptrdiff_t)ms->len - 1; i >= 0; --i) {
		if (ms->buffer[i] == ch) {
			*result = (size_t)i;
			return MS_SUCCEEDTOFINDCH;
		}
	}
	return MS_WITHOUTCH;
}

int jcms_indexOfFrontWithInst(const MutableString* const ms, const MutableString* const sub, size_t* result) {
	if (!result)return MS_NULLRECEIVER;
	if (!ms || !sub)return MS_NULL;
	if (!ms->buffer || !sub->buffer)return MS_NULLBUFFER;
	if (sub->buffer[0] == '\0') {
		*result = ms->len;
		return MS_SUCCESSFULOP;
	}
	const char* found = strstr(ms->buffer, sub->buffer);
	if (!found)return MS_WITHOUTCH;
	*result = (size_t)(found - ms->buffer);
	return MS_SUCCESSFULOP;
}

int jcms_indexOfFrontWithCh(const MutableString* const ms, const char* const ch, size_t* result) {
	if (!result)return MS_NULLRECEIVER;
	if (!ms)return MS_NULL;
	if (!ms->buffer || !ch)return MS_NULLBUFFER;
	if (ch[0] == '\0') {
		*result = ms->len;
		return MS_SUCCESSFULOP;
	}
	const char* found = strstr(ms->buffer, ch);
	if (!found)return MS_WITHOUTCH;
	*result = (size_t)(found - ms->buffer);
	return MS_SUCCESSFULOP;
}

mutableString jcms_subInLengthFrontWithInst(const MutableString* const ms, size_t start, size_t length) {
	if (!ms)return NULL;
	if (!ms->buffer)return jcms_newFrom(NULL);
	if (!ms->len || !length)return jcms_newFrom("");

	size_t len = ms->len;
	// 起始点超限
	if (start >= len) {
		mutableString temp = jcms_newFrom("");
		if (temp && temp->status == MS_AVAILABLE)temp->status = MS_INVALIDSTART;
		return temp;
	}

	// 长度超限就整合长度
	if (length > len - start)length = len - start;

	return pkgMS(ms->buffer + start, length);
}

mutableString subMSInLengthBack(const mutableString* const str, size_t end, size_t length) {
	if (!str)return NULL;
	if (!str->buffer)return newMSFrom(NULL);
	if (str->buffer[0] == '\0' || !length)return newMSFrom("");

	size_t len = getMSLength(str);
	if (end >= len) {
		mutableString temp = newMSFrom("");
		if (temp)temp->status = MS_INVALIDEND;
		return temp;
	}

	if ((ptrdiff_t)end + 1 - (ptrdiff_t)length < 0)length = end + 1;

	mutableString temp = (mutableString)malloc(sizeof(mutableString));
	if (!temp)return NULL;
	temp->buffer = (char*)malloc(length + 1);
	if (!temp->buffer) {
		temp->status = MS_NULLBUFFER;
		temp->len = 0;
		temp->cap = 0;
		return temp;
	}
	memcpy(temp->buffer, str->buffer + (end - length + 1), length);
	temp->buffer[length] = '\0';
	temp->status = MS_AVAILABLE;
	temp->len = length;
	temp->cap = length + 1;
	return temp;
}

mutableString subMSInRange(const mutableString* const str, size_t start, size_t end) {
	if (!str)return NULL;
	if (!str->buffer)return newMSFrom(NULL);
	if (str->buffer[0] == '\0' || start == end)return newMSFrom("");

	if (start > end) {
		mutableString temp = newMSFrom("");
		if (temp)temp->status = MS_INVALIDIDX;
		return temp;
	}

	size_t len = getMSLength(str);
	if (start >= len || end > len) {
		mutableString temp = newMSFrom("");
		if (temp)temp->status = MS_IDXOUTBOUNDS;
		return temp;
	}

	size_t length = end - start;
	mutableString temp = (mutableString)malloc(sizeof(mutableString));
	if (!temp)return NULL;
	temp->buffer = (char*)malloc(length + 1);
	if (!temp->buffer) {
		temp->status = MS_NULLBUFFER;
		temp->len = 0;
		temp->cap = 0;
		return temp;
	}
	memcpy(temp->buffer, str->buffer + start, length);
	temp->buffer[length] = '\0';
	temp->status = MS_AVAILABLE;
	temp->len = length;
	temp->cap = length + 1;
	return temp;
}

mutableString jcms_subInLengthFrontWithCh(const char* const ch, size_t start, size_t length) {
	if (!ch)return jcms_newFrom(NULL);
	if (ch[0] == '\0' || !length)return jcms_newFrom("");

	size_t len = strlen(ch);
	if (start >= len) {
		mutableString temp = jcms_newFrom("");
		if (temp && temp->status == MS_AVAILABLE)temp->status = MS_INVALIDSTART;
		return temp;
	}

	if (length > len - start)length = len - start;

	return pkgMS(ch + start, length);
}

mutableString subMSInLengthBackFrom(const char* const ch, size_t end, size_t length) {
	if (!ch)return newMSFrom(NULL);
	if (ch[0] == '\0' || !length)return newMSFrom("");

	size_t len = strlen(ch);
	if (end >= len) {
		mutableString temp = newMSFrom("");
		if (temp)temp->status = MS_INVALIDEND;
		return temp;
	}

	if ((ptrdiff_t)end + 1 - (ptrdiff_t)length < 0)length = end + 1;

	mutableString temp = (mutableString)malloc(sizeof(mutableString));
	if (!temp)return NULL;
	temp->buffer = (char*)malloc(length + 1);
	if (!temp->buffer) {
		temp->status = MS_NULLBUFFER;
		temp->len = 0;
		temp->cap = 0;
		return temp;
	}
	memcpy(temp->buffer, ch + (end - length + 1), length);
	temp->buffer[length] = '\0';
	temp->status = MS_AVAILABLE;
	temp->len = length;
	temp->cap = length + 1;
	return temp;
}

mutableString subMSInRangeFrom(const char* const ch, size_t start, size_t end) {
	if (!ch)return newMSFrom(NULL);
	if (ch[0] == '\0' || start == end)return newMSFrom("");

	if (start > end) {
		mutableString temp = newMSFrom("");
		if (temp)temp->status = MS_INVALIDIDX;
		return temp;
	}

	size_t len = strlen(ch);
	if (start >= len || end > len) {
		mutableString temp = newMSFrom("");
		if (temp)temp->status = MS_IDXOUTBOUNDS;
		return temp;
	}

	size_t length = end - start;
	mutableString temp = (mutableString)malloc(sizeof(mutableString));
	if (!temp)return NULL;
	temp->buffer = (char*)malloc(length + 1);
	if (!temp->buffer) {
		temp->status = MS_NULLBUFFER;
		temp->len = 0;
		temp->cap = 0;
		return temp;
	}
	memcpy(temp->buffer, ch + start, length);
	temp->buffer[length] = '\0';
	temp->status = MS_AVAILABLE;
	temp->len = length;
	temp->cap = length + 1;
	return temp;
}

int replaceMSFirst(mutableString* str, const char* const target, const mutableString* const replacement) {
	if (!str)return MS_NULL;
	if (!replacement)return MS_INVALIDREPLACEMENT;
	return replaceMSFirstFrom(str, target, replacement->buffer);
}

int replaceMSFirstFrom(mutableString* str, const char* const target, const char* const replacement) {
	if (!str)return MS_NULL;

	if (!str->buffer)return MS_NULLBUFFER;
	if (!target)return MS_NULLTARGET;
	if (!replacement)return MS_INVALIDREPLACEMENT;

	if (str->buffer[0] == '\0' && target[0] == '\0')return setMSContent(str, replacement);
	if (str->buffer[0] == '\0')return MS_NOTARGETMATCHED;
	if (target[0] == '\0')return appendMSFrom(str, replacement);

	char* indexChar = strstr(str->buffer, target);
	if (!indexChar)return MS_NOTARGETMATCHED;

	size_t index = (size_t)(indexChar - str->buffer);
	size_t targetLen = strlen(target);
	size_t replaceLen = strlen(replacement);
	size_t strLen = getMSLength(str);

	if (targetLen == replaceLen) {
		memmove(str->buffer + index, replacement, replaceLen);
		return MS_SUCCESSFULOP;
	}

	size_t preLen = index;
	size_t sufLen = strLen - preLen - targetLen;
	size_t newLen = preLen + replaceLen + sufLen;
	size_t needed = newLen + 1;

	if (str->cap < needed) {
		char* newBuffer = (char*)realloc(str->buffer, needed);
		if (!newBuffer)return MS_FAILEDTOREALLOC;
		str->buffer = newBuffer;
		str->cap = needed;
	}

	memmove(str->buffer + index + replaceLen, str->buffer + index + targetLen, sufLen + 1);
	memcpy(str->buffer + index, replacement, replaceLen);

	str->len = newLen;
	return MS_SUCCESSFULOP;
}

int replaceMSAll(mutableString* str, const char* const target, const mutableString* const replacement) {
	if (!str)return MS_NULL;
	if (!replacement)return MS_INVALIDREPLACEMENT;
	return replaceMSAllFrom(str, target, replacement->buffer);
}

int replaceMSAllFrom(mutableString* str, const char* const target, const char* const replacement) {
	if (!str)return MS_NULL;

	if (!str->buffer)return MS_NULLBUFFER;
	if (!target)return MS_NULLTARGET;
	if (!replacement)return MS_INVALIDREPLACEMENT;

	if (str->buffer[0] == '\0' && target[0] == '\0')return setMSContent(str, replacement);
	if (str->buffer[0] == '\0')return MS_NOTARGETMATCHED;
	if (target[0] == '\0')return appendMSFrom(str, replacement);

	size_t strLen = str->len;
	size_t targetLen = strlen(target);
	size_t replaceLen = strlen(replacement);

	// 第一遍：扫描所有匹配位置
	size_t capacity = 16;
	size_t* positions = (size_t*)malloc(capacity * sizeof(size_t));
	if (!positions)return MS_FAILEDTOALLOC;

	size_t count = 0;
	size_t scan = 0;
	while (scan + targetLen <= strLen) {
		if (memcmp(str->buffer + scan, target, targetLen) == 0) {
			if (count == capacity) {
				capacity *= 2;
				size_t* newPositions = (size_t*)realloc(positions, capacity * sizeof(size_t));
				if (!newPositions) {
					free(positions);
					return MS_FAILEDTOREALLOC;
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
		return MS_NOTARGETMATCHED;
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
	if (newLen + 1 > str->cap) {
		char* newBuffer = (char*)realloc(str->buffer, newLen + 1);
		if (!newBuffer) {
			free(positions);
			return MS_FAILEDTOREALLOC;
		}
		str->buffer = newBuffer;
		str->cap = newLen + 1;
	}

	// 从后往前替换
	size_t currentLen = strLen;
	for (size_t k = count; k > 0; k--) {
		size_t pos = positions[k - 1];
		size_t suffixStart = pos + targetLen;
		size_t suffixLen = currentLen - suffixStart;

		// 先移后缀，再写 replacement
		memmove(str->buffer + pos + replaceLen,
			str->buffer + suffixStart,
			suffixLen + 1);
		memcpy(str->buffer + pos, replacement, replaceLen);

		if (replaceLen >= targetLen) {
			currentLen += replaceLen - targetLen;
		}
		else {
			currentLen -= targetLen - replaceLen;
		}
	}

	free(positions);
	str->len = newLen;
	str->buffer[newLen] = '\0';
	return MS_SUCCESSFULOP;
}

bool MSStartWith(const mutableString* const str, const char* const prefix) {
	if (!str || !str->buffer)return false;

	if (!prefix || prefix[0] == '\0')return true;

	size_t strLen = getMSLength(str);
	size_t preLen = strlen(prefix);
	if (strLen < preLen)return false;

	return strncmp(str->buffer, prefix, preLen) == 0;
}

bool MSEndWith(const mutableString* const str, const char* const suffix) {
	if (!str || !str->buffer)return false;

	if (!suffix || suffix[0] == '\0')return true;

	size_t strLen = getMSLength(str);
	size_t sufLen = strlen(suffix);
	if (strLen < sufLen)return false;

	return strncmp(str->buffer + (strLen - sufLen), suffix, sufLen) == 0;
}

bool MSStartWithString(const mutableString* const str, const mutableString* const prefix) {
	if (!prefix)return true;
	return MSStartWith(str, prefix->buffer);
}

bool MSEndWithString(const mutableString* const str, const mutableString* const suffix) {
	if (!suffix)return true;
	return MSEndWith(str, suffix->buffer);
}

bool MSIsEmpty(const mutableString* const str) {
	return !str || !str->buffer || str->buffer[0] == '\0' ? true : false;
}

bool MSIMSlank(const mutableString* const str) {
	if (!str || !str->buffer)return true;

	for (const char* p = str->buffer; *p; p++) {
		if (!isspace((unsigned char)*p)) {
			return false;
		}
	}

	return true;
}

bool equalsMS(const mutableString* const str1, const mutableString* const str2) {
	if (str1 == str2)return true;
	if (!str1 || !str2)return false;

	if ((!str1->buffer || str1->buffer[0] == '\0') && (!str2->buffer || str2->buffer[0] == '\0'))return true;
	if ((!str1->buffer || str1->buffer[0] == '\0') || (!str2->buffer || str2->buffer[0] == '\0'))return false;

	return strcmp(str1->buffer, str2->buffer) == 0;
}

bool equalsMSFrom(const mutableString* const str, const char* const ch) {
	if ((!str || !str->buffer) && !ch)return true;
	if (((!str || !str->buffer) && ch) || ((str && str->buffer) && !ch))return false;
	return strcmp(str->buffer, ch) == 0;
}

bool containsMS(const mutableString* const str1, const mutableString* const str2) {
	if (!str2)return true;
	if (!str2->buffer)return str1 != NULL;
	if (str2->buffer[0] == '\0') {
		if (!str1 || !str1->buffer)return false;
		return true;
	}

	if (!str1 || !str1->buffer || str1->buffer[0] == '\0')return false;

	return strstr(str1->buffer, str2->buffer) != NULL;
}

bool containsMSFrom(const mutableString* const str, const char* const ch) {
	if (!ch)return str != NULL;
	if (ch[0] == '\0') {
		if (!str || !str->buffer)return false;
		return true;
	}

	if (!str || !str->buffer || str->buffer[0] == '\0')return false;

	return strstr(str->buffer, ch) != NULL;
}


char* MSToArray(const mutableString* const str) {
	if (!str || !str->buffer)return NULL;
	if (str->buffer[0] == '\0') {
		char* temp = (char*)calloc(1, sizeof(char));
		if (!temp)return NULL;
		temp[0] = '\0';
		return temp;
	}

	size_t len = getMSLength(str) + 1;
	char* content = (char*)malloc(len);
	return content ? memcpy(content, str->buffer, len) : NULL;
}

int MSToInt(const mutableString* const str, int* result) {
	if (!str || !str->buffer || str->buffer[0] == '\0')return MS_EMPTY;
	if (getMSStatus(str))return MS_WRONGSTATUS;
	if (!result)return MS_NULLINT;

	const char* p = str->buffer;
	size_t len = strlen(str->buffer);

	size_t start = 0;
	while (start < len && isspace((unsigned char)p[start]))start++;

	size_t end = len;
	while (end > start && isspace((unsigned char)p[end - 1]))end--;

	if (start == end) {
		return MS_BLANK;
	}

	bool isNegative = false;
	int tempNum = 0;
	int numCount = 0;
	size_t i = start;

	if (p[i] == '-' || p[i] == '+') {
		if (p[i] == '-')isNegative = true;
		i++;
	}

	for (; i < end; i++) {
		if (p[i] >= '0' && p[i] <= '9') {
			int digit = p[i] - '0';

			if (tempNum < (INT_MIN + digit) / 10) {
				return MS_OVERFLOWINT;
			}

			tempNum = 10 * tempNum - digit;
			numCount++;
		}
		else {
			return MS_NOTINT;
		}
	}

	if (numCount) {
		if (isNegative)*result = tempNum;
		else *result = -tempNum;
		return MS_SUCCEEDTOINT;
	}
	else {
		return MS_NOTINT;
	}
}

int MSToLong(const mutableString* const str, long* result) {
	if (!str || !str->buffer || str->buffer[0] == '\0')return MS_EMPTY;
	if (getMSStatus(str))return MS_WRONGSTATUS;
	if (!result)return MS_NULLLONG;

	const char* p = str->buffer;
	size_t len = strlen(str->buffer);

	size_t start = 0;
	while (start < len && isspace((unsigned char)p[start]))start++;

	size_t end = len;
	while (end > start && isspace((unsigned char)p[end - 1]))end--;

	if (start == end) {
		return MS_BLANK;
	}

	bool isNegative = false;
	long tempNum = 0;
	int numCount = 0;
	size_t i = start;

	if (p[i] == '-' || p[i] == '+') {
		if (p[i] == '-')isNegative = true;
		i++;
	}

	for (; i < end; i++) {
		if (p[i] >= '0' && p[i] <= '9') {
			long digit = p[i] - '0';

			if (tempNum < (LONG_MIN + digit) / 10) {
				return MS_OVERFLOWLONG;
			}

			tempNum = 10 * tempNum - digit;
			numCount++;
		}
		else {
			return MS_NOTLONG;
		}
	}

	if (numCount) {
		if (isNegative)*result = tempNum;
		else *result = -tempNum;
		return MS_SUCCEEDTOLONG;
	}
	else {
		return MS_NOTLONG;
	}
}

int MSToLongLong(const mutableString* const str, long long* result) {
	if (!str || !str->buffer || str->buffer[0] == '\0')return MS_EMPTY;
	if (getMSStatus(str))return MS_WRONGSTATUS;
	if (!result)return MS_NULLLONGLONG;

	const char* p = str->buffer;
	size_t len = strlen(str->buffer);

	size_t start = 0;
	while (start < len && isspace((unsigned char)p[start]))start++;

	size_t end = len;
	while (end > start && isspace((unsigned char)p[end - 1]))end--;

	if (start == end) {
		return MS_BLANK;
	}

	bool isNegative = false;
	long long tempNum = 0;
	int numCount = 0;
	size_t i = start;

	if (p[i] == '-' || p[i] == '+') {
		if (p[i] == '-')isNegative = true;
		i++;
	}

	for (; i < end; i++) {
		if (p[i] >= '0' && p[i] <= '9') {
			long long digit = p[i] - '0';

			if (tempNum < (LLONG_MIN + digit) / 10) {
				return MS_OVERFLOWLONGLONG;
			}

			tempNum = 10 * tempNum - digit;
			numCount++;
		}
		else {
			return MS_NOTLONGLONG;
		}
	}

	if (numCount) {
		if (isNegative)*result = tempNum;
		else *result = -tempNum;
		return MS_SUCCEEDTOLONGLONG;
	}
	else {
		return MS_NOTLONGLONG;
	}
}

int MSToFloat(const mutableString* const str, float* result) {
	if (!str || !str->buffer || str->buffer[0] == '\0')return MS_EMPTY;
	if (getMSStatus(str))return MS_WRONGSTATUS;
	if (!result)return MS_NULLFLOAT;

	const char* p = str->buffer;
	size_t len = strlen(str->buffer);

	size_t start = 0;
	while (start < len && isspace((unsigned char)p[start]))start++;

	size_t end = len;
	while (end > start && isspace((unsigned char)p[end - 1]))end--;

	if (start == end) {
		return MS_BLANK;
	}

	size_t i = start;
	bool seenDigit = false;
	bool seenDot = false;

	if (p[i] == '-' || p[i] == '+') i++;

	if (i < end && p[i] == '.') {
		return MS_NOTFLOAT;   // 不允许 ".5" 这种没有整数部分的格式
	}

	for (; i < end; i++) {
		if (p[i] >= '0' && p[i] <= '9') {
			seenDigit = true;
		}
		else if (p[i] == '.' && !seenDot) {
			seenDot = true;
		}
		else {
			return MS_NOTFLOAT;
		}
	}

	if (!seenDigit) return MS_NOTFLOAT;

	// 调用标准库转换，从 p + start 开始
	char* endptr = NULL;
	errno = 0;
	float value = strtof(p + start, &endptr);

	// 确认解析到有效片段末尾
	if (endptr != p + end) {
		return MS_NOTFLOAT;
	}

	if (errno == ERANGE) {
		return MS_OVERFLOWFLOAT;
	}

	*result = value;
	return MS_SUCCEEDTOFLOAT;
}

int MSToDouble(const mutableString* const str, double* result) {
	if (!str || !str->buffer || str->buffer[0] == '\0')return MS_EMPTY;
	if (getMSStatus(str))return MS_WRONGSTATUS;
	if (!result)return MS_NULLDOUBLE;

	const char* p = str->buffer;
	size_t len = strlen(str->buffer);

	size_t start = 0;
	while (start < len && isspace((unsigned char)p[start]))start++;

	size_t end = len;
	while (end > start && isspace((unsigned char)p[end - 1]))end--;

	if (start == end) {
		return MS_BLANK;
	}

	size_t i = start;
	bool seenDigit = false;
	bool seenDot = false;

	if (p[i] == '-' || p[i] == '+') i++;

	if (i < end && p[i] == '.') {
		return MS_NOTDOUBLE;   // 不允许 ".5" 这种没有整数部分的格式
	}

	for (; i < end; i++) {
		if (p[i] >= '0' && p[i] <= '9') {
			seenDigit = true;
		}
		else if (p[i] == '.' && !seenDot) {
			seenDot = true;
		}
		else {
			return MS_NOTDOUBLE;
		}
	}

	if (!seenDigit) return MS_NOTDOUBLE;

	// 调用标准库转换，从 p + start 开始
	char* endptr = NULL;
	errno = 0;
	double value = strtod(p + start, &endptr);

	// 确认解析到有效片段末尾
	if (endptr != p + end) {
		return MS_NOTDOUBLE;
	}

	if (errno == ERANGE) {
		return MS_OVERFLOWDOUBLE;
	}

	*result = value;
	return MS_SUCCEEDTODOUBLE;
}

int MSToLowerCase(mutableString* str) {
	if (!str)return MS_NULL;
	if (!str->buffer)return MS_NULLBUFFER;

	for (size_t i = 0; i < str->len; i++) {
		str->buffer[i] = (char)tolower((unsigned char)str->buffer[i]);
	}

	return MS_SUCCESSFULOP;
}

int MSToUpperCase(mutableString* str) {
	if (!str)return MS_NULL;
	if (!str->buffer)return MS_NULLBUFFER;

	for (size_t i = 0; i < str->len; i++) {
		str->buffer[i] = (char)toupper((unsigned char)str->buffer[i]);
	}

	return MS_SUCCESSFULOP;
}

string jcms_toString(const MutableString* const ms) {
	if (!ms)return jcstr_newFrom(NULL);
	return jcstr_newFrom(ms->buffer);
}