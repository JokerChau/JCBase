#include "ArrayList.h"
#include "resources.h"
#include <stdlib.h>
#include <string.h>

typedef struct ArrayList {
	void* data;
	size_t alCount;
	size_t alMaxCount;
	size_t typeSize;
	int status;
}ArrayList;

ArrayList* alCreate(size_t typeSize) {
	ArrayList* list = (ArrayList*)malloc(sizeof(ArrayList));
	if (!list)return NULL;

	list->data = NULL;
	list->alCount = 0;
	list->alMaxCount = 0;
	list->typeSize = typeSize;
	if (!typeSize) {
		list->status = INVALIDALTYPESIZE;
		return list;
	}
	list->status = AVAILABLEAL;

	return list;
}

void alDestroy(ArrayList* list) {
	if (!list)return;

	if (list->data) {
		free(list->data);
		list->data = NULL;
	}
	free(list);
}

int getAlStatus(const ArrayList* const list) {
	if (!list)return NULLAL;

	return list->status;
}

size_t getAlCount(const ArrayList* const list) {
	if (!list)return 0;

	return list->alCount;
}

int alPushBack(ArrayList* const list, const void* content) {
	if (!list)return NULLAL;
	if (list->status != AVAILABLEAL)return WRONGALSTATUS;
	if (!content)return NULLALELEMENT;

	if (list->alCount == list->alMaxCount) {
		size_t newCap = (list->alMaxCount == 0) ? 4 : list->alMaxCount + 4;
		void* newData = realloc(list->data, newCap * list->typeSize);
		if (!newData)return ALLOCALFAIL;
		list->data = newData;
		list->alMaxCount = newCap;
	}

	char* dest = (char*)list->data + list->alCount * list->typeSize;
	memcpy(dest, content, list->typeSize);

	list->alCount++;
	return AVAILABLEAL;
}