#include "JC_LinkedList.h"
#include "JC_LinkedListStructures.h"
#include <stdlib.h>
#include <string.h>

// 生成一个自指结点，接管内容
static jc_LinkedListNode generateNode(const void* const content) {
	jc_LinkedListNode node = (jc_LinkedListNode)malloc(sizeof(JC_LinkedListNode));
	if (!node)return node;
	node->data = content;
	node->next = node;
	node->prior = node;
	return node;
}

// 生成一个自指结点，拷贝内容，要注意node可能为NULL，但实际上会通过status知道node是否为NULL，若content为NULL，行为与generateNode一致
static jc_LinkedListNode generateCopyNode(const void* const content, size_t len, int* status) {
	jc_LinkedListNode node = (jc_LinkedListNode)malloc(sizeof(JC_LinkedListNode));
	if (!node) {
		*status = JC_LL__ALLOCATION_FAILED;
		return node;
	}

	// node自指
	node->prior = node;
	node->next = node;

	if (!content) {
		node->data = content;
		*status = JC_LL__SUCCESSFUL_OP;
		return node;
	}

	// 为内容申请内存，成功与否都更新状态
	void* ctnt = malloc(len);
	if (!ctnt) {
		*status = JC_LL__ALLOCATION_FAILED;
	}
	else {
		*status = JC_LL__SUCCESSFUL_OP;
		memcpy(ctnt, content, len);
	}
	node->data = ctnt;
	return node;
}

// 判断list是否可用，返回状态码
static JC_LinkedListStatus isAvailable(jc_LinkedList list) {
	if (!list)return JC_LL__NULL;
	if (list->status != JC_LL__AVAILABLE)return list->status;
	return JC_LL__AVAILABLE;
}

