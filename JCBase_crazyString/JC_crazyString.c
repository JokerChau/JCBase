#include "JC_crazyString.h"
#include "JC_crazyStringStructure.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static jc_czyString wrapString(char* content, size_t length, size_t capacity) {
	jc_czyString str = (jc_czyString)malloc(sizeof(*str));
	if (!str)return NULL;
	str->buffer = content;
	str->len = length;
	str->cap = capacity;
	return str;
}

static jc_czyString reallocate(jc_czyString str, size_t wholeCapacity) {
	char* tempBuf = (char*)realloc(str->buffer, wholeCapacity);
	if (!tempBuf)return str;
	str->buffer = tempBuf;
	str->cap = wholeCapacity;
	return str;
}

// 创建一个空内容的实例
jc_czyString jccstr_new(void) {
	char* buffer = (char*)malloc(1);
	if (!buffer)return NULL;
	buffer[0] = '\0';
	jc_czyString str = wrapString(buffer, 0, 1);
	if (!str)free(buffer);
	return str;
}

// 创建一个指定容量大小的实例
jc_czyString jccstr_newWithSize(size_t capacity) {
	// 如果容量为0则返回NULL
	if (!capacity)return NULL;
	char* buffer = (char*)malloc(capacity);
	if (!buffer)return NULL;
	buffer[0] = '\0';
	jc_czyString str = wrapString(buffer, 0, capacity);
	if (!str)free(buffer);
	return str;
}

// 当length小于content实际内容长度时，实例记录的字符串内容较原内容将会截断
jc_czyString jccstr_newFrom(const char* content, size_t length) {
	if (length == SIZE_MAX)return NULL;
	if (!content)return NULL;
	char* buffer = (char*)malloc(length + 1);
	if (!buffer)return NULL;
	memcpy(buffer, content, length);
	buffer[length] = '\0';
	jc_czyString str = wrapString(buffer, length, length + 1);
	if (!str)free(buffer);
	return str;
}

void jccstr_delete(jc_czyString str) {
	if (!str)return;
	if (str->buffer)free(str->buffer);
	free(str);
}

const char* jccstr_viewBuffer(jc_czyString str) {
	if (!str)return NULL;
	return str->buffer;
}

char* jccstr_toCharArray(jc_czyString str) {
	if (!str)return NULL;
	char* cstr = (char*)malloc(str->len + 1);
	if (!cstr)return NULL;
	memcpy(cstr, str->buffer, str->len);
	cstr[str->len] = '\0';
	return cstr;
}

size_t jccstr_getLength(jc_czyString str) {
	if (!str)return 0;
	return str->len;
}

size_t jccstr_getCapacity(jc_czyString str) {
	if (!str)return 0;
	return str->cap;
}

jc_czyString jccstr_changeAt(jc_czyString str, size_t index, char content) {
	str->buffer[index] = content;
	if (content == '\0')str->len = index;
	return str;
}

jc_czyString jccstr_changeFrom(jc_czyString str, size_t from, size_t length, const char* content) {
	if (length > SIZE_MAX - from - 1) return str;

	if (from + length + 1 > str->cap) {
		reallocate(str, from + length + 1);
		if (str->cap < from + length + 1)return str;
	}

	memmove(str->buffer + from, content, length);
	if (from + length > str->len) str->buffer[from + length] = '\0';

	char* p = str->buffer + from;
	while (*p != '\0')p++;
	str->len = p - str->buffer;
	return str;
}

jc_czyString jccstr_endAt(jc_czyString str, size_t index) {
	str->buffer[index] = '\0';
	str->len = index;
	return str;
}

jc_czyString jccstr_clear(jc_czyString str) {
	str->buffer[0] = '\0';
	str->len = 0;
	return str;
}

jc_czyString jccstr_removeAt(jc_czyString str, size_t index) {
	memmove(str->buffer + index, str->buffer + index + 1, str->len - index - 1);
	str->len--;
	str->buffer[str->len] = '\0';
	return str;
}

jc_czyString jccstr_removeInRange(jc_czyString str, size_t from, size_t to) {
	memmove(str->buffer + from, str->buffer + to, str->len - to);
	str->len -= to - from;
	str->buffer[str->len] = '\0';
	return str;
}

jc_czyString jccstr_insertAt(jc_czyString str, size_t index, char content) {
	if (str->len + 1 >= str->cap) {
		size_t tempCap = str->cap;
		reallocate(str, str->cap + 1);
		if (str->cap == tempCap)return str;
	}

	memmove(str->buffer + index + 1, str->buffer + index, str->len - index + 1);
	str->buffer[index] = content;
	if (content == '\0')str->len = index;
	else str->len++;
	return str;
}

