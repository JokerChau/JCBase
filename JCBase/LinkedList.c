#include "JC_LinkedList.h"
#include <stdlib.h>
#include <string.h>

typedef struct LinkedList {
	LinkedListNode* header;
	size_t count;
	size_t typeSize;
	int status;
}LinkedList;

typedef struct LinkedListNode {
	LinkedListNode* prior, * next;
	void* data;
}LinkedListNode;

#define IS_AVAILABLE 100

static int isAvailable(linkedList list) {
	if (!list)return LL_NULL;
	if (!list->header)return LL_NULLHEADER;
	return IS_AVAILABLE;
}

static linkedListNode toTheIndex(linkedList list, size_t index) {
	linkedListNode p = list->header;
	// 将p指向index所在结点
	if (index < list->count / 2) {
		for (size_t i = 0; i <= index; i++) {
			p = p->next;
		}
	}
	else {
		for (size_t i = 0; i < list->count - index; i++) {
			p = p->prior;
		}
	}
	return p;
}

static void removeTheNode(linkedListNode node) {
	if (node->data)free(node->data);
	node->prior->next = node->next;
	node->next->prior = node->prior;
	free(node);
}

static void getDownNode(linkedListNode node) {
	node->prior->next = node->next;
	node->next->prior = node->prior;
}

static void pushBefore(linkedListNode node, linkedListNode where) {
	node->prior = where->prior;
	node->next = where;
	where->prior->next = node;
	where->prior = node;
}

static void pushAfter(linkedListNode node, linkedListNode where) {
	node->next = where->next;
	node->prior = where;
	where->next->prior = node;
	where->next = node;
}

linkedList llCreate(size_t typeSize) {
	linkedList temp = (linkedList)malloc(sizeof(LinkedList));
	if (!temp)return NULL;
	temp->count = 0;
	temp->typeSize = typeSize;
	temp->header = NULL;
	if (!typeSize) {
		temp->status = LL_INVALIDTYPESIZE;
		return temp;
	}
	linkedListNode tempNode = (linkedListNode)malloc(sizeof(LinkedListNode));
	if (!tempNode) {
		temp->status = LL_NULLHEADER;
		return temp;
	}
	temp->header = tempNode;
	temp->status = LL_AVAILABLE;
	tempNode->prior = tempNode; 
	tempNode->next = tempNode;
	tempNode->data = NULL;
	return temp;
}

int llDestroy(linkedList list) {
	if (!list)return LL_SUCCESSFULOP;
	if (list->header) {
		// 临时指针指向头节点
		linkedListNode p = list->header->next;
		while (p != list->header) {
			if (p->data)free(p->data);
			linkedListNode q = p;
			p = p->next;
			free(q);
		}
		free(list->header);
	}
	free(list);
	return LL_SUCCESSFULOP;
}

int llClear(linkedList list) {
	int ava = isAvailable(list);
	if (ava != IS_AVAILABLE)return ava;
	// 指向第一个数据节点
	linkedListNode p = list->header->next;
	while (p != list->header) {
		if (p->data)free(p->data);
		linkedListNode q = p;
		p = p->next;
		free(q);
	}
	list->count = 0;
	list->header->prior = list->header;
	list->header->next = list->header;
	return LL_SUCCESSFULOP;
}


int llGetStatus(const LinkedList* const list) {
	if (!list)return LL_NULL;
	return list->status;
}

size_t llGetCount(const LinkedList* const list) {
	if (!list)return 0;
	return list->count;
}

const char* llStatusToCharArray(int status) {
	switch (status) {
	case LL_NULL: {
		return "linkedList instance is NULL";
	}
	case LL_AVAILABLE: {
		return "linkedList instance is available";
	}
	case LL_NULLHEADER: {
		return "header node is NULL";
	}
	case LL_HEADERDELETING: {
		return "invalid operation: header node is deleting";
	}
	case LL_NULLNODE: {
		return "node is NULL";
	}
	case LL_NULLNEWNODE: {
		return "failed to alloc new node";
	}
	case LL_NULLDATA: {
		return "data of the node is NULL";
	}
	case LL_INVALIDTYPESIZE: {
		return "unusable type size";
	}
	case LL_ALREADYEMPTY: {
		return "this instance has already been empty";
	}
	case LL_SAMELL: {
		return "the same linkedList";
	}

	case LL_NULLCONTENT: {
		return "content is NULL";
	}
	case LL_INDEXOUTOFBOUNDS: {
		return "index out of bounds";
	}
	case LL_NULLRECEIVER: {
		return "receiver is NULL";
	}
	case LL_EMPTYRANGE: {
		return "empty range";
	}

	case LL_SUCCESSFULOP: {
		return "successful operation";
	}
	default: {
		return "Unknown status";
	}
	}
}


