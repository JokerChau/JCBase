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
			return REALLOCSIZEOUTBOUNDS;
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
		if (!newData)return REALLOCALFAIL;
		// 分配成功，更新数据
		list->data = newData;
		list->alMaxCount = newCap;
		return SUCCESSFULOP;
	}
	else return SUCCESSFULOP;
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
	// 数据类型无效，状态设置为非法类型INVALIDTYPESIZE
	if (!typeSize) {
		list->status = INVALIDALTYPESIZE;
		return list;
	}
	list->status = AVAILABLEAL;

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
	// 如果实例为NULL，则返回空实例状态NULLAL
	if (!list)return NULLAL;

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
	if (!list)return NULLAL;
	if (list->status != AVAILABLEAL)return WRONGALSTATUS;
	if (count < list->alCount)return CAPACITYLESSTHANMIN;
	if (count == list->alMaxCount)return SUCCESSFULOP;
	if (count == 0 && list->alCount == 0) {
		free(list->data);
		list->data = NULL;
		list->alMaxCount = 0;
		return SUCCESSFULOP;
	}
	if (count > SIZE_MAX / list->typeSize)return REALLOCSIZEOUTBOUNDS;

	void* newData = realloc(list->data, count * list->typeSize);
	if (!newData)return REALLOCALFAIL;
	list->data = newData;
	list->alMaxCount = count;

	return SUCCESSFULOP;
}

int alShrinkToFit(ArrayList* const list) {
	if (!list)return NULLAL;
	if (list->status != AVAILABLEAL)return WRONGALSTATUS;

	if (list->alCount == 0) {
		free(list->data);
		list->data = NULL;
		list->alMaxCount = 0;
		return SUCCESSFULOP;
	}

	if (list->alMaxCount == list->alCount)return SUCCESSFULOP;
	if (list->alCount > SIZE_MAX / list->typeSize) return REALLOCSIZEOUTBOUNDS;

	void* newData = realloc(list->data, list->alCount * list->typeSize);
	if (!newData)return REALLOCALFAIL;

	list->data = newData;
	list->alMaxCount = list->alCount;
	return SUCCESSFULOP;
}

int alGetAt(const ArrayList* const list, size_t index, void* ele) {
	if (!list)return NULLAL;
	if (list->status != AVAILABLEAL)return WRONGALSTATUS;
	if(!ele)return NULLALELEMENT;
	
	if (index >= list->alCount)return ALIDXOUTBOUNDS;

	char* tempData = (char*)list->data + index * list->typeSize;
	memcpy(ele, tempData, list->typeSize);
	
	return SUCCESSFULOP;
}

int alSetAt(ArrayList* const list, size_t index, const void* const content) {
	if (!list)return NULLAL;
	if (list->status != AVAILABLEAL)return WRONGALSTATUS;
	if (!content)return NULLALELEMENT;

	if (index >= list->alCount)return ALIDXOUTBOUNDS;

	memcpy((char*)list->data + index * list->typeSize, content, list->typeSize);

	return SUCCESSFULOP;
}

// 从后面补上单个元素
int alPushBack(ArrayList* const list, const void* const content) {
	if (!list)return NULLAL;
	return alInsertAt(list, content, list->alCount);
}

int alPushFront(ArrayList* const list, const void* const content) {
	return alInsertAt(list, content, 0);
}

int alInsertAt(ArrayList* const list, const void* const content, size_t index) {
	if (!list)return NULLAL;
	if (list->status != AVAILABLEAL)return WRONGALSTATUS;
	if (!content)return NULLALELEMENT;

	if (index > list->alCount)return ALIDXOUTBOUNDS;

	int tempStatus = alRealloc(list);
	if (tempStatus != SUCCESSFULOP)return tempStatus;

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

	return SUCCESSFULOP;
}

int alPopBack(ArrayList* const list) {
	if (!list) return NULLAL;
	if (list->status != AVAILABLEAL) return WRONGALSTATUS;
	if (list->alCount == 0)return ALALREADYEMPTY;
	return alRemoveAt(list, list->alCount - 1);
}

int alPopFront(ArrayList* const list) {
	if (!list) return NULLAL;
	if (list->status != AVAILABLEAL) return WRONGALSTATUS;
	if (list->alCount == 0)return ALALREADYEMPTY;
	return alRemoveAt(list, 0);
}

int alRemoveAt(ArrayList* const list, size_t index) {
	if (!list)return NULLAL;
	if (list->status != AVAILABLEAL)return WRONGALSTATUS;
	if (list->alCount == 0) return ALALREADYEMPTY;
	if (index >= list->alCount)return ALIDXOUTBOUNDS;

	if (index < list->alCount - 1) {
		char* dest = (char*)list->data + list->typeSize * index;
		char* src=(char*)list->data + list->typeSize * (index + 1);
		size_t n = (list->alCount - index - 1) * list->typeSize;
		memmove(dest, src, n);
	}
	list->alCount--;

	return SUCCESSFULOP;
}

int alClear(ArrayList* const list) {
	if (!list)return NULLAL;
	if (list->status != AVAILABLEAL)return WRONGALSTATUS;
	list->alCount = 0;
	return SUCCESSFULOP;
}

const char* alStatusToArray(int status) {
	switch (status) {
	case NULLAL: {
		return "null ArrayList*";
	}
	case AVAILABLEAL: {
		return "available ArrayList*";
	}
	case INVALIDALTYPESIZE: {
		return "invalid type size of data";
	}
	case ALALREADYEMPTY: {
		return "ArrayList* has already been empty";
	}

	case SUCCESSFULOP: {
		return "successful operation";
	}
	case WRONGALSTATUS: {
		return "ArrayList* status in error";
	}
	case REALLOCALFAIL: {
		return "fail to realloc";
	}

	case NULLALELEMENT: {
		return "null element";
	}
	case ALIDXOUTBOUNDS: {
		return "index out of bounds";
	}
	case CAPACITYLESSTHANMIN: {
		return "setted capacity is less than the current capacity";
	}
	case REALLOCSIZEOUTBOUNDS: {
		return "the size is bigger than SIZE_MAX/max size of size_t";
	}
	default: {
		return "unknown status";
	}
	}
}