#include <stdio.h>
#include "pt.h"
#include <unistd.h>

static struct pt pt_led;
static struct pt pt_count;
static int count = 0;


// LED 闪烁protothread
static int protothread_led(struct pt *pt) {
    PT_BEGIN(pt);
    while (1) {
        printf("LED ON\n");
        // PT_YIELD(pt);
        // PT_WAIT_UNTIL(pt, (count % 500 == 0)); // 模拟500ms延时
        PT_YIELD_UNTIL(pt, (count % 500 == 0)); // 挂起、等待下一次调度
        printf("LED OFF\n");
        // PT_YIELD(pt); // 挂起、等待下一次调度
        // PT_WAIT_UNTIL(pt, (count % 500 == 0)); // 模拟500ms延时
        PT_YIELD_UNTIL(pt, (count % 500 == 0)); // 挂起、等待下一次调度
    }
    PT_END(pt);
}

// 计数protothread
static int protothread_count(struct pt *pt) {
    static  int old_count = 0;
    PT_BEGIN(pt);
    while (1) {
        printf("nihao\n");
        // PT_YIELD(pt);
        // PT_WAIT_UNTIL(pt, (count % 5000 == 0)); // 模拟计数变化
        PT_YIELD_UNTIL(pt, (count % 500 == 0));
    }
    PT_END(pt);
}

int main(void) {
    PT_INIT(&pt_led);
    PT_INIT(&pt_count);

    while (1) {
        usleep(1000);
        count ++;
        protothread_led(&pt_led);
        protothread_count(&pt_count);
    }
    return 0;
}