int llPushBack(linkedList list, const void* content) {
	int ava = isAvailable(list);
	if (ava != IS_AVAILABLE)return ava;
	if (!content)return LL_NULLCONTENT;
	linkedListNode temp = (linkedListNode)malloc(sizeof(LinkedListNode));
	if (!temp)return LL_NULLNEWNODE;
	temp->data = malloc(list->typeSize);
	if (!temp->data) {
		free(temp);
		return LL_NULLDATA;
	}
	memcpy(temp->data, content, list->typeSize);

	pushBefore(temp, list->header);
	list->count++;

	return LL_SUCCESSFULOP;
}

int llPushFront(linkedList list, const void* content) {
	int ava = isAvailable(list);
	if (ava != IS_AVAILABLE)return ava;
	if (!content)return LL_NULLCONTENT;
	linkedListNode temp = (linkedListNode)malloc(sizeof(LinkedListNode));
	if (!temp)return LL_NULLNEWNODE;
	temp->data = malloc(list->typeSize);
	if (!temp->data) {
		free(temp);
		return LL_NULLDATA;
	}
	memcpy(temp->data, content, list->typeSize);

	pushAfter(temp, list->header);
	list->count++;

	return LL_SUCCESSFULOP;
}

int llInsertAt(linkedList list, const void* content, size_t index) {
	int ava = isAvailable(list);
	if (ava != IS_AVAILABLE)return ava;
	if (!content)return LL_NULLCONTENT;
	if (index > list->count)return LL_INDEXOUTOFBOUNDS;

	if (index == list->count)return llPushBack(list, content);
	if (!index)return llPushFront(list, content);

	linkedListNode temp = (linkedListNode)malloc(sizeof(LinkedListNode));
	if (!temp)return LL_NULLNEWNODE;
	temp->data = malloc(list->typeSize);
	if (!temp->data) {
		free(temp);
		return LL_NULLDATA;
	}
	memcpy(temp->data, content, list->typeSize);

	linkedListNode p = toTheIndex(list, index);

	pushBefore(temp, p);
	list->count++;

	return LL_SUCCESSFULOP;
}

int llPopBack(linkedList list) {
	int ava = isAvailable(list);
	if (ava != IS_AVAILABLE)return ava;
	if (!list->count)return LL_ALREADYEMPTY;
	linkedListNode temp = list->header->prior;
	removeTheNode(temp);
	list->count--;
	return LL_SUCCESSFULOP;
}

int llPopFront(linkedList list) {
	int ava = isAvailable(list);
	if (ava != IS_AVAILABLE)return ava;
	if (!list->count)return LL_ALREADYEMPTY;
	linkedListNode temp = list->header->next;
	removeTheNode(temp);
	list->count--;
	return LL_SUCCESSFULOP;
}

int llRemoveAt(linkedList list, size_t index) {
	int ava = isAvailable(list);
	if (ava != IS_AVAILABLE)return ava;
	if (!list->count)return LL_ALREADYEMPTY;
	if (index >= list->count)return LL_INDEXOUTOFBOUNDS;

	if (index == list->count - 1)return llPopBack(list);
	if (!index)return llPopFront(list);

	linkedListNode p = toTheIndex(list, index);
	removeTheNode(p);
	list->count--;
	return LL_SUCCESSFULOP;
}


// 按索引从ll实例获取内容
int llGetContentAt(const LinkedList* list, size_t index, void* out) {
	int ava = isAvailable(list);
	if (ava != IS_AVAILABLE)return ava;
	if (!list->count)return LL_ALREADYEMPTY;
	if (index >= list->count)return LL_INDEXOUTOFBOUNDS;
	if (!out)return LL_NULLRECEIVER;

	linkedListNode temp = toTheIndex(list, index);
	memcpy(out, temp->data, list->typeSize);
	return LL_SUCCESSFULOP;
}

int llSetContentAt(linkedList list, size_t index, const void* content) {
	int ava = isAvailable(list);
	if (ava != IS_AVAILABLE)return ava;
	if (!list->count)return LL_ALREADYEMPTY;
	if (index >= list->count)return LL_INDEXOUTOFBOUNDS;
	if (!content)return LL_NULLCONTENT;

	linkedListNode temp = toTheIndex(list, index);
	memcpy(temp->data, content, list->typeSize);
	return LL_SUCCESSFULOP;
}


int llGetFrontNode(const linkedList list, linkedListNode* out) {
	int ava = isAvailable(list);
	if (ava != IS_AVAILABLE)return ava;
	if (!list->count)return LL_ALREADYEMPTY;
	if (!out)return LL_NULLRECEIVER;
	*out = toTheIndex(list, 0);
	return LL_SUCCESSFULOP;
}

