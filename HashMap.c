#include "JC_HashMap.h"
#include <stdlib.h>
#include <string.h>

#define INITIAL_BUCKET_COUNT 16
#define MIN_BUCKET_COUNT 16

// ============================================================
// 内部结构：RealHashMap
// ============================================================

typedef struct HashMapNode {
	void* key;
	void* value;
	struct HashMapNode* next;
} HashMapNode;

typedef struct RealHashMap {
	HashMapNode** buckets;
	size_t bucketCount;
	size_t elementCount;
	size_t keySize;
	size_t valueSize;
	hashFunction hash;
	compareFunction compare;
	int status;
} RealHashMap;

// ============================================================
// 默认哈希函数 FNV-1a
// ============================================================

static size_t fnv1a(const void* data, size_t size) {
	const unsigned char* bytes = (const unsigned char*)data;
	size_t hash = 14695981039346656037ULL;
	for (size_t i = 0; i < size; ++i) {
		hash ^= bytes[i];
		hash *= 1099511628211ULL;
	}
	return hash;
}

// ============================================================
// RealHashMap 内部操作
// ============================================================

static int realHmResize(RealHashMap* map, size_t newBucketCount) {
	// 设定最小桶数量
	if (newBucketCount < MIN_BUCKET_COUNT) newBucketCount = MIN_BUCKET_COUNT;

	// 为同数组申请内存
	HashMapNode** newBuckets = (HashMapNode**)calloc(newBucketCount, sizeof(HashMapNode*));
	if (!newBuckets) return HM_MALLOCFAIL;

	for (size_t i = 0; i < map->bucketCount; ++i) {
		HashMapNode* node = map->buckets[i];
		while (node) {
			HashMapNode* next = node->next;
			size_t h = map->hash(node->key, map->keySize);
			size_t bucket = h & (newBucketCount - 1);
			node->next = newBuckets[bucket];
			newBuckets[bucket] = node;
			node = next;
		}
	}

	free(map->buckets);
	map->buckets = newBuckets;
	map->bucketCount = newBucketCount;
	return HM_SUCCESSFULOP;
}

static RealHashMap* realHmCreate(size_t keySize, size_t valueSize,
	hashFunction hash, compareFunction compare) {

	RealHashMap* map = (RealHashMap*)malloc(sizeof(RealHashMap));
	if (!map) return NULL;

	map->bucketCount = INITIAL_BUCKET_COUNT;
	map->elementCount = 0;
	map->keySize = keySize;
	map->valueSize = valueSize;
	map->hash = hash ? hash : fnv1a;
	map->compare = compare ? compare : memcmp;
	map->status = HM_AVAILABLE;

	map->buckets = (HashMapNode**)calloc(map->bucketCount, sizeof(HashMapNode*));
	if (!map->buckets) {
		free(map);
		return NULL;
	}
	return map;
}

static void realHmDestroy(RealHashMap* map) {
	if (!map) return;
	for (size_t i = 0; i < map->bucketCount; ++i) {
		HashMapNode* node = map->buckets[i];
		while (node) {
			HashMapNode* next = node->next;
			free(node->key);
			free(node->value);
			free(node);
			node = next;
		}
	}
	free(map->buckets);
	free(map);
}

static int realHmPut(RealHashMap* map, const void* key, const void* value) {
	if (!map) return HM_NULL;
	if (map->status != HM_AVAILABLE) return HM_WRONGSTATUS;
	if (!key) return HM_NULLKEY;
	if (!value) return HM_NULLVALUE;

	size_t h = map->hash(key, map->keySize);
	size_t bucket = h & (map->bucketCount - 1);

	HashMapNode* node = map->buckets[bucket];
	while (node) {
		if (map->compare(node->key, key, map->keySize) == 0) {
			void* newValue = malloc(map->valueSize);
			if (!newValue) return HM_MALLOCFAIL;
			memcpy(newValue, value, map->valueSize);
			free(node->value);
			node->value = newValue;
			return HM_SUCCESSFULOP;
		}
		node = node->next;
	}

	HashMapNode* newNode = (HashMapNode*)malloc(sizeof(HashMapNode));
	if (!newNode) return HM_MALLOCFAIL;

	newNode->key = malloc(map->keySize);
	if (!newNode->key) {
		free(newNode);
		return HM_MALLOCFAIL;
	}

	newNode->value = malloc(map->valueSize);
	if (!newNode->value) {
		free(newNode->key);
		free(newNode);
		return HM_MALLOCFAIL;
	}

	memcpy(newNode->key, key, map->keySize);
	memcpy(newNode->value, value, map->valueSize);

	newNode->next = map->buckets[bucket];
	map->buckets[bucket] = newNode;
	map->elementCount++;

	if (map->elementCount * 4 > map->bucketCount * 3) {
		int resizeStatus = realHmResize(map, map->bucketCount * 2);
		if (resizeStatus != HM_SUCCESSFULOP) {
			return HM_EXPANDFAILED;
		}
	}

	return HM_SUCCESSFULOP;
}

