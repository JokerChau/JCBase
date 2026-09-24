#include "JC_StringBuilder.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>

typedef struct StringBuilder {
	char* buffer;
	int status;
	size_t len;
	size_t cap;
}StringBuilder;

typedef enum {
	INITIAL_SB_SIZE = 200,
	GROWTH = 2,
}Sizes;

// StringBuilder构造器
stringBuilder newSb() {
	// 为str实例申请内存
	stringBuilder str = (stringBuilder)malloc(sizeof(StringBuilder));
	if (!str)return NULL;

	// 为str的buffer成员申请内存
	str->buffer = (char*)calloc(INITIAL_SB_SIZE, sizeof(char));
	if (!str->buffer) {
		str->buffer = NULL;
		str->status = SB_NULLBUFFER;
		str->len = 0;
		str->cap = 0;
	}
	else {
		str->len = 0;
		str->cap = INITIAL_SB_SIZE;
		str->status = SB_AVAILABLE;
	}

	return str;
}

// StringBuilder构造器
stringBuilder newSbFrom(const char* const ch) {
	bool isNull = ch == NULL;

	stringBuilder sb = (stringBuilder)malloc(sizeof(StringBuilder));
	if (!sb)return NULL;

	if (isNull) {
		sb->buffer = NULL;
		sb->status = SB_NULLBUFFER;
		sb->len = 0;
		sb->cap = 0;
		return sb;
	}

	// 获取char*长度
	size_t len = strlen(ch);
	// 为buffer申请内存
	sb->buffer = (char*)malloc(len + 1);
	if (!sb->buffer) {
		sb->buffer = NULL;
		sb->status = SB_NULLBUFFER;
		sb->len = 0;
		sb->cap = 0;
	}
	// 若为buffer申请内存成功则更新状态并将长度设置为已知的len
	else {
		memcpy(sb->buffer, ch, len + 1);
		sb->len = len;
		sb->cap = len + 1;
		sb->status = SB_AVAILABLE;
	}

	return sb;
}

void deleteSb(stringBuilder sb) {
	if (!sb)return;
	if (sb->buffer) {
		free(sb->buffer);
		sb->buffer = NULL;
	}
	free(sb);
}

int getSbStatus(const StringBuilder* const sb) {
	if (!sb)return SB_NULL;
	return sb->status;
}

size_t getSbLength(const StringBuilder* const sb) {
	if (!sb)return 0;
	return sb->len;
}

const char* getSbBuffer(const StringBuilder* const sb) {
	return sb->buffer;
}

const char* getSbContent(const StringBuilder* const sb) {
	if (!sb || !sb->buffer)return NULL;
	char* temp = (char*)malloc(sb->len + 1);
	if (!temp)return NULL;
	memcpy(temp, sb->buffer, sb->len + 1);
	temp[sb->len] = '\0';
	return temp;
}

int setSbContent(StringBuilder* const str, const char* const content) {
	if (!str)return SB_NULL;
	if (str->status != SB_AVAILABLE)return SB_WRONGSTATUS;

	// 内容为 NULL → 清空，不释放 buffer
	if (!content) {
		if (!str->buffer) {
			str->buffer = (char*)malloc(1);
			if (!str->buffer) {
				str->status = SB_NULLBUFFER;
				str->len = 0;
				str->cap = 0;
				return SB_NULLBUFFER;
			}
			str->cap = 1;
		}
		str->buffer[0] = '\0';
		str->len = 0;
		str->status = SB_AVAILABLE;
		return SB_SUCCESSFULOP;
	}

	size_t len = strlen(content);
	size_t needed = len + 1;

	// 容量够，直接拷贝，零分配
	if (str->buffer && str->cap >= needed) {
		memcpy(str->buffer, content, needed);
		str->len = len;
		str->status = SB_AVAILABLE;
		return SB_SUCCESSFULOP;
	}

	// 容量不够，扩容
	size_t newCap = needed;
	// 可选：至少保留 INITIAL_SB_SIZE，避免频繁小扩容
	// if (newCap < INITIAL_SB_SIZE)newCap = INITIAL_SB_SIZE;

	char* newBuffer = (char*)realloc(str->buffer, newCap);
	if (!newBuffer)return SB_FAILEDTOREALLOC;

	str->buffer = newBuffer;
	str->cap = newCap;
	memcpy(str->buffer, content, needed);
	str->len = len;
	str->status = SB_AVAILABLE;
	return SB_SUCCESSFULOP;
}