int llGetBackNode(const linkedList list, linkedListNode* out) {
	int ava = isAvailable(list);
	if (ava != IS_AVAILABLE)return ava;
	if (!list->count)return LL_ALREADYEMPTY;
	if (!out)return LL_NULLRECEIVER;
	*out = toTheIndex(list, list->count - 1);
	return LL_SUCCESSFULOP;
}

int llGetHeader(const linkedList list, linkedListNode* out) {
	int ava = isAvailable(list);
	if (ava != IS_AVAILABLE)return ava;
	if (!out)return LL_NULLRECEIVER;
	*out = list->header;
	return LL_SUCCESSFULOP;
}

int llGetNodeAt(const linkedList list, size_t index, linkedListNode* out) {
	int ava = isAvailable(list);
	if (ava != IS_AVAILABLE)return ava;
	if (!list->count)return LL_ALREADYEMPTY;
	if (index >= list->count)return LL_INDEXOUTOFBOUNDS;
	if (!out)return LL_NULLRECEIVER;
	*out = toTheIndex(list, index);
	return LL_SUCCESSFULOP;
}

int llNext(linkedListNode* node) {
	if (!node)return LL_NULLRECEIVER;
	if (!(*node))return LL_NULLNODE;
	*node = (*node)->next;
	return LL_SUCCESSFULOP;
}

int llPrev(linkedListNode* node) {
	if (!node)return LL_NULLRECEIVER;
	if (!(*node))return LL_NULLNODE;
	*node = (*node)->prior;
	return LL_SUCCESSFULOP;
}


int llGetFromNode(const linkedList list, const LinkedListNode* node, void* out) {
	int ava = isAvailable(list);
	if (ava != IS_AVAILABLE)return ava;
	if (!list->count)return LL_ALREADYEMPTY;
	if (!node)return LL_NULLNODE;
	if (node == list->header)return LL_ISHEADER;
	if (!out)return LL_NULLRECEIVER;

	memcpy(out, node->data, list->typeSize);
	return LL_SUCCESSFULOP;
}

int llSetOfNode(linkedList list, linkedListNode node, const void* content) {
	int ava = isAvailable(list);
	if (ava != IS_AVAILABLE)return ava;
	if (!list->count)return LL_ALREADYEMPTY;
	if (!node)return LL_NULLNODE;
	if (node == list->header)return LL_ISHEADER;
	if (!content)return LL_NULLCONTENT;

	memcpy(node->data, content, list->typeSize);
	return LL_SUCCESSFULOP;
}

int llInsertBefore(linkedList list, linkedListNode node, const void* content) {
	int ava = isAvailable(list);
	if (ava != IS_AVAILABLE)return ava;
	if (!list->count)return LL_ALREADYEMPTY;
	if (!node)return LL_NULLNODE;
	if (!content)return LL_NULLCONTENT;

	linkedListNode temp = (linkedListNode)malloc(sizeof(LinkedListNode));
	if (!temp)return LL_NULLNEWNODE;
	temp->data = malloc(list->typeSize);
	if (!temp->data) {
		free(temp);
		return LL_NULLDATA;
	}

	memcpy(temp->data, content, list->typeSize);
	pushBefore(temp, node);
	list->count++;
	return LL_SUCCESSFULOP;
}

int llInsertAfter(linkedList list, linkedListNode node, const void* content) {
	int ava = isAvailable(list);
	if (ava != IS_AVAILABLE)return ava;
	if (!list->count)return LL_ALREADYEMPTY;
	if (!node)return LL_NULLNODE;
	if (!content)return LL_NULLCONTENT;

	linkedListNode temp = (linkedListNode)malloc(sizeof(LinkedListNode));
	if (!temp)return LL_NULLNEWNODE;
	temp->data = malloc(list->typeSize);
	if (!temp->data) {
		free(temp);
		return LL_NULLDATA;
	}

	memcpy(temp->data, content, list->typeSize);
	pushAfter(temp, node);
	list->count++;
	return LL_SUCCESSFULOP;
}

int llRemoveNode(linkedList list, linkedListNode node) {
	int ava = isAvailable(list);
	if (ava != IS_AVAILABLE)return ava;
	if (!node)return LL_SUCCESSFULOP;
	if (node == list->header)return LL_HEADERDELETING;
	removeTheNode(node);
	list->count--;
	return LL_SUCCESSFULOP;
}


int llMoveBefore(linkedList list, linkedListNode node, linkedListNode pos) {
	int ava = isAvailable(list);
	if (ava != IS_AVAILABLE)return ava;
	if (!list->count)return LL_ALREADYEMPTY;
	if (!node)return LL_NULLNODE;
	if (node == list->header)return LL_ISHEADER;
	if (!pos)return LL_NULLNODE;
	if (node == pos)return LL_SUCCESSFULOP;

	getDownNode(node);
	pushBefore(node, pos);

	return LL_SUCCESSFULOP;
}

