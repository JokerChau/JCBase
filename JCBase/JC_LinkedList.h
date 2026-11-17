#pragma once

#include "wheel_ex.h"
#include <stddef.h>
#include "JC_LinkedListSrc.h"

typedef struct JC_LinkedList JC_LinkedList;
typedef JC_LinkedList* jc_LinkedList;
typedef struct JC_LinkedListNode JC_LinkedListNode;
typedef JC_LinkedListNode* jc_LinkedListNode;

wheels jc_LinkedList jcll_inner_new(size_t typeSize);

#define jcll_newOf(type) jcll_inner_new(sizeof(type))

wheels void jcll_delete(jc_LinkedList list);

wheels size_t jcll_getLength(const JC_LinkedList* const list);

wheels int jcll_getStatus(const JC_LinkedList* const list);

wheels int jcll_clear(jc_LinkedList list);

wheels const char* jcll_statusToCharArray(int status);

wheels int jcll_insertAt(jc_LinkedList list, const void* const content, size_t index);

wheels int jcll_pushBack(jc_LinkedList list, const void* const content);

wheels int jcll_pushFront(jc_LinkedList list, const void* const content);

wheels int jcll_removeAt(jc_LinkedList list, size_t index);

wheels int jcll_popBack(jc_LinkedList list);

wheels int jcll_popFront(jc_LinkedList list);

wheels int jcll_getContentAt(const JC_LinkedList* const list, size_t index, void* outContent);

wheels int jcll_setContentAt(jc_LinkedList list, size_t index, const void* const content);

/*
wheels int llGetFrontNode(const linkedList list, linkedListNode* out);

wheels int llGetBackNode(const linkedList list, linkedListNode* out);

wheels int llGetHeader(const linkedList list, linkedListNode* out);

wheels int llGetNodeAt(const linkedList list, size_t index, linkedListNode* out);

wheels int llNext(linkedListNode* node);

wheels int llPrev(linkedListNode* node);

wheels int llGetFromNode(const linkedList list, const LinkedListNode* node, void* out);

wheels int llSetOfNode(linkedList list, linkedListNode node, const void* content);

wheels int llInsertBefore(linkedList list, linkedListNode node, const void* content);

wheels int llInsertAfter(linkedList list, linkedListNode node, const void* content);

wheels int llRemoveAt(linkedList list, size_t index);

wheels int llMoveBefore(linkedList list, linkedListNode node, linkedListNode pos);

wheels int llMoveAfter(linkedList list, linkedListNode node, linkedListNode pos);

wheels int llMoveToFront(linkedList list, linkedListNode node);

wheels int llMoveToBack(linkedList list, linkedListNode node);

wheels int llSpliceBack(linkedList front, linkedList back);

wheels int llSpliceBefore(linkedList hold, linkedListNode node, linkedList material);

wheels int llSpliceAfter(linkedList hold, linkedListNode node, linkedList material);

wheels int llSpliceAt(linkedList hold, size_t index, linkedList material);

wheels int llClearInRange(linkedList list, size_t start, size_t end);
*/

/*
// 值操作（按索引）
int llGetContentAt(const linkedList list, size_t index, void* out);按索引从ll实例获取内容 Y
int llSetContentAt(linkedList list, size_t index, const void* content);按索引对ll实例设置内容 Y

// 节点操作
int llGetFrontNode(const linkedList list, linkedListNode* out);获取实例的首个数据节点 Y
int llGetBackNode(const linkedList list, linkedListNode* out);获取实例的最后一个数据节点 Y
int llGetHeader(const linkedList list, linkedListNode* out);获取实例的头节点 Y
int llGetNodeAt(const linkedList list, size_t index, linkedListNode* out);获取索引处结点 Y
int llNext(linkedListNode* node);将当前结点指针一向下一个结点 Y
int llPrev(linkedListNode* node);将当前结点指针一向上一个结点 Y

int llGetFromNode(const linkedList list, const LinkedListNode* node, void* out);以list的标准通过结点获取内容 Y
int llSetOfNode(linkedList list, linkedListNode node, const void* content);以list的标准通过结点设置内容 Y
int llInsertBefore(linkedList list, linkedListNode node, const void* content);在某个结点前插入 Y
int llInsertAfter(linkedList list, linkedListNode node, const void* content);在某个结点后插入 Y
int llRemoveNode(linkedList list, linkedListNode node);去除某个结点 Y

// 移动节点
int llMoveBefore(linkedList list, linkedListNode node, linkedListNode pos);将结点移至某一结点前面 Y
int llMoveAfter(linkedList list, linkedListNode node, linkedListNode pos);将结点移至某一结点后面 Y
int llMoveToFront(linkedList list, linkedListNode node);将结点移至第一个数据节点 Y
int llMoveToBack(linkedList list, linkedListNode node);将结点移至最后一个数据节点 Y

// 拼接
int llSpliceBack(linkedList front, linkedList back);在链表后面拼接另一个链表，front的header仍然为原头节点，back不可用，记得释放
int llSpliceBefore(linkedList hold, linkedListNode node, linkedList material);将一个链表拼在另一个链表某个结点前面
int llSpliceAfter(linkedList hold, linkedListNode node, linkedList material);将一个链表拼在另一个链表某个结点后面
int llSpliceAt(linkedList hold, size_t index, linkedList material);

// 范围
int llClearInRange(linkedList list, size_t start, size_t end);清除范围内所有结点（左开右闭）
*/