static int realHmGet(RealHashMap* map, const void* key, void* outValue) {
	if (!map) return HM_NULL;
	if (map->status != HM_AVAILABLE) return HM_WRONGSTATUS;
	if (!key) return HM_NULLKEY;
	if (!outValue) return HM_NULLOUTVALUE;

	size_t h = map->hash(key, map->keySize);
	size_t bucket = h & (map->bucketCount - 1);

	HashMapNode* node = map->buckets[bucket];
	while (node) {
		if (map->compare(node->key, key, map->keySize) == 0) {
			memcpy(outValue, node->value, map->valueSize);
			return HM_SUCCESSFULOP;
		}
		node = node->next;
	}
	return HM_KEYNOTFOUND;
}

static int realHmRemove(RealHashMap* map, const void* key) {
	if (!map) return HM_NULL;
	if (map->status != HM_AVAILABLE) return HM_WRONGSTATUS;
	if (!key) return HM_NULLKEY;

	size_t h = map->hash(key, map->keySize);
	size_t bucket = h & (map->bucketCount - 1);

	HashMapNode** link = &map->buckets[bucket];
	while (*link) {
		HashMapNode* node = *link;
		if (map->compare(node->key, key, map->keySize) == 0) {
			*link = node->next;
			free(node->key);
			free(node->value);
			free(node);
			map->elementCount--;

			if (map->bucketCount > MIN_BUCKET_COUNT &&
				map->elementCount * 4 < map->bucketCount) {
				int resizeStatus = realHmResize(map, map->bucketCount / 2);
				if (resizeStatus != HM_SUCCESSFULOP) {
					return HM_SHRINKFAILED;
				}
			}
			return HM_SUCCESSFULOP;
		}
		link = &node->next;
	}
	return HM_KEYNOTFOUND;
}

static int realHmContains(RealHashMap* map, const void* key) {
	if (!map) return HM_NULL;
	if (map->status != HM_AVAILABLE) return HM_WRONGSTATUS;
	if (!key) return HM_NULLKEY;

	size_t h = map->hash(key, map->keySize);
	size_t bucket = h & (map->bucketCount - 1);

	HashMapNode* node = map->buckets[bucket];
	while (node) {
		if (map->compare(node->key, key, map->keySize) == 0) {
			return HM_SUCCESSFULOP;
		}
		node = node->next;
	}
	return HM_KEYNOTFOUND;
}

static int realHmClear(RealHashMap* map) {
	if (!map) return HM_NULL;

	for (size_t i = 0; i < map->bucketCount; ++i) {
		HashMapNode* node = map->buckets[i];
		while (node) {
			HashMapNode* next = node->next;
			free(node->key);
			free(node->value);
			free(node);
			node = next;
		}
		map->buckets[i] = NULL;
	}
	map->elementCount = 0;
	return HM_SUCCESSFULOP;
}

// ============================================================
// 注册表：identifier -> RealHashMap*
// 注册表自己也是一个 RealHashMap，但不对外暴露。
// ============================================================

static RealHashMap* registry = NULL;
static uint64_t nextIdentifier = 1;
static bool identifierExhausted = false;
static int lastCreateStatus = HM_SUCCESSFULOP;

static bool ensureRegistry(void) {
	if (registry != NULL) return true;
	registry = realHmCreate(sizeof(uint64_t), sizeof(RealHashMap*), NULL, NULL);
	return registry != NULL;
}

static RealHashMap* resolveHandle(hashMap handle) {
	if (handle.identifier == 0) return NULL;

	RealHashMap* real = NULL;
	if (realHmGet(registry, &handle.identifier, &real) != HM_SUCCESSFULOP) {
		return NULL;
	}
	return real;
}

// ============================================================
// 对外接口实现
// ============================================================

