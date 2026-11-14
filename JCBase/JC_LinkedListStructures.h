#pragma once

#include <stddef.h>

typedef struct JC_LinkedList {
	JC_LinkedListNode* header;
	size_t count;
	size_t typeSize;
	int status;
} JC_LinkedList;

typedef struct JC_LinkedListNode {
	JC_LinkedListNode* prior, * next;
	void* data;
} JC_LinkedListNode;