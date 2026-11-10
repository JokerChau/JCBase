#include "JC_ArrayList.h"
#include "JC_ArrayListStructures.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// enable
static jc_ArrayList wrapAl(void* data, size_t count, size_t maxCount, size_t typeSize, int status) {
	jc_ArrayList list = (jc_ArrayList)malloc(sizeof(JC_ArrayList));
	if (!list)return NULL;
	list->data = data;
	list->count = count;
	list->maxCount = maxCount;
	list->typeSize = typeSize;
	list->status = status;
	return list;
}

static int alRealloc(ArrayList* list) {
	if (list->alCount >= list->alMaxCount) {
		// 设置临时容量，如果最大容量为0则设为4，且每次加4
		size_t newCap;
		size_t newMaxCap = SIZE_MAX / list->typeSize;
		if (list->alMaxCount == 0) {
			if (newMaxCap >= 4)newCap = 4;
			else newCap = newMaxCap;
		}
		else if (list->alMaxCount >= newMaxCap) {
			// 已经到达理论最大容量，无法再涨
			return AL_REALLOCSIZEOUTBOUNDS;
		}
		else if (list->alMaxCount > newMaxCap / 2) {
			// 翻倍会超过 maxCapacity，直接拉到 maxCapacity
			newCap = newMaxCap;
		}
		else {
			newCap = list->alMaxCount * 2;
		}
		// 重分配一个数据指针，大小为临时容量*类型大小
		void* newData = realloc(list->data, newCap * list->typeSize);
		if (!newData)return AL_REALLOCFAIL;
		// 分配成功，更新数据
		list->data = newData;
		list->alMaxCount = newCap;
		return AL_SUCCESSFULOP;
	}
	else return AL_SUCCESSFULOP;
}

// enable
jc_ArrayList jcal_inner_new(size_t sizeType) {
	int status = JC_AL__AVAILABLE;
	if (!sizeType) {
		status = JC_AL__INVALID_TYPE_SIZE;
	}
	return wrapAl(NULL, 0, 0, sizeType, status);
}

// enable
void jcal_delete(jc_ArrayList list) {
	if (!list)return;
	if (list->data)free(list->data);
	free(list);
}

// enable
size_t jcal_getLength(const jc_ArrayList list) {
	if (!list)return 0;
	if (!list->data)return 0;
	return list->count;
}

// enable
size_t jcal_getMaxLength(const jc_ArrayList list) {
	if (!list)return 0;
	return list->maxCount;
}

// enable
int jcal_getStatus(const jc_ArrayList list) {
	if (!list)return JC_AL__NULL;
	return list->status;
}

// enable
int jcal_setMaxLength(jc_ArrayList list, size_t count) {
	if (!list)return JC_AL__NULL;
	if (list->status != JC_AL__AVAILABLE)return list->status;
	// 如果新的最大长度小于当前元素数量，返回错误
	if (count < list->count)return JC_AL__CAPACITY_LESS_THAN_MIN;
	// 如果新的最大长度等于当前最大长度，直接返回成功
	if (count == list->maxCount)return JC_AL__SUCCESSFUL_OP;
	// 其余情况均为重新分配内存

	if (!count) {
		free(list->data);
		list->data = NULL;
		list->maxCount = count;
		return JC_AL__SUCCESSFUL_OP;
	}

	// 如果新的最大长度*类型大小大于SIZE_MAX，返回错误
	if (count > SIZE_MAX / list->typeSize)return JC_AL__COUNT_TOO_LARGE;

	void* newData = realloc(list->data, count * list->typeSize);
	if (!newData)return JC_AL__REALLOCATION_FAILED;
	list->data = newData;
	list->maxCount = count;
	return JC_AL__SUCCESSFUL_OP;
}

// enable
int jcal_clear(jc_ArrayList list) {
	if (!list)return JC_AL__NULL;
	if (list->status != JC_AL__AVAILABLE)return list->status;
	list->count = 0;
	return JC_AL__SUCCESSFUL_OP;
}

// enable
int jcal_shrinkToFit(const jc_ArrayList list) {
	if (!list)return JC_AL__NULL;
	if (list->data != JC_AL__AVAILABLE)return list->status;

	if (list->count == list->maxCount)return JC_AL__SUCCESSFUL_OP;

	if (!list->count) {
		free(list->data);
		list->data = NULL;
		list->maxCount = 0;
		return JC_AL__SUCCESSFUL_OP;
	}

	if (list->count > SIZE_MAX / list->typeSize) return JC_AL__COUNT_TOO_LARGE;

	void* newData = realloc(list->data, list->count * list->typeSize);
	if (!newData)return JC_AL__REALLOCATION_FAILED;
	list->data = newData;
	list->maxCount = list->count;
	return JC_AL__SUCCESSFUL_OP;
}

