#include <stdio.h>
#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h" // 引入 stb_sprintf.h
#include <stdarg.h>
#include <stdio.h>

// 定义回调函数
int my_callback(const char *buf, void *user, int len) {
    // 将格式化后的数据输出到终端
    fwrite(buf, 1, len, stdout);
    return len; // 返回处理的字符长度
}

// 自定义 printf 函数
int my_printf(const char *fmt, ...) {
    char temp_buffer[1024]; // 临时缓冲区（stbsp_vsprintfcb 需要）
    va_list args;

    va_start(args, fmt); // 启动 va_list
    int result = stbsp_vsprintfcb(my_callback, NULL, temp_buffer, fmt, args); // 调用 stbsp_vsprintfcb
    va_end(args); // 结束 va_list

    return result; // 返回生成的字符长度
}

int main() {
    // 测试自定义 printf 函数
    my_printf("Hello, %s! You have %d new messages.\n", "Alice", 5);
    my_printf("Current temperature: %.2f°C\n", 26.3);

    return 0;
}
