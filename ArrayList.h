#pragma once

#include "wheel_ex.h"
#include <stddef.h>

typedef struct ArrayList ArrayList;

wheels ArrayList* alCreate(size_t typeSize);

wheels void alDestroy(ArrayList* list);

wheels int getAlStatus(const ArrayList* const list);

wheels size_t getAlCount(const ArrayList* const list);

wheels int alPushBack(ArrayList* const list, const void* content);