int changeAt(stringBuilder str, size_t index, char ch) {
	if (!str) {
		return SB_NULL;
	}
	if (!str->buffer) {
		return SB_NULLBUFFER;
	}
	if (index >= getSbLength(str))return SB_IDXOUTBOUNDS;

	str->buffer[index] = ch;
	if (ch == '\0') {
		str->len = index;
		return SB_SUCCEEDTOTRUNCATE;
	}

	return SB_SUCCESSFULIDX;
}

const char* sbStatusToCharArray(const int status) {
	switch (status) {
	case SB_NULL: {
		return "null stringbuilder";
	}
	case SB_AVAILABLE: {
		return "available stringbuilder";
	}
	case SB_NULLBUFFER: {
		return "null buffer";
	}
	case SB_EMPTY: {
		return "empty stringbuilder";
	}
	case SB_BLANK: {
		return "blank stringbuilder";
	}

	case SB_FAILEDTOCOMBINE: {
		return "failed to combine";
	}
	case SB_WRONGSTATUS: {
		return "wrong stringbuilder status";
	}
	case SB_FAILEDTOREALLOC: {
		return "failed to realloc";
	}
	case SB_FAILEDTOALLOC: {
		return "failed to alloc";
	}

	case SB_NULLINT: {
		return "null int pointer";
	}
	case SB_OVERFLOWINT: {
		return "overflow int range";
	}
	case SB_NOTINT: {
		return "not int";
	}
	case SB_SUCCEEDTOINT: {
		return "succeed to change stringbuilder into int";
	}

	case SB_NULLLONG: {
		return "null long int pointer";
	}
	case SB_OVERFLOWLONG: {
		return "overflow long int range";
	}
	case SB_NOTLONG: {
		return "not long int";
	}
	case SB_SUCCEEDTOLONG: {
		return "succeed to change stringbuilder into long int";
	}

	case SB_NULLLONGLONG: {
		return "null long long int pointer";
	}
	case SB_OVERFLOWLONGLONG: {
		return "overflow long long int range";
	}
	case SB_NOTLONGLONG: {
		return "not long long int";
	}
	case SB_SUCCEEDTOLONGLONG: {
		return "succeed to change stringbuilder into long long int";
	}

	case SB_NULLFLOAT: {
		return "null float pointer";
	}
	case SB_OVERFLOWFLOAT: {
		return "overflow float range";
	}
	case SB_NOTFLOAT: {
		return "not float";
	}
	case SB_SUCCEEDTOFLOAT: {
		return "succeed to change stringbuilder into float";
	}

	case SB_NULLDOUBLE: {
		return "null double pointer";
	}
	case SB_OVERFLOWDOUBLE: {
		return "overflow double range";
	}
	case SB_NOTDOUBLE: {
		return "not double";
	}
	case SB_SUCCEEDTODOUBLE: {
		return "succeed to change stringbuilder into double";
	}

	case SB_NULLCHAR: {
		return "null char pointer";
	}
	case SB_NULLARRAY: {
		return "null stringBuilder array";
	}
	case SB_NULLCOUNT: {
		return "null counter";
	}
	case SB_NULLTARGET: {
		return "null target pointer";
	}

	case SB_IDXOUTBOUNDS: {
		return "index out of bounds";
	}
	case SB_SUCCESSFULIDX: {
		return "succeed to use the index";
	}
	case SB_SUCCEEDTOTRUNCATE: {
		return "succeed to truncate the stringbuilder";
	}
	case SB_SUCCEEDTOFINDCH: {
		return "succeed to find the char you want";
	}
	case SB_WITHOUTCH: {
		return "there is no char you want here";
	}
	case SB_INVALIDSTART: {
		return "invalid start index";
	}
	case SB_INVALIDEND: {
		return "invalid end index";
	}
	case SB_INVALIDIDX: {
		return "invalid index";
	}
	case SB_INVALIDREPLACEMENT: {
		return "what you want to replace with is invalid";
	}

	case SB_SUCCESSFULOP: {
		return "successful operation";
	}
	case SB_PARTIALSPLIT: {
		return "stringBuilder partial split";
	}
	case SB_NOTARGETMATCHED: {
		return "target not matched";
	}
	default: {
		return "Unknown status";
	}
	}
}

