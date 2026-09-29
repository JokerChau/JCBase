#include "JC_HashMap.h"
#include "RegistryCore.h"
#include <stdlib.h>
#include <string.h>

/* ============================================================
 * HashMapRegistry：每个 HashMap 注册表持有一个 RegistryCore
 * ============================================================ */

struct HashMapRegistry {
    RegistryCore* core;
    int lastCreateStatus;
};

/* 把 RegistryCore 的哈希表状态码翻译成 HashMap 的对外状态码 */
static int translateRhmStatus(int rhmStatus) {
    switch (rhmStatus) {
    case RHM_SUCCESSFULOP:  return HM_SUCCESSFULOP;
    case RHM_NULL:          return HM_NULL;
    case RHM_MALLOCFAIL:    return HM_MALLOCFAIL;
    case RHM_NULLKEY:       return HM_NULLKEY;
    case RHM_NULLVALUE:     return HM_NULLVALUE;
    case RHM_NULLOUTVALUE:  return HM_NULLOUTVALUE;
    case RHM_KEYNOTFOUND:   return HM_KEYNOTFOUND;
    case RHM_EXPANDFAILED:  return HM_EXPANDFAILED;
    case RHM_WRONGSTATUS:   return HM_WRONGSTATUS;
    default:                return HM_WRONGSTATUS;
    }
}

/* ============================================================
 * 注册表生命周期
 * ============================================================ */

static void destroyRealHashMapValue(void* value, void* userData) {
    (void)userData;
    realHmDestroy((RealHashMap*)value);
}

HashMapRegistry* hmRegistryCreate(void) {
    HashMapRegistry* reg = (HashMapRegistry*)malloc(sizeof(HashMapRegistry));
    if (!reg) return NULL;

    reg->core = registryCoreCreate();
    if (!reg->core) {
        free(reg);
        return NULL;
    }
    reg->lastCreateStatus = HM_SUCCESSFULOP;
    return reg;
}

void hmRegistryDestroy(HashMapRegistry* reg) {
    if (!reg) return;
    if (reg->core) {
        /* 兜底：销毁所有残留的 map */
        registryCoreForEach(reg->core, destroyRealHashMapValue, NULL);
        registryCoreDestroy(reg->core);
        reg->core = NULL;
    }
    free(reg);
}

/* ============================================================
 * 句柄解析
 * ============================================================ */

static RealHashMap* resolveHandle(hashMap handle) {
    if (!handle.reg || handle.slot == 0) return NULL;
    Handle h = { handle.slot, handle.generation };
    return (RealHashMap*)registryCoreResolve(handle.reg->core, h);
}

/* ============================================================
 * 对外接口
 * ============================================================ */

hashMap hmNew(HashMapRegistry* reg, size_t keySize, size_t valueSize,
    hashFunction hash, compareFunction compare) {
    hashMap invalid = { NULL, 0, 0 };

    if (!reg) return invalid;
    reg->lastCreateStatus = HM_SUCCESSFULOP;

    if (keySize == 0 || valueSize == 0) {
        reg->lastCreateStatus = HM_INVALIDSIZE;
        return invalid;
    }

    RealHashMap* real = realHmCreate(keySize, valueSize, (realHashFunction)hash, (rhmCompareFunction)compare);
    if (!real) {
        reg->lastCreateStatus = HM_MALLOCFAIL;
        return invalid;
    }

    Handle h;
    int regStatus = registryCoreInsert(reg->core, real, &h);
    if (regStatus != REG_SUCCESSFULOP) {
        realHmDestroy(real);
        reg->lastCreateStatus = (regStatus == REG_MALLOCFAIL)
            ? HM_MALLOCFAIL
            : HM_IDENTIFIEREXHAUSTED;
        return invalid;
    }

    hashMap handle = { reg, h.slot, h.generation };
    reg->lastCreateStatus = HM_SUCCESSFULOP;
    return handle;
}

int hmGetLastCreateStatus(HashMapRegistry* reg) {
    if (!reg) return HM_NULL;
    return reg->lastCreateStatus;
}

