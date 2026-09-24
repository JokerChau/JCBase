#include "JC_ArrayList.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct ArrayList {
	void* data;				// 数据内容
	size_t alCount;			// 当前数量
	size_t alMaxCount;		// 最大容量
	size_t typeSize;		// 类型大小
	int status;				// 当前AL状态
}ArrayList;

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

// 构造函数
ArrayList* alCreate(size_t typeSize) {	//传入类型大小
	// 为实例申请内存
	ArrayList* list = (ArrayList*)malloc(sizeof(ArrayList));
	// 申请失败
	if (!list)return NULL;

	// 类型大小初始化，其余成员置空
	list->data = NULL;
	list->alCount = 0;
	list->alMaxCount = 0;
	list->typeSize = typeSize;
	// 数据类型无效，状态设置为非法类型AL_INVALIDTYPESIZE
	if (!typeSize) {
		list->status = AL_INVALIDTYPESIZE;
		return list;
	}
	list->status = AL_AVAILABLE;

	return list;
}

// 析构函数
void alDestroy(ArrayList* list) {
	// 如果本来该实例为NULL则已经置空，释放
	if (!list)return;

	// 如果数据存在则释放并指向NULL
	if (list->data) {
		free(list->data);
		list->data = NULL;
	}
	free(list);
}

// 获取实例状态
int alGetStatus(const ArrayList* const list) {
	// 如果实例为NULL，则返回空实例状态AL_NULL
	if (!list)return AL_NULL;

	return list->status;
}

// 获取实例数量
size_t alGetCount(const ArrayList* const list) {
	// NULL实例被认为没有成员
	if (!list)return 0;

	return list->alCount;
}

size_t alGetMaxCount(const ArrayList* const list) {
	if (!list)return 0;
	return list->alMaxCount;
}

int alSetMaxCount(ArrayList* const list, size_t count) {
	if (!list)return AL_NULL;
	if (list->status != AL_AVAILABLE)return AL_WRONGSTATUS;
	if (count < list->alCount)return AL_CAPACITYLESSTHANMIN;
	if (count == list->alMaxCount)return AL_SUCCESSFULOP;
	if (count == 0 && list->alCount == 0) {
		free(list->data);
		list->data = NULL;
		list->alMaxCount = 0;
		return AL_SUCCESSFULOP;
	}
	if (count > SIZE_MAX / list->typeSize)return AL_REALLOCSIZEOUTBOUNDS;

	void* newData = realloc(list->data, count * list->typeSize);
	if (!newData)return AL_REALLOCFAIL;
	list->data = newData;
	list->alMaxCount = count;

	return AL_SUCCESSFULOP;
}

int alShrinkToFit(ArrayList* const list) {
	if (!list)return AL_NULL;
	if (list->status != AL_AVAILABLE)return AL_WRONGSTATUS;

	if (list->alCount == 0) {
		free(list->data);
		list->data = NULL;
		list->alMaxCount = 0;
		return AL_SUCCESSFULOP;
	}

	if (list->alMaxCount == list->alCount)return AL_SUCCESSFULOP;
	if (list->alCount > SIZE_MAX / list->typeSize) return AL_REALLOCSIZEOUTBOUNDS;

	void* newData = realloc(list->data, list->alCount * list->typeSize);
	if (!newData)return AL_REALLOCFAIL;

	list->data = newData;
	list->alMaxCount = list->alCount;
	return AL_SUCCESSFULOP;
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

const char* alStatusToArray(int status) {
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