int appendSb(StringBuilder* sb, const StringBuilder* const src) {
	int tempStatus = getSbStatus(sb);
	if (tempStatus != SB_AVAILABLE)return tempStatus;
	if (!src || !src->buffer || src->buffer[0] == '\0')return SB_SUCCESSFULOP;
	if (sb->buffer[0] == '\0')return setSbContent(sb, src->buffer);
	// 如果都不为空

	size_t srcLen = strlen(src->buffer);
	size_t sbLen = strlen(sb->buffer);

	BEGIN:
	// 如果cap够则不扩容
	if (sb->cap >= srcLen + sbLen + 1) {
		memmove(sb->buffer + sbLen, src->buffer, srcLen + 1);
		sb->len = sbLen + srcLen;
		return SB_SUCCESSFULOP;
	}
	// 如果cap不够则扩容
	else {
		if (sb->cap > SIZE_MAX / GROWTH) return SB_FAILEDTOREALLOC;
		char* newBuffer = (char*)realloc(sb->buffer, sb->cap * GROWTH);
		if (!newBuffer)return SB_FAILEDTOREALLOC;
		sb->buffer = newBuffer;
		sb->cap *= GROWTH;
		goto BEGIN;
	}
}

int appendSbFrom(stringBuilder sb, const char* const ch) {
	int tempStatus = getSbStatus(sb);
	if (tempStatus != SB_AVAILABLE)return tempStatus;
	if (!ch || ch[0] == '\0')return SB_SUCCESSFULOP;
	if (sb->buffer[0] == '\0')return setSbContent(sb, ch);

	size_t srcLen = strlen(ch);
	size_t sbLen = strlen(sb->buffer);

	while (sb->cap < sbLen + srcLen + 1) {
		if (sb->cap > SIZE_MAX / GROWTH) return SB_FAILEDTOREALLOC;
		char* newBuffer = (char*)realloc(sb->buffer, sb->cap * GROWTH);
		if (!newBuffer)return SB_FAILEDTOREALLOC;
		sb->buffer = newBuffer;
		sb->cap *= GROWTH;
	}
	memmove(sb->buffer + sbLen, ch, srcLen + 1);
	sb->len = sbLen + srcLen;
	return SB_SUCCESSFULOP;
}

int reverseSb(stringBuilder sb) {
	if (!sb)return SB_NULL;
	if (!sb->buffer)return SB_NULLBUFFER;
	if (sb->buffer[0] == '\0')return SB_SUCCESSFULOP;

	for (size_t i = 0; i < sb->len / 2; i++) {
		char c = sb->buffer[i];
		sb->buffer[i] = sb->buffer[sb->len - 1 - i];
		sb->buffer[sb->len - 1 - i] = c;
	}

	return SB_SUCCESSFULOP;
}

int splitSb(const StringBuilder* const str, char delimiter, stringBuilder** outArray, size_t* count) {
	if (!outArray)return SB_NULLARRAY;
	// 计数器为空
	if (!count)return SB_NULLCOUNT;

	*outArray = NULL;
	*count = 0;

	// 三种置空判断
	if (!str)return SB_NULL;
	if (!str->buffer)return SB_NULLBUFFER;
	if (str->buffer[0] == '\0') {
		stringBuilder* stringArray = (stringBuilder*)malloc(sizeof(stringBuilder));
		if (!stringArray)return SB_FAILEDTOALLOC;
		stringArray[0] = newSbFrom("");
		if (!stringArray[0]) {
			free(stringArray);
			return SB_FAILEDTOALLOC;
		}
		*count = 1;
		*outArray = stringArray;
		return SB_SUCCESSFULOP;
	}

	size_t len = strlen(str->buffer);

	size_t wholeCount = 1;
	for (size_t i = 0; i < len; i++)if (str->buffer[i] == delimiter)wholeCount++;

	stringBuilder* stringArray = (stringBuilder*)malloc(sizeof(stringBuilder) * wholeCount);
	if (!stringArray) {
		return SB_FAILEDTOALLOC;
	}

	bool hasNull = false;
	size_t j = 0;
	// i表示元素下标
	for (size_t i = 0; i < wholeCount; i++) {
		size_t staPos = j;
		// 在buffer长度内循环
		while (j < len && str->buffer[j] != delimiter) {
			j++;
		}
		size_t tempLen = j - staPos;
		// 分配临时字符数组
		char* temp = (char*)malloc(sizeof(char) * tempLen + 1);
		if (!temp) {
			stringArray[i] = newSbFrom(NULL);
			hasNull = true;
			if (j < len)j++;
			continue;
		}
		// 将buffer的片段内容复制给temp
		memcpy(temp, str->buffer + staPos, tempLen);
		temp[tempLen] = '\0';
		stringArray[i] = newSbFrom(temp);
		free(temp);

		if (!stringArray[i])hasNull = true;

		if (j < len) {
			j++;
		}
	}

	*outArray = stringArray;
	*count = wholeCount;

	return hasNull ? SB_PARTIALSPLIT : SB_SUCCESSFULOP;
}