int alGetAt(const ArrayList* const list, size_t index, void* ele) {
	if (!list)return AL_NULL;
	if (list->status != AL_AVAILABLE)return AL_WRONGSTATUS;
	if(!ele)return AL_NULLELEMENT;
	
	if (index >= list->alCount)return AL_IDXOUTBOUNDS;

	char* tempData = (char*)list->data + index * list->typeSize;
	memcpy(ele, tempData, list->typeSize);
	
	return AL_SUCCESSFULOP;
}

int alSetAt(ArrayList* const list, size_t index, const void* const content) {
	if (!list)return AL_NULL;
	if (list->status != AL_AVAILABLE)return AL_WRONGSTATUS;
	if (!content)return AL_NULLELEMENT;

	if (index >= list->alCount)return AL_IDXOUTBOUNDS;

	memcpy((char*)list->data + index * list->typeSize, content, list->typeSize);

	return AL_SUCCESSFULOP;
}

// 从后面补上单个元素
int alPushBack(ArrayList* const list, const void* const content) {
	if (!list)return AL_NULL;
	return alInsertAt(list, content, list->alCount);
}

int alPushFront(ArrayList* const list, const void* const content) {
	return alInsertAt(list, content, 0);
}

int alInsertAt(ArrayList* const list, const void* const content, size_t index) {
	if (!list)return AL_NULL;
	if (list->status != AL_AVAILABLE)return AL_WRONGSTATUS;
	if (!content)return AL_NULLELEMENT;

	if (index > list->alCount)return AL_IDXOUTBOUNDS;

	int tempStatus = alRealloc(list);
	if (tempStatus != AL_SUCCESSFULOP)return tempStatus;

	if (index == list->alCount) {
		char* dest = (char*)list->data + list->alCount * list->typeSize;
		memcpy(dest, content, list->typeSize);
	}
	else {
		char* dest = (char*)list->data + list->typeSize * (index + 1);
		char* src = (char*)list->data + list->typeSize * index;
		size_t n = (list->alCount - index) * list->typeSize;
		memmove(dest, src, n);

		memcpy((char*)list->data + list->typeSize * index, content, list->typeSize);
	}
	list->alCount++;

	return AL_SUCCESSFULOP;
}

int alPopBack(ArrayList* const list) {
	if (!list) return AL_NULL;
	if (list->status != AL_AVAILABLE) return AL_WRONGSTATUS;
	if (list->alCount == 0)return AL_ALREADYEMPTY;
	return alRemoveAt(list, list->alCount - 1);
}

int alPopFront(ArrayList* const list) {
	if (!list) return AL_NULL;
	if (list->status != AL_AVAILABLE) return AL_WRONGSTATUS;
	if (list->alCount == 0)return AL_ALREADYEMPTY;
	return alRemoveAt(list, 0);
}

int alRemoveAt(ArrayList* const list, size_t index) {
	if (!list)return AL_NULL;
	if (list->status != AL_AVAILABLE)return AL_WRONGSTATUS;
	if (list->alCount == 0) return AL_ALREADYEMPTY;
	if (index >= list->alCount)return AL_IDXOUTBOUNDS;

	if (index < list->alCount - 1) {
		char* dest = (char*)list->data + list->typeSize * index;
		char* src=(char*)list->data + list->typeSize * (index + 1);
		size_t n = (list->alCount - index - 1) * list->typeSize;
		memmove(dest, src, n);
	}
	list->alCount--;

	return AL_SUCCESSFULOP;
}

int alClear(ArrayList* const list) {
	if (!list)return AL_NULL;
	if (list->status != AL_AVAILABLE)return AL_WRONGSTATUS;
	list->alCount = 0;
	return AL_SUCCESSFULOP;
}

const char* alStatusToCharArray(int status) {
	switch (status) {
	case AL_NULL: {
		return "null ArrayList*";
	}
	case AL_AVAILABLE: {
		return "available ArrayList*";
	}
	case AL_INVALIDTYPESIZE: {
		return "invalid type size of data";
	}
	case AL_ALREADYEMPTY: {
		return "ArrayList* has already been empty";
	}

	case AL_SUCCESSFULOP: {
		return "successful operation";
	}
	case AL_WRONGSTATUS: {
		return "ArrayList* status in error";
	}
	case AL_REALLOCFAIL: {
		return "fail to realloc";
	}

	case AL_NULLELEMENT: {
		return "null element";
	}
	case AL_IDXOUTBOUNDS: {
		return "index out of bounds";
	}
	case AL_CAPACITYLESSTHANMIN: {
		return "setted capacity is less than the current capacity";
	}
	case AL_REALLOCSIZEOUTBOUNDS: {
		return "the size is bigger than SIZE_MAX/max size of size_t";
	}
	default: {
		return "unknown status";
	}
	}
}