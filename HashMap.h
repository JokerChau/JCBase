#pragma once

#include "wheel_ex.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "HashMapSrc.h"

typedef struct hashMap {
	uint64_t identifier;
}hashMap;

typedef struct hashMapStatusReport {
	bool valid;
	int lastCreateStatus;
	int instanceStatus;
}hashMapStatusReport;

typedef size_t(*hashFunction)(const void* key, size_t keySize);
typedef int (*compareFunction)(const void* keyA, const void* keyB, size_t keySize);

//static const hashMap invalidHashMap = { 0 };

static inline bool hmHasIdentifier(hashMap handle) {
	return handle.identifier != 0;
}

wheels hashMapStatusReport hmGetStatusReport(hashMap handle);

// ============================================================
// 内部构造器：由 hmCreate 宏展开调用，不要直接调用。
// 它不做类型检查，直接接收 hash/compare 函数指针。
// 直接调用它可能传入不匹配的 hash/compare，导致运行时崩溃。
// 永远使用 hmCreate 宏。
// ============================================================
wheels hashMap hmNew(size_t keySize, size_t valueSize,
	hashFunction hash, compareFunction compare);

wheels int hmGetLastCreateStatus(void);

wheels void hmDestroy(hashMap* handle);

wheels int hmPut(hashMap handle, const void* key, const void* value);

wheels int hmGet(hashMap handle, const void* key, void* outValue);

wheels int hmRemove(hashMap handle, const void* key);

wheels int hmContains(hashMap handle, const void* key);

wheels int hmClear(hashMap handle);

wheels size_t hmGetCount(hashMap handle);

wheels size_t hmGetBucketCount(hashMap handle);

wheels int hmGetStatus(hashMap handle);

wheels const char* hmStatusToArray(int status);

// char* 内容哈希和比较，供 hmNew 宏和调用方使用
wheels size_t hashCString(const void* key, size_t keySize);
wheels int compareCString(const void* keyA, const void* keyB, size_t keySize);

// ============================================================
// hmCreate：唯一对外构造入口。
//
// 用法：
//   int key;
//   char* value;
//   hashMap map = hmCreate(&key, &value, NULL, NULL);
//
//   char* key2;
//   char* value2;
//   hashMap map2 = hmCreate(&key2, &value2, NULL, NULL);
//
//   MyStruct* key3;
//   char* value3;
//   hashMap map3 = hmCreate(&key3, &value3, myHash, myCompare);
//
// 第四个和第五个参数是给调用方自定义类型准备的。
// 如果 key 类型已经在上面的 _Generic 列表里，这两个参数传 NULL, NULL。
// ============================================================
#define hmCreate(keyPointer, valuePointer, customHash, customCompare) \
	hmNew( \
		sizeof(*(keyPointer)), \
		sizeof(*(valuePointer)), \
		_Generic((keyPointer), \
			char*:               NULL, \
			signed char*:        NULL, \
			unsigned char*:      NULL, \
			short*:              NULL, \
			unsigned short*:     NULL, \
			int*:                NULL, \
			unsigned int*:       NULL, \
			long*:               NULL, \
			unsigned long*:      NULL, \
			long long*:          NULL, \
			unsigned long long*: NULL, \
			float*:              NULL, \
			double*:             NULL, \
			long double*:        NULL, \
			size_t*:			 NULL, \
			void**:              NULL, \
			char**:              hashCString, \
			default:             customHash \
		), \
		_Generic((keyPointer), \
			char*:               NULL, \
			signed char*:        NULL, \
			unsigned char*:      NULL, \
			short*:              NULL, \
			unsigned short*:     NULL, \
			int*:                NULL, \
			unsigned int*:       NULL, \
			long*:               NULL, \
			unsigned long*:      NULL, \
			long long*:          NULL, \
			unsigned long long*: NULL, \
			float*:              NULL, \
			double*:             NULL, \
			long double*:        NULL, \
			size_t*:			 NULL, \
			void**:              NULL, \
			char**:              compareCString, \
			default:             customCompare \
		) \
	)