int trimSb(stringBuilder const str) {
	if (!str)return SB_NULL;
	if (!str->buffer)return SB_NULLBUFFER;
	if (str->buffer[0] == '\0')return SB_SUCCESSFULOP;

	size_t len = getSbLength(str);
	size_t start = 0;
	while (start < len && isspace((unsigned char)str->buffer[start])) {
		start++;
	}

	if (start == len) {
		str->buffer[0] = '\0';
		str->len = 0;
		return SB_SUCCESSFULOP;
	}

	size_t end = len - 1;
	while (end > start && isspace((unsigned char)str->buffer[end])) {
		end--;
	}

	size_t newLen = end - start + 1;

	if (start > 0) {
		memmove(str->buffer, str->buffer + start, newLen);
	}
	str->buffer[newLen] = '\0';
	str->len = newLen;
	
	return SB_SUCCESSFULOP;
}

stringBuilder cloneSb(const StringBuilder* const str) {
	if (!str)return NULL;
	return newSbFrom(str->buffer);
}

int charAtSb(const StringBuilder* const str, size_t index, char* result) {
	if (!result)return SB_NULLCHAR;
	if (!str) {
		return SB_NULL;
	}
	if (!str->buffer) {
		return SB_NULLBUFFER;
	}
	if (index >= getSbLength(str))return SB_IDXOUTBOUNDS;

	*result = str->buffer[index];

	return SB_SUCCESSFULIDX;
}

int sbIndexOfFront(const StringBuilder* const str, char ch, size_t* result) {
	if (!result)return SB_NULLINT;
	if (!str)return SB_NULL;
	if (!str->buffer)return SB_NULLBUFFER;

	const char* found = strchr(str->buffer, ch);
	if (found) {
		*result = (size_t)(found - str->buffer);
		return SB_SUCCEEDTOFINDCH;
	}
	return SB_WITHOUTCH;
}

int sbIndexOfBack(const StringBuilder* const str, char ch, size_t* result) {
	if (!result)return SB_NULLINT;
	if (!str)return SB_NULL;
	if (!str->buffer)return SB_NULLBUFFER;
	for (ptrdiff_t i = (ptrdiff_t)getSbLength(str) - 1; i >= 0; --i) {
		if (str->buffer[i] == ch) {
			*result = (size_t)i;
			return SB_SUCCEEDTOFINDCH;
		}
	}
	return SB_WITHOUTCH;
}

stringBuilder subSbInLengthFront(const StringBuilder* const str, size_t start, size_t length) {
	if (!str)return NULL;
	if (!str->buffer)return newSbFrom(NULL);
	if (str->buffer[0] == '\0' || !length)return newSbFrom("");

	size_t len = getSbLength(str);
	if (start >= len) {
		stringBuilder temp = newSbFrom("");
		if (temp)temp->status = SB_INVALIDSTART;
		return temp;
	}

	if (length - 1 >= len - start)length = len - start;

	stringBuilder temp = (stringBuilder)malloc(sizeof(StringBuilder));
	if (!temp)return NULL;
	temp->buffer = (char*)malloc(length + 1);
	if (!temp->buffer) {
		temp->status = SB_NULLBUFFER;
		temp->len = 0;
		temp->cap = 0;
		return temp;
	}
	memcpy(temp->buffer, str->buffer + start, length);
	temp->buffer[length] = '\0';
	temp->status = SB_AVAILABLE;
	temp->len = length;
	temp->cap = length + 1;
	return temp;
}

stringBuilder subSbInLengthBack(const StringBuilder* const str, size_t end, size_t length) {
	if (!str)return NULL;
	if (!str->buffer)return newSbFrom(NULL);
	if (str->buffer[0] == '\0' || !length)return newSbFrom("");

	size_t len = getSbLength(str);
	if (end >= len) {
		stringBuilder temp = newSbFrom("");
		if (temp)temp->status = SB_INVALIDEND;
		return temp;
	}

	if ((ptrdiff_t)end + 1 - (ptrdiff_t)length < 0)length = end + 1;

	stringBuilder temp = (stringBuilder)malloc(sizeof(StringBuilder));
	if (!temp)return NULL;
	temp->buffer = (char*)malloc(length + 1);
	if (!temp->buffer) {
		temp->status = SB_NULLBUFFER;
		temp->len = 0;
		temp->cap = 0;
		return temp;
	}
	memcpy(temp->buffer, str->buffer + (end - length + 1), length);
	temp->buffer[length] = '\0';
	temp->status = SB_AVAILABLE;
	temp->len = length;
	temp->cap = length + 1;
	return temp;
}

