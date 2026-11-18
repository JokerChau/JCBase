#pragma once

#include "wheel_crazyString_ex.h"
#include <stddef.h>

typedef struct JC_crazyString JC_crazyString;
typedef JC_crazyString* jc_czyString;

// 三大构造
wheels jc_czyString jccstr_new(void);

wheels jc_czyString jccstr_newWithSize(size_t capacity);

wheels jc_czyString jccstr_newFrom(const char* content, size_t length);

// 析构
wheels void jccstr_delete(jc_czyString str);

// 视图，直接获取内部char*，const表明不要释放或修改，操作后出问题自行负责
wheels const char* jccstr_viewBuffer(jc_czyString str);

// 将内部char*拷贝后返回，需要自行释放free
wheels char* jccstr_toCharArray(jc_czyString str);

// 获取内容长度，与C语义一致，截至\0
wheels size_t jccstr_getLength(jc_czyString str);

// 获取容量大小，与\0无关，与char*实际占用内存有关
wheels size_t jccstr_getCapacity(jc_czyString str);

// 修改某处内容，若传入\0为内容则和endAt同一行为
wheels jc_czyString jccstr_changeAt(jc_czyString str, size_t index, char content);

// 修改一定范围的内容
wheels jc_czyString jccstr_changeFrom(jc_czyString str, size_t from, size_t length, const char* content);

// 将某处设定为字符串截止符，同将假设一char*为Hello World\0非字面量的字符串数组某处改为\0
wheels jc_czyString jccstr_endAt(jc_czyString str, size_t index);

// 将截止符设置在第一位字符位置
wheels jc_czyString jccstr_clear(jc_czyString str);

// 将索引处字符去除
wheels jc_czyString jccstr_removeAt(jc_czyString str, size_t index);

// 将左闭右开区间内字符去除
wheels jc_czyString jccstr_removeInRange(jc_czyString str, size_t from, size_t to);

// 在索引处添加一个字符
wheels jc_czyString jccstr_insertAt(jc_czyString str, size_t index, char content);

// 自索引处起，插入length长度的内容
wheels jc_czyString jccstr_insertBytes(jc_czyString str, size_t index, const char* content, size_t length);

// 直接设置内容
wheels jc_czyString jccstr_setContent(jc_czyString str, const char* content, size_t length);

// 为字符串预定空间
wheels jc_czyString jccstr_reserve(jc_czyString str, size_t wholeCapacity);

// 释放多余内存，注意，第一个\0后的内存都将释放！！！！！！！！
wheels jc_czyString jccstr_shrinkToFit(jc_czyString str);

// 重新设置串容量，多出部分将截断
wheels jc_czyString jccstr_reallocate(jc_czyString str, size_t capacity);

// 克隆一份一样的字符串实例
wheels jc_czyString jccstr_clone(jc_czyString str);

// 追加 length 字节（遇 \0 截断）
wheels jc_czyString jccstr_appendBytes(jc_czyString str, const char* content, size_t length);

// 把 other 的内容追加到 str 之后（允许 str == other 自拼接）
wheels jc_czyString jccstr_append(jc_czyString str, jc_czyString other);

// 把 a、b 拼接成一个新实例，a、b 不变
wheels jc_czyString jccstr_concat(jc_czyString a, jc_czyString b);