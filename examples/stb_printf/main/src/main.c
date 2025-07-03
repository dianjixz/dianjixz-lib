#include <stdio.h>
#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"  // 引入 stb_sprintf.h
#include <stdarg.h>
#include <stdio.h>

// 定义回调函数
static char *my_callback(const char *buf, void *user, int len)
{
    stbsp__context *c = (stbsp__context *)user;
    //    (void) sizeof(buf);
    fwrite(buf, 1, len, stdout);
    c->length += len;
    return c->tmp;  // go direct into buffer if you can
}

// 自定义 printf 函数
int my_printf(const char *fmt, ...)
{
    stbsp__context c = {0};
    va_list args;
    va_start(args, fmt);                                  // 启动 va_list
    stbsp_vsprintfcb(my_callback, &c, c.tmp, fmt, args);  // 调用 stbsp_vsprintfcb
    va_end(args);                                         // 结束 va_list

    return c.length;  // 返回生成的字符长度
}

int main()
{
    // 测试自定义 printf 函数
    my_printf(
        "Hello, %s! You have %d new "
        "messages."
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaasssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss"
        "ssssssssssssssssssssssssssssssssssssssssssss"
        "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"
        "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"
        "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"
        "cccccccccccccccccccccccccccccccccccccccccccc"
        "aaaaaaafffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
        "fffffffffffffffffffffffffffffffffffffff\n",
        "Alice", 5);
    my_printf("Current temperature: %.2f°C\n", 26.3);

    return 0;
}
