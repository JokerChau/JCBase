# ArrayList
## Other Languages md
### 简体中文
[ArrayList.md 简中版](ArrayList.md)
## Interface Description

> jc_ArrayList jcal_inner_new(size_t sizeType, unsigned int flags)
```text
This is an internal function. It returns a stateful empty ArrayList instance, requiring the size of the element type and instance flags.
Note: If sizeType is 0, the returned instance status is JC_AL__INVALID_TYPE_SIZE; the returned instance must be released via jcal_delete.
```
> jcal_newOf(type)
```text
This is the recommended macro to create an ArrayList instance with default flags (flags = 0).
Note: It uses _Generic to reject void types at compile time and automatically computes sizeof(type).
```
> jcal_newOfWithFlags(type, flags)
```text
This is the recommended macro to create an ArrayList instance with specified flags.
Note: You can pass JC_AL_FLAG_AUTO_SHRINK to enable automatic shrinking.
```
> unsigned int jcal_getFlags(const jc_ArrayList list)
```text
Returns the current flags (bitmask) of the instance.
Note: Returns 0 if list is NULL.
```
> void jcal_setFlags(jc_ArrayList list, unsigned int flags)
```text
Sets the flags (bitmask) of the instance.
Note: Passing NULL will not crash; you can dynamically toggle auto-shrinking via JC_AL_FLAG_AUTO_SHRINK.
```
> void jcal_delete(jc_ArrayList list)
```text
Releases the given ArrayList instance and its internal buffer.
Note: Passing NULL will not crash; this only frees the container, not any external memory pointed to by pointers stored inside the container (shallow free).
```
> size_t jcal_getLength(const jc_ArrayList list)
```text
Returns the current number of elements (count).
Note: Returns 0 if list is NULL or its data is NULL.
```
> size_t jcal_getMaxLength(const jc_ArrayList list)
```text
Returns the current maximum capacity (maxCount).
Note: Returns 0 if list is NULL.
```
> int jcal_getStatus(const jc_ArrayList list)
```text
Returns the current status code of the instance.
Note: Returns JC_AL__NULL if list is NULL.
```
> int jcal_setMaxLength(jc_ArrayList list, size_t count)
```text
Forces the maximum capacity to count.
Note: If count is less than the current element count, it returns JC_AL__CAPACITY_LESS_THAN_MIN; if count is 0, it frees the underlying memory; this operation may cause shrinking.
```
> int jcal_reserve(jc_ArrayList list, size_t capacity)
```text
Pre-allocates capacity (grow-only) for performance optimization.
Note: If capacity is less than or equal to the current capacity, it returns success immediately without any operation; a success return does not mean memory was actually allocated, only that the capacity is sufficient.
```
> int jcal_clear(jc_ArrayList list)
```text
Clears all elements in the instance (count = 0).
Note: This does not free the underlying memory; capacity (maxCount) remains unchanged.
```
> int jcal_shrinkToFit(const jc_ArrayList list)
```text
Shrinks the underlying memory to exactly fit the current elements (maxCount = count).
Note: If the current element count is 0, it frees the underlying memory; a success return does not guarantee the memory was shrunk (realloc may return the original pointer).
```
> int jcal_getAt(const JC_ArrayList* const list, size_t index, void* ele)
```text
Retrieves the element at index and copies its content into the memory pointed to by ele.
Note: The caller must ensure ele points to memory of at least typeSize; out-of-bounds index returns JC_AL__INDEX_OUT_OF_BOUNDS.
```
> int jcal_setAt(JC_ArrayList* const list, size_t index, const void* const content)
```text
Replaces the element at index with the content pointed to by content.
Note: content must not be NULL; out-of-bounds index returns JC_AL__INDEX_OUT_OF_BOUNDS.
```
> int jcal_pushBack(JC_ArrayList* const list, const void* const content)
```text
Appends an element to the end.
Note: Automatic growth is triggered internally; content must not be NULL.
```
> int jcal_pushFront(JC_ArrayList* const list, const void* const content)
```text
Inserts an element at the beginning.
Note: Time complexity is O(N) due to memmove of all elements; content must not be NULL.
```
> int jcal_insertAt(JC_ArrayList* const list, const void* const content, size_t index)
```text
Inserts an element at the specified index.
Note: index may equal the current element count (append at end); if index > count, returns JC_AL__INDEX_OUT_OF_BOUNDS; time complexity is O(N).
```
> int jcal_popBack(JC_ArrayList* const list)
```text
Removes the element at the end.
Note: If JC_AL_FLAG_AUTO_SHRINK is enabled, auto-shrinking is triggered; if the instance is already empty, returns JC_AL__ALREADY_EMPTY.
```
> int jcal_popFront(JC_ArrayList* const list)
```text
Removes the element at the beginning.
Note: Time complexity is O(N); if the instance is already empty, returns JC_AL__ALREADY_EMPTY.
```
> int jcal_removeAt(JC_ArrayList* const list, size_t index)
```text
Removes the element at index.
Note: Out-of-bounds index returns JC_AL__INDEX_OUT_OF_BOUNDS; if JC_AL_FLAG_AUTO_SHRINK is enabled, auto-shrinking is triggered; time complexity is O(N).
```
> const char* jcal_statusToCharArray(int status)
```text
Returns a human-readable description of the given status code.
Note: The returned string is static; the caller must not and need not free it.
```