jc_czyString jccstr_insertBytes(jc_czyString str, size_t index, const char* content, size_t length) {
	if (!length)return str;
	if (length > SIZE_MAX - str->len - 1)return str;

	// 扩容
	size_t need = str->len + length + 1;
	if (need > str->cap) {
		reallocate(str, need);
		if (str->cap < need)return str;
	}

	// 迁移数据
	memmove(str->buffer + index + length, str->buffer + index, str->len - index);
	memmove(str->buffer + index, content, length);
	str->len += length;
	str->buffer[str->len] = '\0';
	return str;
}

jc_czyString jccstr_setContent(jc_czyString str, const char* content, size_t length) {
	if (length > SIZE_MAX - 1)length = SIZE_MAX - 1;
	if (length >= str->cap) {
		reallocate(str, length + 1);
		if (str->cap < length + 1) return str;
	}
	memmove(str->buffer, content, length);
	str->buffer[length] = '\0';
	str->len = length;
	return str;
}

jc_czyString jccstr_reserve(jc_czyString str, size_t wholeCapacity) {
	// 新容量比旧容量小时默认不更改
	if (str->cap < wholeCapacity) reallocate(str, wholeCapacity);
	return str;
}

jc_czyString jccstr_shrinkToFit(jc_czyString str) {
	return reallocate(str, str->len + 1);
}

jc_czyString jccstr_reallocate(jc_czyString str, size_t capacity) {
	reallocate(str, capacity);

	if (str->cap == capacity && str->len >= capacity) {
		str->len = capacity - 1;
		str->buffer[str->len] = '\0';
	}
	return str;
}

jc_czyString jccstr_clone(jc_czyString str) {
	if (!str)return NULL;
	char* buffer = (char*)malloc(str->cap);
	if (!buffer)return NULL;
	memcpy(buffer, str->buffer, str->cap);
	jc_czyString tempStr = wrapString(buffer, str->len, str->cap);
	if (!tempStr)free(buffer);
	return tempStr;
}

jc_czyString jccstr_appendBytes(jc_czyString str, const char* content, size_t length) {
	if (!length) return str;

	if (length > SIZE_MAX - str->len - 1) return str; // 加法溢出，视为非法参数，原样返回
	size_t need = str->len + length + 1;

	if (need > str->cap) {
		reallocate(str, need);
		if (str->cap < need) return str;       // 扩容失败，原样返回
	}

	size_t oldLen = str->len;
	memmove(str->buffer + oldLen, content, length);
	str->buffer[oldLen + length] = '\0';

	// 内容里若含 \0，按 C 语义在此截断
	char* p = str->buffer + oldLen;
	while (*p != '\0') p++;
	str->len = (size_t)(p - str->buffer);

	return str;
}

jc_czyString jccstr_append(jc_czyString str, jc_czyString other) {
	if (!other->len) return str;

	size_t oldLen = str->len;
	size_t addLen = other->len;
	if (oldLen > SIZE_MAX - addLen - 1)return str; // 加法溢出，视为非法参数
	size_t need = oldLen + addLen + 1;

	if (need > str->cap) {
		jccstr_reserve(str, need);
		if (str->cap < need) return str;       // 扩容失败，原样返回
	}

	// 先扩容再取源指针：若 str == other，realloc 之后 other->buffer 已经更新为同一新地址
	const char* src = (str == other) ? str->buffer : other->buffer;
	memmove(str->buffer + oldLen, src, addLen);
	str->buffer[oldLen + addLen] = '\0';

	// 内容里若含 \0，按 C 语义在此截断
	char* p = str->buffer + oldLen;
	while (*p != '\0') p++;
	str->len = (size_t)(p - str->buffer);

	return str;
}

jc_czyString jccstr_concat(jc_czyString a, jc_czyString b) {
	if (!a || !b) return NULL;
	if (a->len > SIZE_MAX - b->len - 1) return NULL;   // 长度溢出

	size_t need = a->len + b->len + 1;
	jc_czyString out = jccstr_newWithSize(need);
	if (!out) return NULL;

	if (a->len) memmove(out->buffer, a->buffer, a->len);
	if (b->len) memmove(out->buffer + a->len, b->buffer, b->len);
	out->buffer[need - 1] = '\0';
	out->len = need - 1;

	return out;
}