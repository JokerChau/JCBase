#pragma once

#include "wheel_ex.h"
#include <stddef.h>
#include "ArrayListSrc.h"

#define newArrayList(pointer) alCreate(sizeof(*(pointer)))

typedef struct ArrayList ArrayList;

typedef ArrayList* arrayList;

wheels ArrayList* alCreate(size_t typeSize);

wheels void alDestroy(ArrayList* list);

wheels int alGetStatus(const ArrayList* const list);

wheels size_t alGetCount(const ArrayList* const list);

wheels size_t alGetMaxCount(const ArrayList* const list);

wheels int alSetMaxCount(ArrayList* const list, size_t count);

wheels int alShrinkToFit(ArrayList* const list);

wheels int alGetAt(const ArrayList* const list, size_t index, void* ele);

wheels int alSetAt(ArrayList* const list, size_t index, const void* const content);

wheels int alPushBack(ArrayList* const list, const void* const content);

wheels int alPushFront(ArrayList* const list, const void* const content);

wheels int alInsertAt(ArrayList* const list, const void* const content, size_t index);

wheels int alPopBack(ArrayList* const list);

wheels int alPopFront(ArrayList* const list);

wheels int alRemoveAt(ArrayList* const list, size_t index);

wheels int alClear(ArrayList* const list);

wheels const char* alStatusToArray(int status);