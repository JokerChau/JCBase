#include "RegistryCore.h"
#include <stdlib.h>
#include <string.h>

#define INITIAL_BUCKET_COUNT 16
#define MIN_BUCKET_COUNT 16

/* ============================================================
 * 泛型哈希表
 * ============================================================ */

typedef struct HashMapNode {
    void* key;
    void* value;
    struct HashMapNode* next;
} HashMapNode;

struct RealHashMap {
    HashMapNode** buckets;
    size_t bucketCount;
    size_t elementCount;
    size_t keySize;
    size_t valueSize;
    realHashFunction hash;
    rhmCompareFunction compare;
    int status;
};

static size_t fnv1a(const void* data, size_t size) {
    const unsigned char* bytes = (const unsigned char*)data;
    size_t hash = 14695981039346656037ULL;
    for (size_t i = 0; i < size; ++i) {
        hash ^= bytes[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

static int realHmResize(RealHashMap* map, size_t newBucketCount) {
    if (newBucketCount < MIN_BUCKET_COUNT) newBucketCount = MIN_BUCKET_COUNT;

    HashMapNode** newBuckets = (HashMapNode**)calloc(newBucketCount, sizeof(HashMapNode*));
    if (!newBuckets) return RHM_MALLOCFAIL;

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
    return RHM_SUCCESSFULOP;
}

RealHashMap* realHmCreate(size_t keySize, size_t valueSize,
    realHashFunction hash, rhmCompareFunction compare) {

    RealHashMap* map = (RealHashMap*)malloc(sizeof(RealHashMap));
    if (!map) return NULL;

    map->bucketCount = INITIAL_BUCKET_COUNT;
    map->elementCount = 0;
    map->keySize = keySize;
    map->valueSize = valueSize;
    map->hash = hash ? hash : fnv1a;
    map->compare = compare ? compare : memcmp;
    map->status = 0;

    map->buckets = (HashMapNode**)calloc(map->bucketCount, sizeof(HashMapNode*));
    if (!map->buckets) {
        free(map);
        return NULL;
    }
    return map;
}

void realHmDestroy(RealHashMap* map) {
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

int realHmPut(RealHashMap* map, const void* key, const void* value) {
    if (!map) return RHM_NULL;
    if (map->status != 0) return RHM_WRONGSTATUS;
    if (!key) return RHM_NULLKEY;
    if (!value) return RHM_NULLVALUE;

    size_t h = map->hash(key, map->keySize);
    size_t bucket = h & (map->bucketCount - 1);

    HashMapNode* node = map->buckets[bucket];
    while (node) {
        if (map->compare(node->key, key, map->keySize) == 0) {
            void* newValue = malloc(map->valueSize);
            if (!newValue) return RHM_MALLOCFAIL;
            memcpy(newValue, value, map->valueSize);
            free(node->value);
            node->value = newValue;
            return RHM_SUCCESSFULOP;
        }
        node = node->next;
    }

    HashMapNode* newNode = (HashMapNode*)malloc(sizeof(HashMapNode));
    if (!newNode) return RHM_MALLOCFAIL;

    newNode->key = malloc(map->keySize);
    if (!newNode->key) {
        free(newNode);
        return RHM_MALLOCFAIL;
    }

    newNode->value = malloc(map->valueSize);
    if (!newNode->value) {
        free(newNode->key);
        free(newNode);
        return RHM_MALLOCFAIL;
    }

    memcpy(newNode->key, key, map->keySize);
    memcpy(newNode->value, value, map->valueSize);

    newNode->next = map->buckets[bucket];
    map->buckets[bucket] = newNode;
    map->elementCount++;

    if (map->elementCount * 4 > map->bucketCount * 3) {
        int resizeStatus = realHmResize(map, map->bucketCount * 2);
        if (resizeStatus != RHM_SUCCESSFULOP) {
            return RHM_EXPANDFAILED;
        }
    }

    return RHM_SUCCESSFULOP;
}

int realHmGet(RealHashMap* map, const void* key, void* outValue) {
    if (!map) return RHM_NULL;
    if (map->status != 0) return RHM_WRONGSTATUS;
    if (!key) return RHM_NULLKEY;
    if (!outValue) return RHM_NULLOUTVALUE;

    size_t h = map->hash(key, map->keySize);
    size_t bucket = h & (map->bucketCount - 1);

    HashMapNode* node = map->buckets[bucket];
    while (node) {
        if (map->compare(node->key, key, map->keySize) == 0) {
            memcpy(outValue, node->value, map->valueSize);
            return RHM_SUCCESSFULOP;
        }
        node = node->next;
    }
    return RHM_KEYNOTFOUND;
}

int realHmRemove(RealHashMap* map, const void* key, void* outValue) {
    if (!map) return RHM_NULL;
    if (map->status != 0) return RHM_WRONGSTATUS;
    if (!key) return RHM_NULLKEY;

    size_t h = map->hash(key, map->keySize);
    size_t bucket = h & (map->bucketCount - 1);

    HashMapNode** link = &map->buckets[bucket];
    while (*link) {
        HashMapNode* node = *link;
        if (map->compare(node->key, key, map->keySize) == 0) {
            if (outValue) {
                memcpy(outValue, node->value, map->valueSize);
            }
            *link = node->next;
            free(node->key);
            free(node->value);
            free(node);
            map->elementCount--;
            return RHM_SUCCESSFULOP;
        }
        link = &node->next;
    }
    return RHM_KEYNOTFOUND;
}

int realHmContains(RealHashMap* map, const void* key) {
    if (!map) return RHM_NULL;
    if (map->status != 0) return RHM_WRONGSTATUS;
    if (!key) return RHM_NULLKEY;

    size_t h = map->hash(key, map->keySize);
    size_t bucket = h & (map->bucketCount - 1);

    HashMapNode* node = map->buckets[bucket];
    while (node) {
        if (map->compare(node->key, key, map->keySize) == 0) {
            return RHM_SUCCESSFULOP;
        }
        node = node->next;
    }
    return RHM_KEYNOTFOUND;
}

int realHmClear(RealHashMap* map) {
    if (!map) return RHM_NULL;

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
    return RHM_SUCCESSFULOP;
}

size_t realHmGetCount(const RealHashMap* map) {
    if (!map) return 0;
    return map->elementCount;
}

size_t realHmGetBucketCount(const RealHashMap* map) {
    if (!map) return 0;
    return map->bucketCount;
}

int realHmGetStatus(const RealHashMap* map) {
    if (!map) return RHM_NULL;
    return map->status;
}

int realHmSetStatus(RealHashMap* map, int status) {
    if (!map) return RHM_NULL;
    map->status = status;
    return RHM_SUCCESSFULOP;
}

/* ============================================================
 * slot 管理
 * ============================================================ */

typedef struct FreeSlot {
    uint64_t slot;
    uint64_t nextGen;
} FreeSlot;

struct RegistryCore {
    RealHashMap* map;
    uint64_t nextSlot;
    FreeSlot* freeSlots;
    size_t freeTop;
    size_t freeCapacity;
};

static bool freeSlotsPush(RegistryCore* core, uint64_t slot, uint64_t nextGen) {
    if (core->freeTop == core->freeCapacity) {
        size_t newCap = core->freeCapacity == 0 ? 16 : core->freeCapacity * 2;
        if (newCap > SIZE_MAX / sizeof(FreeSlot)) return false;
        FreeSlot* newArr = (FreeSlot*)realloc(core->freeSlots, newCap * sizeof(FreeSlot));
        if (!newArr) return false;
        core->freeSlots = newArr;
        core->freeCapacity = newCap;
    }
    core->freeSlots[core->freeTop].slot = slot;
    core->freeSlots[core->freeTop].nextGen = nextGen;
    core->freeTop++;
    return true;
}

static bool freeSlotsPop(RegistryCore* core, uint64_t* slot, uint64_t* gen) {
    if (core->freeTop == 0) return false;
    core->freeTop--;
    *slot = core->freeSlots[core->freeTop].slot;
    *gen = core->freeSlots[core->freeTop].nextGen;
    return true;
}

RegistryCore* registryCoreCreate(void) {
    RegistryCore* core = (RegistryCore*)malloc(sizeof(RegistryCore));
    if (!core) return NULL;

    core->map = realHmCreate(sizeof(Handle), sizeof(void*), NULL, NULL);
    if (!core->map) {
        free(core);
        return NULL;
    }

    core->nextSlot = 1;
    core->freeSlots = NULL;
    core->freeTop = 0;
    core->freeCapacity = 0;

    return core;
}

void registryCoreDestroy(RegistryCore* core) {
    if (!core) return;
    if (core->map) {
        realHmDestroy(core->map);
        core->map = NULL;
    }
    free(core->freeSlots);
    core->freeSlots = NULL;
    core->freeTop = 0;
    core->freeCapacity = 0;
    free(core);
}

int registryCoreInsert(RegistryCore* core, void* value, Handle* out) {
    if (!core || !out) return REG_NULL;

    uint64_t slot;
    uint64_t gen;

    if (!freeSlotsPop(core, &slot, &gen)) {
        if (core->nextSlot == UINT64_MAX) return REG_IDENTIFIEREXHAUSTED;
        slot = core->nextSlot++;
        gen = 1;
    }

    Handle h = { slot, gen };
    int status = realHmPut(core->map, &h, &value);
    if (status != RHM_SUCCESSFULOP && status != RHM_EXPANDFAILED) {
        freeSlotsPush(core, slot, gen);
        return status == RHM_MALLOCFAIL ? REG_MALLOCFAIL : REG_NULL;
    }

    *out = h;
    return REG_SUCCESSFULOP;
}

void* registryCoreResolve(RegistryCore* core, Handle handle) {
    if (!core || handle.slot == 0) return NULL;

    void* value = NULL;
    if (realHmGet(core->map, &handle, &value) != RHM_SUCCESSFULOP) {
        return NULL;
    }
    return value;
}

int registryCoreRemove(RegistryCore* core, Handle handle, void** outValue) {
    if (!core) return REG_NULL;
    if (handle.slot == 0) return REG_KEYNOTFOUND;

    void* value = NULL;
    int status = realHmRemove(core->map, &handle, &value);
    if (status != RHM_SUCCESSFULOP) {
        return status == RHM_KEYNOTFOUND ? REG_KEYNOTFOUND : REG_NULL;
    }

    freeSlotsPush(core, handle.slot, handle.generation + 1);

    if (outValue) *outValue = value;
    return REG_SUCCESSFULOP;
}

void registryCoreForEach(RegistryCore* core, RegistryValueDestructor fn, void* userData) {
    if (!core || !fn) return;

    RealHashMap* map = core->map;
    for (size_t i = 0; i < map->bucketCount; ++i) {
        HashMapNode* node = map->buckets[i];
        while (node) {
            HashMapNode* next = node->next;
            void* value = *(void**)node->value;
            fn(value, userData);
            node = next;
        }
    }
}