stringBuilder subSbInRange(const StringBuilder* const str, size_t start, size_t end) {
	if (!str)return NULL;
	if (!str->buffer)return newSbFrom(NULL);
	if (str->buffer[0] == '\0' || start == end)return newSbFrom("");

	if (start > end) {
		stringBuilder temp = newSbFrom("");
		if (temp)temp->status = SB_INVALIDIDX;
		return temp;
	}

	size_t len = getSbLength(str);
	if (start >= len || end > len) {
		stringBuilder temp = newSbFrom("");
		if (temp)temp->status = SB_IDXOUTBOUNDS;
		return temp;
	}

	size_t length = end - start;
	stringBuilder temp = (stringBuilder)malloc(sizeof(StringBuilder));
	if (!temp)return NULL;
	temp->buffer = (char*)malloc(length + 1);
	if (!temp->buffer) {
		temp->status = SB_NULLBUFFER;
		temp->len = 0;
		temp->cap = 0;
		return temp;
	}
	memcpy(temp->buffer, str->buffer + start, length);
	temp->buffer[length] = '\0';
	temp->status = SB_AVAILABLE;
	temp->len = length;
	temp->cap = length + 1;
	return temp;
}

stringBuilder subSbInLengthFrontFrom(const char* const ch, size_t start, size_t length) {
	if (!ch)return newSbFrom(NULL);
	if (ch[0] == '\0' || !length)return newSbFrom("");

	size_t len = strlen(ch);
	if (start >= len) {
		stringBuilder temp = newSbFrom("");
		if (temp)temp->status = SB_INVALIDSTART;
		return temp;
	}

	if (length - 1 >= len - start)length = len - start;

	stringBuilder temp = (stringBuilder)malloc(sizeof(StringBuilder));
	if (!temp)return NULL;
	temp->buffer = (char*)malloc(length + 1);
	if (!temp->buffer) {
		temp->status = SB_NULLBUFFER;
		temp->len = 0;
		temp->cap = 0;
		return temp;
	}
	memcpy(temp->buffer, ch + start, length);
	temp->buffer[length] = '\0';
	temp->status = SB_AVAILABLE;
	temp->len = length;
	temp->cap = length + 1;
	return temp;
}

stringBuilder subSbInLengthBackFrom(const char* const ch, size_t end, size_t length) {
	if (!ch)return newSbFrom(NULL);
	if (ch[0] == '\0' || !length)return newSbFrom("");

	size_t len = strlen(ch);
	if (end >= len) {
		stringBuilder temp = newSbFrom("");
		if (temp)temp->status = SB_INVALIDEND;
		return temp;
	}

	if ((ptrdiff_t)end + 1 - (ptrdiff_t)length < 0)length = end + 1;

	stringBuilder temp = (stringBuilder)malloc(sizeof(StringBuilder));
	if (!temp)return NULL;
	temp->buffer = (char*)malloc(length + 1);
	if (!temp->buffer) {
		temp->status = SB_NULLBUFFER;
		temp->len = 0;
		temp->cap = 0;
		return temp;
	}
	memcpy(temp->buffer, ch + (end - length + 1), length);
	temp->buffer[length] = '\0';
	temp->status = SB_AVAILABLE;
	temp->len = length;
	temp->cap = length + 1;
	return temp;
}

stringBuilder subSbInRangeFrom(const char* const ch, size_t start, size_t end) {
	if (!ch)return newSbFrom(NULL);
	if (ch[0] == '\0' || start == end)return newSbFrom("");

	if (start > end) {
		stringBuilder temp = newSbFrom("");
		if (temp)temp->status = SB_INVALIDIDX;
		return temp;
	}

	size_t len = strlen(ch);
	if (start >= len || end > len) {
		stringBuilder temp = newSbFrom("");
		if (temp)temp->status = SB_IDXOUTBOUNDS;
		return temp;
	}

	size_t length = end - start;
	stringBuilder temp = (stringBuilder)malloc(sizeof(StringBuilder));
	if (!temp)return NULL;
	temp->buffer = (char*)malloc(length + 1);
	if (!temp->buffer) {
		temp->status = SB_NULLBUFFER;
		temp->len = 0;
		temp->cap = 0;
		return temp;
	}
	memcpy(temp->buffer, ch + start, length);
	temp->buffer[length] = '\0';
	temp->status = SB_AVAILABLE;
	temp->len = length;
	temp->cap = length + 1;
	return temp;
}

int replaceSbFirst(StringBuilder* str, const char* const target, const StringBuilder* const replacement) {
	if (!str)return SB_NULL;
	if (!replacement)return SB_INVALIDREPLACEMENT;
	return replaceSbFirstFrom(str, target, replacement->buffer);
}

