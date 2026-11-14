#include "JC_IO.h"
#include "JC_StringStructures.h"
#include "JC_StringSrc.h"
#include "JC_MutableStringSrc.h"
#include <limits.h>
#include <stdlib.h>
#include <stddef.h>

// 从 fp 读一行到新分配的 buffer（不 shrink）。
// 产出：
//   *outBuf    malloc 的 buffer（失败时保留已读内容，成功时非 NULL）
//   *outLen    读到的字符数
//   *outCap    实际分配字节数
//   *outStatus AVAILABLE            —— 正常读到一行（len 可为 0，即空行）
//               NULL_BUFFER          —— 初始 malloc 失败（outBuf 为 NULL）
//               REALLOCATION_FAILED  —— 扩容失败（已读内容保留）
//               READERROR            —— fgetc 遇到 I/O 错误
//               EOF                  —— 读到 EOF 前没有任何字符（无更多行）
static void readLineCore(char** outBuf, size_t* outLen, size_t* outCap, int* outStatus, FILE* fp) {
    *outBuf = NULL;
    *outLen = 0;
    *outCap = 0;
    *outStatus = JC_STR__AVAILABLE;

    char* buf = (char*)malloc(INITIAL_STRING_SIZE);
    if (!buf) {
        *outStatus = JC_STR__NULL_BUFFER;
        return;
    }
    size_t cap = INITIAL_STRING_SIZE;
    size_t len = 0;
    int ch;

    while ((ch = fgetc(fp)) != EOF) {
        if (ch == '\n') break;
        if (ch == '\r') {
            // 吞掉随后的 '\n'（Windows 换行），但不吞其它字符
            int next = fgetc(fp);
            if (next == EOF && ferror(fp)) {
                *outStatus = JC_STR__READ_ERROR;
                break;
            }
            if (next != '\n' && next != EOF) ungetc(next, fp);
            break;
        }
        if (len + 1 >= cap) {
            if (cap > SIZE_MAX / GROWTH) {
                *outStatus = JC_STR__REALLOCATION_FAILED;
                break;
            }
            size_t newCap = cap * GROWTH;
            char* newBuf = (char*)realloc(buf, newCap);
            if (!newBuf) {
                *outStatus = JC_STR__REALLOCATION_FAILED;
                break;
            }
            buf = newBuf;
            cap = newCap;
        }
        buf[len++] = (char)ch;
    }

    // 区分 EOF 的实际情况：只有正常退出循环（未因错误 break）才检查
    if (ch == EOF && *outStatus == JC_STR__AVAILABLE) {
        if (ferror(fp)) {
            *outStatus = JC_STR__READ_ERROR;
        }
        else if (len == 0) {
            *outStatus = JC_STR__EOF;
        }
        // len > 0 且正常 EOF：末行无换行符，按正常行处理
    }

    buf[len] = '\0';
    *outBuf = buf;
    *outLen = len;
    *outCap = cap;
}

jc_String jcio_readLineString(FILE* fp) {
    if (!fp) {
        jc_String temp = (jc_String)malloc(sizeof(JC_String));
        if (!temp) return NULL;
        temp->buffer = NULL;
        temp->status = JC_STR__INVALID_FILE_POINTER;
        temp->cachedLen = 0;
        return temp;
    }
    char* buf; size_t len, cap; int status;
    readLineCore(&buf, &len, &cap, &status, fp);

    jc_String temp = (jc_String)malloc(sizeof(JC_String));
    if (!temp) {
        free(buf);
        return NULL;
    }

    if (buf) {
        char* shrunk = (char*)realloc(buf, len + 1);
        temp->buffer = shrunk ? shrunk : buf;   // 失败保留原 buffer
    }
    else {
        temp->buffer = NULL;                    // NULL_BUFFER 时保持 NULL
    }
    temp->status = status;
    temp->cachedLen = len;
    return temp;
}

jc_MutableString jcio_readLineMutableString(FILE* fp) {
    if (!fp) {
        jc_MutableString temp = (jc_MutableString)malloc(sizeof(JC_MutableString));
        if (!temp) return NULL;
        temp->buffer = NULL;
        temp->status = JC_MS__INVALID_FILE_POINTER;
        temp->len = 0;
        temp->cap = 0;
        return temp;
    }
	char* buf; size_t len, cap; int status;
	readLineCore(&buf, &len, &cap, &status, fp);

    switch (status) {
    case JC_STR__AVAILABLE:
        status = JC_MS__AVAILABLE;
        break;
    case JC_STR__NULL_BUFFER:
        status = JC_MS__NULL_BUFFER;
        break;
    case JC_STR__REALLOCATION_FAILED:
        status = JC_MS__REALLOCATION_FAILED;
        break;
    case JC_STR__READ_ERROR:
        status = JC_MS__READ_ERROR;
        break;
    case JC_STR__EOF:
        status = JC_MS__EOF;
        break;
    }

    jc_MutableString ms = (jc_MutableString)malloc(sizeof(JC_MutableString));
    if (!ms) {
        free(buf);
        return NULL;
    }
    ms->buffer = buf;
    ms->cap = cap;
    ms->len = len;
    ms->status = status;
    return ms;
}

int jcio_writeLineString(FILE* fp, jc_String str) {
    if (!fp)return JC_IO__NULL_FILE;
    if (!str)return JC_STR__NULL;
    if (!str->buffer)return JC_STR__NULL_BUFFER;
    if (str->cachedLen > 0 &&
        fwrite(str->buffer, 1, str->cachedLen, fp) != str->cachedLen)
        return JC_IO__WRITE_FAILED;
    if (fputc('\n', fp) == EOF)
        return JC_IO__WRITE_FAILED;
    return JC_IO__SUCCESSFUL_OP;
}

int jcio_writeLineMutableString(FILE* fp, jc_MutableString ms) {
    if (!fp)return JC_IO__NULL_FILE;
    if (!ms)return JC_MS__NULL;
    if (!ms->buffer)return JC_MS__NULL_BUFFER;
    if (ms->len > 0 &&
        fwrite(ms->buffer, 1, ms->len, fp) != ms->len)
        return JC_IO__WRITE_FAILED;
    if (fputc('\n', fp) == EOF)
        return JC_IO__WRITE_FAILED;
    return JC_IO__SUCCESSFUL_OP;
}