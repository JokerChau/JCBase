#pragma once

#include "wheel_ex.h"
#include <stddef.h>
#include "JC_ArrayListSrc.h"


// define types
struct JC_ArrayList;
typedef struct JC_ArrayList JC_ArrayList;
typedef JC_ArrayList* jc_ArrayList;

wheels jc_ArrayList jcal_inner_new(size_t sizeType, unsigned int flags);

// recommended to use the macro jcal_newOf(type) to create an ArrayList instance, which is equivalent to calling jcal_inner_new(sizeof(type))
#define jcal_newOf(type) \
    (_Static_assert(sizeof(type) > 0, "size must > 0"), \
     _Static_assert(!_Generic((type*)0, void*: 1, default: 0), "Error: type cannot be void!"), \
     jcal_inner_new(sizeof(type), 0))

#define jcal_newOfWithFlags(type, flags) \
    (_Static_assert(sizeof(type) > 0, "size must > 0"), \
     _Static_assert(!_Generic((type*)0, void*: 1, default: 0), "Error: type cannot be void!"), \
     jcal_inner_new(sizeof(type), flags))

wheels unsigned int jcal_getFlags(const jc_ArrayList list);

wheels void jcal_setFlags(jc_ArrayList list, unsigned int flags);

wheels void jcal_delete(jc_ArrayList list);

wheels size_t jcal_getLength(const jc_ArrayList list);

wheels size_t jcal_getMaxLength(const jc_ArrayList list);

wheels int jcal_getStatus(const jc_ArrayList list);

wheels int jcal_setMaxLength(jc_ArrayList list, size_t count);

wheels int jcal_clear(jc_ArrayList list);

wheels int jcal_shrinkToFit(const jc_ArrayList list);

wheels int jcal_reserve(jc_ArrayList list, size_t capacity);

wheels int jcal_getAt(const JC_ArrayList* const list, size_t index, void* ele);

wheels int jcal_setAt(JC_ArrayList* const list, size_t index, const void* const content);

wheels size_t jcal_indexOf(const JC_ArrayList* const list, void* content, size_t* receiver);

wheels int jcal_pushBack(JC_ArrayList* const list, const void* const content);

wheels int jcal_pushFront(JC_ArrayList* const list, const void* const content);

wheels int jcal_insertAt(JC_ArrayList* const list, const void* const content, size_t index);

wheels int jcal_popBack(JC_ArrayList* const list);

wheels int jcal_popFront(JC_ArrayList* const list);

wheels int jcal_removeAt(JC_ArrayList* const list, size_t index);

wheels const char* jcal_statusToCharArray(int status);