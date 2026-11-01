#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* ============================================================
 * 第 1 层：泛型哈希表
 * ============================================================ */

typedef size_t(*realHashFunction)(const void* key, size_t keySize);
typedef int (*rhmCompareFunction)(const void* keyA, const void* keyB, size_t keySize);

typedef struct RealHashMap RealHashMap;

RealHashMap* realHmCreate(size_t keySize, size_t valueSize,
    realHashFunction hash, rhmCompareFunction compare);
void   realHmDestroy(RealHashMap* map);

int    realHmPut(RealHashMap* map, const void* key, const void* value);
int    realHmGet(RealHashMap* map, const void* key, void* outValue);
int    realHmRemove(RealHashMap* map, const void* key, void* outValue);
int    realHmContains(RealHashMap* map, const void* key);
int    realHmClear(RealHashMap* map);

size_t realHmGetCount(const RealHashMap* map);
size_t realHmGetBucketCount(const RealHashMap* map);
int    realHmGetStatus(const RealHashMap* map);
int    realHmSetStatus(RealHashMap* map, int status);

/* 哈希表状态码 */
#define RHM_NULL                -1
#define RHM_MALLOCFAIL          -2
#define RHM_NULLKEY             -3
#define RHM_NULLVALUE           -4
#define RHM_NULLOUTVALUE        -5
#define RHM_KEYNOTFOUND         -6
#define RHM_SUCCESSFULOP         0
#define RHM_EXPANDFAILED         1
#define RHM_WRONGSTATUS          2

/* ============================================================
 * 第 2 层：slot 管理（句柄注册表）
 * ============================================================ */

typedef struct Handle {
    uint64_t slot;
    uint64_t generation;
} Handle;

typedef struct RegistryCore RegistryCore;

typedef void (*RegistryValueDestructor)(void* value, void* userData);

RegistryCore* registryCoreCreate(void);
void          registryCoreDestroy(RegistryCore* core);

int    registryCoreInsert(RegistryCore* core, void* value, Handle* out);
void*  registryCoreResolve(RegistryCore* core, Handle handle);
int    registryCoreRemove(RegistryCore* core, Handle handle, void** outValue);
void   registryCoreForEach(RegistryCore* core, RegistryValueDestructor fn, void* userData);

/* slot 状态码 */
#define REG_NULL                 -1
#define REG_MALLOCFAIL           -2
#define REG_IDENTIFIEREXHAUSTED  -3
#define REG_KEYNOTFOUND          -4
#define REG_SUCCESSFULOP          0