int replaceSbFirstFrom(StringBuilder* str, const char* const target, const char* const replacement) {
	if (!str)return SB_NULL;

	if (!str->buffer)return SB_NULLBUFFER;
	if (!target)return SB_NULLTARGET;
	if (!replacement)return SB_INVALIDREPLACEMENT;

	if (str->buffer[0] == '\0' && target[0] == '\0')return setSbContent(str, replacement);
	if (str->buffer[0] == '\0')return SB_NOTARGETMATCHED;
	if (target[0] == '\0')return appendSbFrom(str, replacement);

	char* indexChar = strstr(str->buffer, target);
	if (!indexChar)return SB_NOTARGETMATCHED;

	size_t index = (size_t)(indexChar - str->buffer);
	size_t targetLen = strlen(target);
	size_t replaceLen = strlen(replacement);
	size_t strLen = getSbLength(str);

	if (targetLen == replaceLen) {
		memmove(str->buffer + index, replacement, replaceLen);
		return SB_SUCCESSFULOP;
	}

	size_t preLen = index;
	size_t sufLen = strLen - preLen - targetLen;
	size_t newLen = preLen + replaceLen + sufLen;
	size_t needed = newLen + 1;

	if (str->cap < needed) {
		char* newBuffer = (char*)realloc(str->buffer, needed);
		if (!newBuffer)return SB_FAILEDTOREALLOC;
		str->buffer = newBuffer;
		str->cap = needed;
	}

	memmove(str->buffer + index + replaceLen, str->buffer + index + targetLen, sufLen + 1);
	memcpy(str->buffer + index, replacement, replaceLen);

	str->len = newLen;
	return SB_SUCCESSFULOP;
}

int replaceSbAll(StringBuilder* str, const char* const target, const StringBuilder* const replacement) {
	if (!str)return SB_NULL;
	if (!replacement)return SB_INVALIDREPLACEMENT;
	return replaceSbAllFrom(str, target, replacement->buffer);
}