hashMap hmNew(size_t keySize, size_t valueSize,
	hashFunction hash, compareFunction compare) {
	hashMap invalid = { 0 };
	lastCreateStatus = HM_SUCCESSFULOP;

	if (identifierExhausted) {
		lastCreateStatus = HM_IDENTIFIEREXHAUSTED;
		return invalid;
	}

	if (!ensureRegistry()) {
		lastCreateStatus = HM_MALLOCFAIL;
		return invalid;
	}

	if (nextIdentifier == UINT64_MAX) {
		identifierExhausted = true;
		lastCreateStatus = HM_IDENTIFIEREXHAUSTED;
		return invalid;
	}

	RealHashMap* real = realHmCreate(keySize, valueSize, hash, compare);
	if (!real) {
		lastCreateStatus = HM_MALLOCFAIL;
		return invalid;
	}

	if (keySize == 0 || valueSize == 0) {
		real->status = HM_INVALIDSIZE;
	}

	uint64_t identifier = nextIdentifier++;

	int putStatus = realHmPut(registry, &identifier, &real);
	if (putStatus != HM_SUCCESSFULOP && putStatus != HM_EXPANDFAILED) {
		realHmDestroy(real);
		lastCreateStatus = putStatus;
		return invalid;
	}

	lastCreateStatus = putStatus;

	hashMap handle;
	handle.identifier = identifier;
	return handle;
}

int hmGetLastCreateStatus(void) {
	return lastCreateStatus;
}

hashMapStatusReport hmGetStatusReport(hashMap handle) {
	hashMapStatusReport report;
	report.valid = hmHasIdentifier(handle);
	report.lastCreateStatus = lastCreateStatus;
	report.instanceStatus = hmGetStatus(handle);
	return report;
}

void hmDestroy(hashMap* handle) {
	if (!handle) return;
	if (handle->identifier == 0) return;

	uint64_t identifier = handle->identifier;
	handle->identifier = 0;

	RealHashMap* real = NULL;
	if (realHmGet(registry, &identifier, &real) != HM_SUCCESSFULOP) {
		return;
	}

	realHmRemove(registry, &identifier);
	realHmDestroy(real);
}

int hmPut(hashMap handle, const void* key, const void* value) {
	RealHashMap* real = resolveHandle(handle);
	if (!real) return HM_NULL;
	return realHmPut(real, key, value);
}

int hmGet(hashMap handle, const void* key, void* outValue) {
	RealHashMap* real = resolveHandle(handle);
	if (!real) return HM_NULL;
	return realHmGet(real, key, outValue);
}

int hmRemove(hashMap handle, const void* key) {
	RealHashMap* real = resolveHandle(handle);
	if (!real) return HM_NULL;
	return realHmRemove(real, key);
}

int hmContains(hashMap handle, const void* key) {
	RealHashMap* real = resolveHandle(handle);
	if (!real) return HM_NULL;
	return realHmContains(real, key);
}

int hmClear(hashMap handle) {
	RealHashMap* real = resolveHandle(handle);
	if (!real) return HM_NULL;
	return realHmClear(real);
}

size_t hmGetCount(hashMap handle) {
	RealHashMap* real = resolveHandle(handle);
	if (!real) return 0;
	return real->elementCount;
}

size_t hmGetBucketCount(hashMap handle) {
	RealHashMap* real = resolveHandle(handle);
	if (!real) return 0;
	return real->bucketCount;
}

int hmGetStatus(hashMap handle) {
	RealHashMap* real = resolveHandle(handle);
	if (!real) return HM_NULL;
	return real->status;
}

// ============================================================
// char* 内容哈希和比较
// ============================================================

size_t hashCString(const void* key, size_t keySize) {
	(void)keySize;
	const char* str = *(const char**)key;
	if (!str) return 0;
	return fnv1a(str, strlen(str));
}

int compareCString(const void* keyA, const void* keyB, size_t keySize) {
	(void)keySize;
	const char* strA = *(const char**)keyA;
	const char* strB = *(const char**)keyB;
	if (strA == strB) return 0;
	if (!strA) return -1;
	if (!strB) return 1;
	return strcmp(strA, strB);
}

// ============================================================
// 状态转字符串
// ============================================================

const char* hmStatusToArray(int status) {
	switch (status) {
	case HM_NULL:				return "null hashMap handle";
	case HM_AVAILABLE:			return "available hashMap";
	case HM_INVALIDSIZE:		return "invalid key size or value size";
	case HM_WRONGSTATUS:		return "hashMap status in error";
	case HM_NULLKEY:			return "null key";
	case HM_NULLVALUE:			return "null value";
	case HM_NULLOUTVALUE:		return "null output value";
	case HM_KEYNOTFOUND:		return "key not found";

	case HM_EXPANDFAILED:        return "expand failed";
	case HM_SHRINKFAILED:        return "shrink failed";
	case HM_IDENTIFIEREXHAUSTED: return "identifier exhausted";

	case HM_REALLOCFAIL:         return "fail to realloc";
	case HM_MALLOCFAIL:          return "fail to malloc";
	case HM_SUCCESSFULOP:        return "successful operation";
	default:                  return "unknown status";
	}
}