// 自动判断哪边更靠近index并返回index指向结点的指针，若index == list->count，则返回头结点
static jc_LinkedListNode toIndexNode(jc_LinkedList list, size_t index) {
	jc_LinkedListNode p = list->header;
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

// 摘链，不释放
static void getDownNode(jc_LinkedListNode node) {
	node->prior->next = node->next;
	node->next->prior = node->prior;
}

// 删除所给结点，会释放该结点
static void deleteNode(jc_LinkedListNode node) {
	if (node->data)free(node->data);
	getDownNode(node);
	free(node);
}

// 在某个结点前推入结点
static void pushBefore(jc_LinkedListNode node, jc_LinkedListNode where) {
	node->prior = where->prior;
	node->next = where;
	where->prior->next = node;
	where->prior = node;
}

// 在某个结点后推入结点
static void pushAfter(jc_LinkedListNode node, jc_LinkedListNode where) {
	node->next = where->next;
	node->prior = where;
	where->next->prior = node;
	where->next = node;
}

// 自行保证list和index可用，index至少不能大于list->count，允许NULL data，不允许NULL node
static int insertCore(jc_LinkedList list, const void* const content, size_t index) {
	// 1. 找到索引所指向的结点
	jc_LinkedListNode node = toIndexNode(list, index);
	int status = JC_LL__SUCCESSFUL_OP;
	// 2. 生成一个自指结点
	jc_LinkedListNode tempNode = generateCopyNode(content, list->typeSize, &status); // status会接受操作的状态
	// 拒绝NULL入链
	if (status != JC_LL__SUCCESSFUL_OP) {
		free(tempNode);
		return status;
	}
	// 3. 将结点前插
	pushBefore(tempNode, node);
	list->count++;
	return JC_LL__SUCCESSFUL_OP;
}

static int removeCore(jc_LinkedList list, size_t index) {
	// 1. 找到索引指向的结点
	jc_LinkedListNode node = toIndexNode(list, index);
	// 2. 摘链释放
	deleteNode(node);
	list->count--;
	return JC_LL__SUCCESSFUL_OP;
}

jc_LinkedList jcll_inner_new(size_t typeSize) {
	jc_LinkedList temp = (jc_LinkedList)malloc(sizeof(JC_LinkedList));
	if (!temp)return NULL;
	temp->count = 0;
	temp->typeSize = typeSize;
	temp->header = NULL;
	if (!typeSize) {
		temp->status = JC_LL__INVALID_TYPE_SIZE;
		return temp;
	}

	jc_LinkedListNode tempNode = generateNode(NULL);
	if (!tempNode) {
		temp->status = JC_LL__NULL_HEADER;
		return temp;
	}
	temp->header = tempNode;
	temp->status = JC_LL__AVAILABLE;
	return temp;
}

void jcll_delete(jc_LinkedList list) {
	if (!list)return;
	if (list->header) {
		jc_LinkedListNode p = list->header->next;
		while (p != list->header) {
			if (p->data)free(p->data);
			jc_LinkedListNode q = p;
			p = p->next;
			free(q);
		}
		free(list->header);
	}
	free(list);
}

size_t jcll_getLength(const JC_LinkedList* const list) {
	if (!list)return 0;
	return list->count;
}

int jcll_getStatus(const JC_LinkedList* const list) {
	if (!list)return JC_LL__NULL;
	return list->status;
}

int jcll_clear(jc_LinkedList list) {
	JC_LinkedListStatus ts = isAvailable(list);
	if (ts != JC_LL__AVAILABLE)return ts;

	jc_LinkedListNode p = list->header->next;
	while (p != list->header) {
		if (p->data)free(p->data);
		jc_LinkedListNode q = p;
		p = p->next;
		free(q);
	}
	list->count = 0;
	list->header->prior = list->header;
	list->header->next = list->header;
	return JC_LL__SUCCESSFUL_OP;
}

const char* jcll_statusToCharArray(int status) {
	switch (status) {
	case JC_LL__NULL:                       return "null list";
	case JC_LL__AVAILABLE:                  return "available list";
	case JC_LL__INVALID_TYPE_SIZE:          return "invalid type size";
	case JC_LL__NULL_HEADER:                return "null header of this list";
	case JC_LL__ALREADY_EMPTY:				return "empty list";

	case JC_LL__SUCCESSFUL_OP:              return "successful operation";
	case JC_LL__ALLOCATION_FAILED:          return "memory allocation failed";

	case JC_LL__INDEX_OUT_OF_BOUNDS:        return "index is out of bounds";
	case JC_LL__NULL_RECEIVER:				return "null receiver";

	default:                                return "Unknown status";
	}
}

int jcll_insertAt(jc_LinkedList list, const void* const content, size_t index) {
	JC_LinkedListStatus ts = isAvailable(list);
	if (ts != JC_LL__AVAILABLE)return ts;
	if (index > list->count)return JC_LL__INDEX_OUT_OF_BOUNDS;

	return insertCore(list, content, index);
}

int jcll_pushBack(jc_LinkedList list, const void* const content) {
	JC_LinkedListStatus ts = isAvailable(list);
	if (ts != JC_LL__AVAILABLE)return ts;

	return insertCore(list, content, list->count);
}

int jcll_pushFront(jc_LinkedList list, const void* const content) {
	JC_LinkedListStatus ts = isAvailable(list);
	if (ts != JC_LL__AVAILABLE)return ts;
	
	return insertCore(list, content, 0);
}

int jcll_removeAt(jc_LinkedList list, size_t index) {
	JC_LinkedListStatus ts = isAvailable(list);
	if (ts != JC_LL__AVAILABLE)return ts;
	if (!list->count)return JC_LL__ALREADY_EMPTY;
	if (index >= list->count)return JC_LL__INDEX_OUT_OF_BOUNDS;

	return removeCore(list, index);
}

int jcll_popBack(jc_LinkedList list) {
	JC_LinkedListStatus ts = isAvailable(list);
	if (ts != JC_LL__AVAILABLE)return ts;
	if (!list->count)return JC_LL__ALREADY_EMPTY;

	return removeCore(list, list->count - 1);
}

int jcll_popFront(jc_LinkedList list) {
	JC_LinkedListStatus ts = isAvailable(list);
	if (ts != JC_LL__AVAILABLE)return ts;
	if (!list->count)return JC_LL__ALREADY_EMPTY;

	return removeCore(list, 0);
}

int jcll_getContentAt(const JC_LinkedList* const list, size_t index, void* outContent) {
	JC_LinkedListStatus ts = isAvailable(list);
	if (ts != JC_LL__AVAILABLE)return ts;
	if (!list->count)return JC_LL__ALREADY_EMPTY;
	if (index >= list->count)return JC_LL__INDEX_OUT_OF_BOUNDS;
	if (!outContent)return JC_LL__NULL_RECEIVER;

	jc_LinkedListNode temp = toIndexNode(list, index);

	if (!temp->data)memset(outContent, 0, list->typeSize); // 把接收器的内存全清零，代表取到了一个空值
	else memcpy(outContent, temp->data, list->typeSize); // 正常有数据的情况
	return JC_LL__SUCCESSFUL_OP;
}

int jcll_setContentAt(jc_LinkedList list, size_t index, const void* const content) {
	JC_LinkedListStatus ts = isAvailable(list);
	if (ts != JC_LL__AVAILABLE)return ts;
	if (!list->count)return JC_LL__ALREADY_EMPTY;
	if (index >= list->count)return JC_LL__INDEX_OUT_OF_BOUNDS;

	jc_LinkedListNode temp = toIndexNode(list, index);
	if (!content) {
		free(temp->data);
		temp->data = NULL;
	}
	else {
		if (!temp->data) {
			temp->data = malloc(list->typeSize);
			if (!temp->data)return JC_LL__ALLOCATION_FAILED;
		}
		memcpy(temp->data, content, list->typeSize);
	}

	return JC_LL__SUCCESSFUL_OP;
}

/*
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
*/