int replaceSbAllFrom(StringBuilder* str, const char* const target, const char* const replacement) {
	if (!str)return SB_NULL;

	if (!str->buffer)return SB_NULLBUFFER;
	if (!target)return SB_NULLTARGET;
	if (!replacement)return SB_INVALIDREPLACEMENT;

	if (str->buffer[0] == '\0' && target[0] == '\0')return setSbContent(str, replacement);
	if (str->buffer[0] == '\0')return SB_NOTARGETMATCHED;
	if (target[0] == '\0')return appendSbFrom(str, replacement);

	size_t strLen = str->len;
	size_t targetLen = strlen(target);
	size_t replaceLen = strlen(replacement);

	// 第一遍：扫描所有匹配位置
	size_t capacity = 16;
	size_t* positions = (size_t*)malloc(capacity * sizeof(size_t));
	if (!positions)return SB_FAILEDTOALLOC;

	size_t count = 0;
	size_t scan = 0;
	while (scan + targetLen <= strLen) {
		if (memcmp(str->buffer + scan, target, targetLen) == 0) {
			if (count == capacity) {
				capacity *= 2;
				size_t* newPositions = (size_t*)realloc(positions, capacity * sizeof(size_t));
				if (!newPositions) {
					free(positions);
					return SB_FAILEDTOREALLOC;
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
		return SB_NOTARGETMATCHED;
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
			return SB_FAILEDTOREALLOC;
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
	return SB_SUCCESSFULOP;
}

bool sbStartWith(const StringBuilder* const str, const char* const prefix) {
	if (!str || !str->buffer)return false;

	if (!prefix || prefix[0] == '\0')return true;

	size_t strLen = getSbLength(str);
	size_t preLen = strlen(prefix);
	if (strLen < preLen)return false;

	return strncmp(str->buffer, prefix, preLen) == 0;
}

bool sbEndWith(const StringBuilder* const str, const char* const suffix) {
	if (!str || !str->buffer)return false;

	if (!suffix || suffix[0] == '\0')return true;

	size_t strLen = getSbLength(str);
	size_t sufLen = strlen(suffix);
	if (strLen < sufLen)return false;

	return strncmp(str->buffer + (strLen - sufLen), suffix, sufLen) == 0;
}

bool sbStartWithString(const StringBuilder* const str, const StringBuilder* const prefix) {
	if (!prefix)return true;
	return sbStartWith(str, prefix->buffer);
}

bool sbEndWithString(const StringBuilder* const str, const StringBuilder* const suffix) {
	if (!suffix)return true;
	return sbEndWith(str, suffix->buffer);
}

bool sbIsEmpty(const StringBuilder* const str) {
	return !str || !str->buffer || str->buffer[0] == '\0' ? true : false;
}

bool sbIsBlank(const StringBuilder* const str) {
	if (!str || !str->buffer)return true;

	for (const char* p = str->buffer; *p; p++) {
		if (!isspace((unsigned char)*p)) {
			return false;
		}
	}

	return true;
}

bool equalsSb(const StringBuilder* const str1, const StringBuilder* const str2) {
	if (str1 == str2)return true;
	if (!str1 || !str2)return false;

	if ((!str1->buffer || str1->buffer[0] == '\0') && (!str2->buffer || str2->buffer[0] == '\0'))return true;
	if ((!str1->buffer || str1->buffer[0] == '\0') || (!str2->buffer || str2->buffer[0] == '\0'))return false;

	return strcmp(str1->buffer, str2->buffer) == 0;
}

bool equalsSbFrom(const StringBuilder* const str, const char* const ch) {
	if ((!str || !str->buffer) && !ch)return true;
	if (((!str || !str->buffer) && ch) || ((str && str->buffer) && !ch))return false;
	return strcmp(str->buffer, ch) == 0;
}

bool containsSb(const StringBuilder* const str1, const StringBuilder* const str2) {
	if (!str2)return true;
	if (!str2->buffer)return str1 != NULL;
	if (str2->buffer[0] == '\0') {
		if (!str1 || !str1->buffer)return false;
		return true;
	}

	if (!str1 || !str1->buffer || str1->buffer[0] == '\0')return false;

	return strstr(str1->buffer, str2->buffer) != NULL;
}

bool containsSbFrom(const StringBuilder* const str, const char* const ch) {
	if (!ch)return str != NULL;
	if (ch[0] == '\0') {
		if (!str || !str->buffer)return false;
		return true;
	}

	if (!str || !str->buffer || str->buffer[0] == '\0')return false;

	return strstr(str->buffer, ch) != NULL;
}


char* sbToArray(const StringBuilder* const str) {
	if (!str || !str->buffer)return NULL;
	if (str->buffer[0] == '\0') {
		char* temp = (char*)calloc(1, sizeof(char));
		if (!temp)return NULL;
		temp[0] = '\0';
		return temp;
	}

	size_t len = getSbLength(str) + 1;
	char* content = (char*)malloc(len);
	return content ? memcpy(content, str->buffer, len) : NULL;
}

int sbToInt(const StringBuilder* const str, int* result) {
	if (!str || !str->buffer || str->buffer[0] == '\0')return SB_EMPTY;
	if (getSbStatus(str))return SB_WRONGSTATUS;
	if (!result)return SB_NULLINT;

	const char* p = str->buffer;
	size_t len = strlen(str->buffer);

	size_t start = 0;
	while (start < len && isspace((unsigned char)p[start]))start++;

	size_t end = len;
	while (end > start && isspace((unsigned char)p[end - 1]))end--;

	if (start == end) {
		return SB_BLANK;
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
				return SB_OVERFLOWINT;
			}

			tempNum = 10 * tempNum - digit;
			numCount++;
		}
		else {
			return SB_NOTINT;
		}
	}

	if (numCount) {
		if (isNegative)*result = tempNum;
		else *result = -tempNum;
		return SB_SUCCEEDTOINT;
	}
	else {
		return SB_NOTINT;
	}
}

int sbToLong(const StringBuilder* const str, long* result) {
	if (!str || !str->buffer || str->buffer[0] == '\0')return SB_EMPTY;
	if (getSbStatus(str))return SB_WRONGSTATUS;
	if (!result)return SB_NULLLONG;

	const char* p = str->buffer;
	size_t len = strlen(str->buffer);

	size_t start = 0;
	while (start < len && isspace((unsigned char)p[start]))start++;

	size_t end = len;
	while (end > start && isspace((unsigned char)p[end - 1]))end--;

	if (start == end) {
		return SB_BLANK;
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
				return SB_OVERFLOWLONG;
			}

			tempNum = 10 * tempNum - digit;
			numCount++;
		}
		else {
			return SB_NOTLONG;
		}
	}

	if (numCount) {
		if (isNegative)*result = tempNum;
		else *result = -tempNum;
		return SB_SUCCEEDTOLONG;
	}
	else {
		return SB_NOTLONG;
	}
}

