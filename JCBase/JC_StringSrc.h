#pragma once

// naming rule
// NULL_<what>/<operation>_FAILED
typedef enum JC_StringStatus {
    // 实例状态（存在 list->status 里）
    JC_STR__NULL = -1,                      // NULL 实例
    JC_STR__AVAILABLE = 0,                // 可用实例
    JC_STR__NULL_BUFFER = 1,                    // 实例存在，buffer 为 NULL
    JC_STR__EMPTY,                          // 字符串内容为空
    JC_STR__BLANK,                          // 字符串内容全为空白

    // 操作成功
    JC_STR__SUCCESSFUL_OP,            // 操作成功
    JC_STR__PARTIAL_SPLIT,                  // split 部分元素构造失败

    // 内存/组合错误
    JC_STR__ALLOCATION_FAILED,        // malloc 失败
    JC_STR__REALLOCATION_FAILED,            // realloc 失败
    JC_STR__COMBINATION_FAILED,             // 拼接时分配失败
    JC_STR__LEN_OUT_OF_TYPE_MAX,            // 长度溢出 size_t

    // 文件IO错误
    JC_STR__READ_ERROR,                      // 读取出错
    JC_STR__EOF,                             // EOF

    // 参数错误
    JC_STR__NULL_CHAR,                // char* 输出参数为 NULL
    JC_STR__NULL_ARRAY,                     // 数组输出参数为 NULL
    JC_STR__NULL_RECEIVER,                  // 结果接收指针为 NULL
    JC_STR__NULL_TARGET,                    // target 为 NULL
    JC_STR__NULL_COUNT,                     // count 输出参数为 NULL
    JC_STR__INVALID_FOLLOWER,               // 追加参数实例为 NULL
    JC_STR__INVALID_FOLLOWER_BUFFER,        // 追加参数 buffer 为 NULL
    JC_STR__INVALID_REPLACEMENT,            // replacement 为 NULL 或无 buffer
    JC_STR__INVALID_START,                  // 起始下标无效
    JC_STR__INVALID_END,                    // 结束下标无效
    JC_STR__INVALID_IDX,                    // 区间 start > end
    JC_STR__IDX_OUT_BOUNDS,                 // 下标越界
    JC_STR__WITHOUT_CH,                     // 未找到目标字符/子串
    JC_STR__INVALID_FILE_POINTER,           // 文件指针无效

    // 数字解析：输出指针为 NULL
    JC_STR__NULL_INT,
    JC_STR__NULL_LONG,
    JC_STR__NULL_LONGLONG,
    JC_STR__NULL_FLOAT,
    JC_STR__NULL_DOUBLE,

    // 数字解析：格式错误
    JC_STR__NOT_INT,
    JC_STR__NOT_LONG,
    JC_STR__NOT_LONGLONG,
    JC_STR__NOT_FLOAT,
    JC_STR__NOT_DOUBLE,

    // 数字解析：溢出
    JC_STR__OVERFLOW_INT,
    JC_STR__OVERFLOW_LONG,
    JC_STR__OVERFLOW_LONGLONG,
    JC_STR__OVERFLOW_FLOAT,
    JC_STR__OVERFLOW_DOUBLE,
} JC_StringStatus;