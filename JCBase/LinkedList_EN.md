# LinkedList
## Other Languages md
### 简体中文
[LinkedList.md 简中版](LinkedList.md)
## Interface Description

> jc_LinkedList jcll_inner_new(size_t typeSize)
```text
This is an internal function. It returns a stateful empty linked list instance (with a header sentinel node), requiring the size of the element type.
Note: If typeSize is 0, the returned instance status is JC_LL__INVALID_TYPE_SIZE; if memory allocation fails, the status may be JC_LL__NULL_HEADER; the returned instance must be released via jcll_delete.
```
> jcll_newOf(type)
```text
This is the recommended macro to create a LinkedList instance.
Note: It uses _Generic to reject void types at compile time and automatically computes sizeof(type).
```
> void jcll_delete(jc_LinkedList list)
```text
Releases the given linked list instance, all its internal nodes, and node data.
Note: Passing NULL will not crash; this only frees memory allocated by the list itself, not any external memory pointed to by pointers stored inside the container (shallow copy semantics).
```
> size_t jcll_getLength(const JC_LinkedList* const list)
```text
Returns the current number of elements in the linked list (count).
Note: Returns 0 if list is NULL.
```
> int jcll_getStatus(const JC_LinkedList* const list)
```text
Returns the current status code of the instance.
Note: Returns JC_LL__NULL if list is NULL.
```
> int jcll_clear(jc_LinkedList list)
```text
Clears all data nodes in the linked list, resetting it to an empty state.
Note: This operation is idempotent; it returns success regardless of whether the list is currently empty; it frees all data nodes and their internal data memory, but retains the header sentinel node structure.
```
> const char* jcll_statusToCharArray(int status)
```text
Returns a human-readable description of the given status code.
Note: The returned string is static; the caller must not and need not free it.
```
> int jcll_insertAt(jc_LinkedList list, const void* const content, size_t index)
```text
Inserts an element at the specified index.
Note: index may equal the current element count (append at end); if index > count, returns JC_LL__INDEX_OUT_OF_BOUNDS; if content is NULL, a node with NULL data is inserted at that position; time complexity is O(N) (bidirectional search optimization is used internally).
```
> int jcll_pushBack(jc_LinkedList list, const void* const content)
```text
Appends an element to the end of the linked list.
Note: Equivalent to jcll_insertAt(list, content, list->count).
```
> int jcll_pushFront(jc_LinkedList list, const void* const content)
```text
Inserts an element at the beginning of the linked list.
Note: Equivalent to jcll_insertAt(list, content, 0).
```
> int jcll_removeAt(jc_LinkedList list, size_t index)
```text
Removes the element at index.
Note: Out-of-bounds index returns JC_LL__INDEX_OUT_OF_BOUNDS; if the list is empty, returns JC_LL__ALREADY_EMPTY; time complexity is O(N).
```
> int jcll_popBack(jc_LinkedList list)
```text
Removes the element at the end of the linked list.
Note: If the list is empty, returns JC_LL__ALREADY_EMPTY.
```
> int jcll_popFront(jc_LinkedList list)
```text
Removes the element at the beginning of the linked list.
Note: If the list is empty, returns JC_LL__ALREADY_EMPTY.
```
> int jcll_getContentAt(const JC_LinkedList* const list, size_t index, void* outContent)
```text
Retrieves the element at index and copies its content into the memory pointed to by outContent.
Note: The caller must ensure outContent points to memory of at least typeSize; if the node stores NULL data, outContent is zeroed out (memset) and success is returned; out-of-bounds index returns JC_LL__INDEX_OUT_OF_BOUNDS.
```
> int jcms_setContentAt(jc_LinkedList list, size_t index, const void* const content)
```text
Replaces the element at index with the content pointed to by content.
Note: If content is NULL, it frees the node's original data memory and sets the node's data to NULL; if the original node data is NULL, it allocates memory first before copying; out-of-bounds index returns JC_LL__INDEX_OUT_OF_BOUNDS.
```