int sbToLongLong(const StringBuilder* const str, long long* result) {
	if (!str || !str->buffer || str->buffer[0] == '\0')return SB_EMPTY;
	if (getSbStatus(str))return SB_WRONGSTATUS;
	if (!result)return SB_NULLLONGLONG;

	const char* p = str->buffer;
	size_t len = strlen(str->buffer);

	size_t start = 0;
	while (start < len && isspace((unsigned char)p[start]))start++;

	size_t end = len;
	while (end > start && isspace((unsigned char)p[end - 1]))end--;

	if (start == end) {
		return SB_BLANK;
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
				return SB_OVERFLOWLONGLONG;
			}

			tempNum = 10 * tempNum - digit;
			numCount++;
		}
		else {
			return SB_NOTLONGLONG;
		}
	}

	if (numCount) {
		if (isNegative)*result = tempNum;
		else *result = -tempNum;
		return SB_SUCCEEDTOLONGLONG;
	}
	else {
		return SB_NOTLONGLONG;
	}
}

int sbToFloat(const StringBuilder* const str, float* result) {
	if (!str || !str->buffer || str->buffer[0] == '\0')return SB_EMPTY;
	if (getSbStatus(str))return SB_WRONGSTATUS;
	if (!result)return SB_NULLFLOAT;

	const char* p = str->buffer;
	size_t len = strlen(str->buffer);

	size_t start = 0;
	while (start < len && isspace((unsigned char)p[start]))start++;

	size_t end = len;
	while (end > start && isspace((unsigned char)p[end - 1]))end--;

	if (start == end) {
		return SB_BLANK;
	}

	size_t i = start;
	bool seenDigit = false;
	bool seenDot = false;

	if (p[i] == '-' || p[i] == '+') i++;

	if (i < end && p[i] == '.') {
		return SB_NOTFLOAT;   // 不允许 ".5" 这种没有整数部分的格式
	}

	for (; i < end; i++) {
		if (p[i] >= '0' && p[i] <= '9') {
			seenDigit = true;
		}
		else if (p[i] == '.' && !seenDot) {
			seenDot = true;
		}
		else {
			return SB_NOTFLOAT;
		}
	}

	if (!seenDigit) return SB_NOTFLOAT;

	// 调用标准库转换，从 p + start 开始
	char* endptr = NULL;
	errno = 0;
	float value = strtof(p + start, &endptr);

	// 确认解析到有效片段末尾
	if (endptr != p + end) {
		return SB_NOTFLOAT;
	}

	if (errno == ERANGE) {
		return SB_OVERFLOWFLOAT;
	}

	*result = value;
	return SB_SUCCEEDTOFLOAT;
}

int sbToDouble(const StringBuilder* const str, double* result) {
	if (!str || !str->buffer || str->buffer[0] == '\0')return SB_EMPTY;
	if (getSbStatus(str))return SB_WRONGSTATUS;
	if (!result)return SB_NULLDOUBLE;

	const char* p = str->buffer;
	size_t len = strlen(str->buffer);

	size_t start = 0;
	while (start < len && isspace((unsigned char)p[start]))start++;

	size_t end = len;
	while (end > start && isspace((unsigned char)p[end - 1]))end--;

	if (start == end) {
		return SB_BLANK;
	}

	size_t i = start;
	bool seenDigit = false;
	bool seenDot = false;

	if (p[i] == '-' || p[i] == '+') i++;

	if (i < end && p[i] == '.') {
		return SB_NOTDOUBLE;   // 不允许 ".5" 这种没有整数部分的格式
	}

	for (; i < end; i++) {
		if (p[i] >= '0' && p[i] <= '9') {
			seenDigit = true;
		}
		else if (p[i] == '.' && !seenDot) {
			seenDot = true;
		}
		else {
			return SB_NOTDOUBLE;
		}
	}

	if (!seenDigit) return SB_NOTDOUBLE;

	// 调用标准库转换，从 p + start 开始
	char* endptr = NULL;
	errno = 0;
	double value = strtod(p + start, &endptr);

	// 确认解析到有效片段末尾
	if (endptr != p + end) {
		return SB_NOTDOUBLE;
	}

	if (errno == ERANGE) {
		return SB_OVERFLOWDOUBLE;
	}

	*result = value;
	return SB_SUCCEEDTODOUBLE;
}

int sbToLowerCase(StringBuilder* str) {
	if (!str)return SB_NULL;
	if (!str->buffer)return SB_NULLBUFFER;

	for (size_t i = 0; i < str->len; i++) {
		str->buffer[i] = (char)tolower((unsigned char)str->buffer[i]);
	}

	return SB_SUCCESSFULOP;
}

int sbToUpperCase(StringBuilder* str) {
	if (!str)return SB_NULL;
	if (!str->buffer)return SB_NULLBUFFER;

	for (size_t i = 0; i < str->len; i++) {
		str->buffer[i] = (char)toupper((unsigned char)str->buffer[i]);
	}

	return SB_SUCCESSFULOP;
}

string sbToString(const StringBuilder* const str) {
	return newStringFrom(getSbBuffer(str));
}