#pragma once

#include "wheel_ex.h"
#include <stddef.h>
#include "JC_ArrayListSrc.h"

/*
* Design of ArrayList:
* 1. API rules of name: jcal_<function name>, the crazy branch should use name: jccal_<function name>, the callback branch should use name: jclbkal_<function name>
* 2. Part ArrayList: 
* 
*	typedef struct JC_ArrayList {
*		void* data;				// 数据内容
*		size_t alCount;			// 当前数量
*		size_t alMaxCount;		// 最大容量
*		size_t typeSize;		// 类型大小
*		int status;				// 当前AL状态
*	} JC_ArrayList;
* 
*	typedef JC_ArrayList jc_ArrayList;
* 
* 3. Part of callback ArrayList:
* 
*	typedef struct JC_callbackArrayList JC_callbackArrayList;
*	typedef JC_callbackArrayList* jc_clbkArrayList;
*	typedef struct JC_callbackcore JC_callbackcore;
* 
*	typedef struct JC_callbackcore {
*		jc_clbkArrayList (*jcclbkal_inner_clone)(const jc_clbkArrayList list);
*		jc_clbkArrayList (*jcclbkal_inner_delete)(jc_clbkArrayList list);
*	} JC_callbackcore;
* 
*	typedef struct JC_callbackArrayList {
*		void* data;
* 		size_t alCount;
* 		size_t alMaxCount;
* 		size_t typeSize;
* 		int status;
*		JC_callbackcore callbackcore;
*	} JC_callbackArrayList;
* 
*	4. Part of crazy ArrayList:
*		
*	typedef struct JC_crazyArrayList {
* 		void* data;
* 		size_t alCount;
* 		size_t alMaxCount;
* 		size_t typeSize;
*	} JC_crazyArrayList;
* 
*	typedef JC_crazyArrayList* jc_crazyArrayList;
* 
*	5. Rules of naming status:
*	
*		Take ArrayList as an example, the status of the ArrayList is defined as follows:
*		JC_AL__NULL = -2,					// NULL AL实例
*		Take crazy ArrayList as an example, the status of the crazy ArrayList is defined as follows:
*		JC_CAl__NULL = -2,					// NULL CAL实例
*     
*       JC_ArrayListStatus as the name of JC_ArrayList
*	
*	6. About file name:
*		Take ArrayList as an example, the file name is JC_ArrayList.h, the source file name is JC_ArrayList.c, and the header file for the status definition is JC_ArrayListSrc.h.
*/

// define types
struct JC_ArrayList;
typedef struct JC_ArrayList JC_ArrayList;
typedef JC_ArrayList* jc_ArrayList;

wheels jc_ArrayList jcal_inner_new(size_t sizeType);

// recommended to use the macro jcal_newOf(type) to create an ArrayList instance, which is equivalent to calling jcal_inner_new(sizeof(type))
#define jcal_newOf(type) \
    (_Static_assert(sizeof(type) > 0, "size must > 0"), \
     _Static_assert(!_Generic((type*)0, void*: 1, default: 0), "Error: type cannot be void!"), \
     jcal_inner_new(sizeof(type)))

wheels void jcal_delete(jc_ArrayList list);

wheels size_t jcal_getLength(const jc_ArrayList list);

wheels size_t jcal_getMaxLength(const jc_ArrayList list);

wheels int jcal_getStatus(const jc_ArrayList list);

wheels int jcal_setMaxLength(jc_ArrayList list, size_t count);

wheels int jcal_clear(jc_ArrayList list);

wheels int jcal_shrinkToFit(const jc_ArrayList list);

typedef struct ArrayList ArrayList;

typedef ArrayList* arrayList;

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

wheels const char* alStatusToCharArray(int status);