int llMoveAfter(linkedList list, linkedListNode node, linkedListNode pos) {
	int ava = isAvailable(list);
	if (ava != IS_AVAILABLE)return ava;
	if (!list->count)return LL_ALREADYEMPTY;
	if (!node)return LL_NULLNODE;
	if (node == list->header)return LL_ISHEADER;
	if (!pos)return LL_NULLNODE;
	if (node == pos)return LL_SUCCESSFULOP;

	getDownNode(node);
	pushAfter(node, pos);

	return LL_SUCCESSFULOP;
}

int llMoveToFront(linkedList list, linkedListNode node) {
	return llMoveAfter(list, node, list->header);
}

int llMoveToBack(linkedList list, linkedListNode node) {
	return llMoveBefore(list, node, list->header);
}


int llSpliceBack(linkedList front, linkedList back) {
	int ava = isAvailable(front);
	if (ava != IS_AVAILABLE)return ava;
	ava = isAvailable(back);
	if (ava != IS_AVAILABLE)return ava;
	if (front == back)return LL_SAMELL;
	if (!front->count || !back->count)return LL_ALREADYEMPTY;
	// 在链表后面拼接另一个链表，front的header仍然为原头节点，back不可用，记得释放
	// front的尾结点连上back的第一个数据结点
	front->header->prior->next = back->header->next;
	// back的第一个数据结点接上front的第一个数据结点
	back->header->next->prior = front->header->prior;
	// front的头结点连上back的尾结点
	front->header->prior = back->header->prior;
	// back的尾结点接上front的头结点
	back->header->prior->next = front->header;

	front->count += back->count;
	free(back->header);
	back->header = NULL;

	return LL_SUCCESSFULOP;
}

int llSpliceBefore(linkedList hold, linkedListNode node, linkedList material) {
	int ava = isAvailable(hold);
	if (ava != IS_AVAILABLE)return ava;
	ava = isAvailable(material);
	if (ava != IS_AVAILABLE)return ava;
	if (hold == material)return LL_SAMELL;
	if (!hold->count || !material->count)return LL_ALREADYEMPTY;
	if (!node)return LL_NULLNODE;

	node->prior->next = material->header->next;
	material->header->next->prior = node->prior;
	material->header->prior->next = node;
	node->prior = material->header->prior;

	hold->count += material->count;
	free(material->header);
	material->header = NULL;

	return LL_SUCCESSFULOP;
}

int llSpliceAfter(linkedList hold, linkedListNode node, linkedList material) {
	int ava = isAvailable(hold);
	if (ava != IS_AVAILABLE)return ava;
	ava = isAvailable(material);
	if (ava != IS_AVAILABLE)return ava;
	if (hold == material)return LL_SAMELL;
	if (!hold->count || !material->count)return LL_ALREADYEMPTY;
	if (!node)return LL_NULLNODE;

	node->next->prior = material->header->prior;
	material->header->prior->next = node->next;
	material->header->next->prior = node;
	node->next = material->header->next;

	hold->count += material->count;
	free(material->header);
	material->header = NULL;

	return LL_SUCCESSFULOP;
}

int llSpliceAt(linkedList hold, size_t index, linkedList material) {
	int ava = isAvailable(hold);
	if (ava != IS_AVAILABLE)return ava;
	ava = isAvailable(material);
	if (ava != IS_AVAILABLE)return ava;
	if (hold == material)return LL_SAMELL;
	if (!hold->count || !material->count)return LL_ALREADYEMPTY;
	if (index > hold->count)return LL_INDEXOUTOFBOUNDS;
	if (index == 0)return llSpliceBefore(hold, hold->header->next, material);
	if (index == hold->count)return llSpliceBack(hold, material);

	linkedListNode temp = toTheIndex(hold, index);
	return llSpliceBefore(hold, temp, material);
}


int llClearInRange(linkedList list, size_t start, size_t end) {
	int ava = isAvailable(list);
	if (ava != IS_AVAILABLE)return ava;
	if (!list->count)return LL_ALREADYEMPTY;
	if (start > end || end > list->count)return LL_INDEXOUTOFBOUNDS;
	if (start == end)return LL_EMPTYRANGE;

	linkedListNode p = toTheIndex(list, start);
	linkedListNode w = p->prior;
	for (size_t i = start; i < end; i++) {
		if (p->data)free(p->data);
		linkedListNode q = p;
		p = p->next;
		free(q);
	}
	w->next = p;
	p->prior = w;
	
	list->count -= end - start;
	return LL_SUCCESSFULOP;
}