hashMapStatusReport hmGetStatusReport(hashMap handle) {
    hashMapStatusReport report;
    report.valid = hmIsValid(handle);
    report.instanceStatus = hmGetStatus(handle);
    return report;
}

void hmDestroy(hashMap* handle) {
    if (!handle) return;
    if (handle->slot == 0 || !handle->reg) return;

    HashMapRegistry* reg = handle->reg;
    Handle h = { handle->slot, handle->generation };
    handle->reg = NULL;
    handle->slot = 0;
    handle->generation = 0;

    RealHashMap* real = NULL;
    int status = registryCoreRemove(reg->core, h, (void**)&real);
    if (status == REG_SUCCESSFULOP && real) {
        realHmDestroy(real);
    }
}

int hmPut(hashMap handle, const void* key, const void* value) {
    RealHashMap* real = resolveHandle(handle);
    if (!real) return HM_NULL;
    return translateRhmStatus(realHmPut(real, key, value));
}

int hmGet(hashMap handle, const void* key, void* outValue) {
    RealHashMap* real = resolveHandle(handle);
    if (!real) return HM_NULL;
    return translateRhmStatus(realHmGet(real, key, outValue));
}

int hmRemove(hashMap handle, const void* key) {
    RealHashMap* real = resolveHandle(handle);
    if (!real) return HM_NULL;
    return translateRhmStatus(realHmRemove(real, key, NULL));
}

int hmContains(hashMap handle, const void* key) {
    RealHashMap* real = resolveHandle(handle);
    if (!real) return HM_NULL;
    return translateRhmStatus(realHmContains(real, key));
}

int hmClear(hashMap handle) {
    RealHashMap* real = resolveHandle(handle);
    if (!real) return HM_NULL;
    return translateRhmStatus(realHmClear(real));
}

size_t hmGetCount(hashMap handle) {
    RealHashMap* real = resolveHandle(handle);
    if (!real) return 0;
    return realHmGetCount(real);
}

size_t hmGetBucketCount(hashMap handle) {
    RealHashMap* real = resolveHandle(handle);
    if (!real) return 0;
    return realHmGetBucketCount(real);
}

int hmGetStatus(hashMap handle) {
    RealHashMap* real = resolveHandle(handle);
    if (!real) return HM_NULL;
    return realHmGetStatus(real) == 0 ? HM_AVAILABLE : HM_WRONGSTATUS;
}

/* ============================================================
 * char* 内容哈希和比较（hmCreate 宏用到）
 * ============================================================ */

static size_t fnv1aForCString(const void* data, size_t size) {
    const unsigned char* bytes = (const unsigned char*)data;
    size_t hash = 14695981039346656037ULL;
    for (size_t i = 0; i < size; ++i) {
        hash ^= bytes[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

size_t hashCString(const void* key, size_t keySize) {
    (void)keySize;
    const char* str = *(const char**)key;
    if (!str) return 0;
    return fnv1aForCString(str, strlen(str));
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

/* ============================================================
 * 状态转字符串
 * ============================================================ */

const char* hmStatusToCharArray(int status) {
    switch (status) {
    case HM_NULL:                return "null hashMap handle";
    case HM_AVAILABLE:           return "available hashMap";
    case HM_INVALIDSIZE:         return "invalid key size or value size";
    case HM_WRONGSTATUS:         return "hashMap status in error";
    case HM_NULLKEY:             return "null key";
    case HM_NULLVALUE:           return "null value";
    case HM_NULLOUTVALUE:        return "null output value";
    case HM_KEYNOTFOUND:         return "key not found";

    case HM_EXPANDFAILED:        return "expand failed";
    case HM_SHRINKFAILED:        return "shrink failed";
    case HM_IDENTIFIEREXHAUSTED: return "identifier exhausted";

    case HM_REALLOCFAIL:         return "fail to realloc";
    case HM_MALLOCFAIL:          return "fail to malloc";
    case HM_SUCCESSFULOP:        return "successful operation";
    default:                     return "unknown status";
    }
}