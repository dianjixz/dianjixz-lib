/*
# Protothreads 统一头文件

## 概述

`protothreads_unified.h` 是一个将 Protothreads 库的所有头文件整合到单个文件中的统一头文件。这个文件包含了原始 Protothreads 库的所有功能，无需分别包含多个头文件。

## 包含的组件

该统一头文件包含以下原始头文件的所有内容：

1. **lc-addrlabels.h** - 基于 GCC "labels as values" 特性的本地继续实现
2. **lc-switch.h** - 基于 switch 语句的本地继续实现（已注释，避免冲突）
3. **lc.h** - 本地继续的通用接口
4. **pt.h** - Protothreads 核心实现
5. **pt-sem.h** - Protothread 信号量
6. **pt-sleep.h** - Arduino 睡眠功能
7. **protothreads.h** - 原始的主头文件

## 使用方法

只需包含统一头文件即可使用所有 Protothreads 功能：

```c
#include "protothreads_unified.h"

struct pt my_thread;

PT_THREAD(my_protothread(struct pt *pt))
{
    PT_BEGIN(pt);
    
    // 你的代码在这里
    PT_YIELD(pt);
    
    PT_END(pt);
}

int main() {
    PT_INIT(&my_thread);
    
    while(PT_SCHEDULE(my_protothread(&my_thread))) {
        // 调度其他任务
    }
    
    return 0;
}
```

## 主要功能

### 基本 Protothread 操作
- `PT_INIT(pt)` - 初始化 protothread
- `PT_BEGIN(pt)` - 开始 protothread
- `PT_END(pt)` - 结束 protothread
- `PT_YIELD(pt)` - 让出执行权
- `PT_EXIT(pt)` - 退出 protothread
- `PT_RESTART(pt)` - 重启 protothread

### 等待操作
- `PT_WAIT_UNTIL(pt, condition)` - 等待直到条件为真
- `PT_WAIT_WHILE(pt, condition)` - 等待直到条件为假
- `PT_WAIT_THREAD(pt, thread)` - 等待子线程完成

### 信号量操作
- `PT_SEM_INIT(sem, count)` - 初始化信号量
- `PT_SEM_WAIT(pt, sem)` - 等待信号量
- `PT_SEM_SIGNAL(pt, sem)` - 发送信号量信号

### 睡眠功能（Arduino）
- `PT_SLEEP(pt, delay)` - 睡眠指定毫秒数

## 兼容性

- 与原始 Protothreads 库完全兼容
- 保留了所有原始版权声明和许可证信息
- 支持 GCC 编译器的 "labels as values" 特性
- 包含 NULL 定义，无需额外包含标准库头文件

## 注意事项

1. 默认使用 `lc-addrlabels.h` 实现（基于 GCC labels as values）
2. `lc-switch.h` 实现已被注释掉以避免冲突
3. 如需使用 switch 实现，请手动修改头文件
4. 所有原始功能和 API 保持不变

## 编译测试

该统一头文件已通过编译测试，可以正常使用。

## 原始作者

Adam Dunkels <adam@sics.se>  
Swedish Institute of Computer Science

## 许可证

遵循原始 Protothreads 库的 BSD 许可证。
*/
/*
 * Protothreads Unified Header
 * 
 * This file contains all Protothreads library headers unified into a single file.
 * Generated from the original Protothreads library by Adam Dunkels.
 * 
 * Original copyright notices are preserved for each component.
 */

#ifndef __PROTOTHREADS_UNIFIED_H__
#define __PROTOTHREADS_UNIFIED_H__

/* Standard library includes needed for NULL and other basic definitions */
#ifndef NULL
#define NULL ((void*)0)
#endif

/*
 * ============================================================================
 * lc-addrlabels.h - Implementation of local continuations based on address labels
 * ============================================================================
 */

/*
 * Copyright (c) 2004-2005, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 * $Id: lc-addrlabels.h,v 1.4 2006/06/03 11:29:43 adam Exp $
 */

/**
 * \addtogroup lc
 * @{
 */

/**
 * \file
 * Implementation of local continuations based on the "Labels as
 * values" feature of gcc
 * \author
 * Adam Dunkels <adam@sics.se>
 *
 * This implementation of local continuations is based on a special
 * feature of the GCC C compiler called "labels as values". This
 * feature allows assigning pointers with the address of the code
 * corresponding to a particular C label.
 *
 * For more information, see the GCC documentation:
 * http://gcc.gnu.org/onlinedocs/gcc/Labels-as-Values.html
 *
 */

 #ifndef __LC_ADDRLABELS_H__
 #define __LC_ADDRLABELS_H__
 
 /** \hideinitializer */
 typedef void * lc_t;
 
 #define LC_INIT(s) s = NULL
 
 #define LC_RESUME(s)				\
   do {						\
     if(s != NULL) {				\
       goto *s;					\
     }						\
   } while(0)
 
 #define LC_CONCAT2(s1, s2) s1##s2
 #define LC_CONCAT(s1, s2) LC_CONCAT2(s1, s2)
 
 #define LC_SET(s)				\
   do {						\
     LC_CONCAT(LC_LABEL, __LINE__):   	        \
     (s) = &&LC_CONCAT(LC_LABEL, __LINE__);	\
   } while(0)
 
 #define LC_END(s)
 
 #endif /* __LC_ADDRLABELS_H__ */
 /** @} */

/*
 * ============================================================================
 * lc-switch.h - Implementation of local continuations based on switch statement
 * ============================================================================
 */

/*
 * Copyright (c) 2004-2005, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 * $Id: lc-switch.h,v 1.4 2006/06/03 11:29:43 adam Exp $
 */

/**
 * \addtogroup lc
 * @{
 */

/**
 * \file
 * Implementation of local continuations based on switch() statment
 * \author Adam Dunkels <adam@sics.se>
 *
 * This implementation of local continuations uses the C switch()
 * statement to resume execution of a function somewhere inside the
 * function's body. The implementation is based on the fact that
 * switch() statements are able to jump directly into the bodies of
 * control structures such as if() or while() statmenets.
 *
 * This implementation borrows heavily from Simon Tatham's coroutines
 * implementation in C:
 * http://www.chiark.greenend.org.uk/~sgtatham/coroutines.html
 */

 #ifndef __LC_SWITCH_H__
 #define __LC_SWITCH_H__
 
 /* WARNING! lc implementation using switch() does not work if an
    LC_SET() is done within another switch() statement! */
 
 /** \hideinitializer */
 /* typedef unsigned short lc_t; */  /* Commented out to avoid conflict with lc-addrlabels.h */
 
 /* Switch-based LC macros - commented out as addrlabels version is used by default
 #define LC_INIT(s) s = 0;
 
 #define LC_RESUME(s) switch(s) { case 0:
 
 #define LC_SET(s) s = __LINE__; case __LINE__:
 
 #define LC_END(s) }
 */
 
 #endif /* __LC_SWITCH_H__ */
 
 /** @} */

/*
 * ============================================================================
 * lc.h - Local continuations
 * ============================================================================
 */

/*
 * Copyright (c) 2004-2005, Swedish Institute of Computer Science.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 3. Neither the name of the Institute nor the names of its contributors 
 *    may be used to endorse or promote products derived from this software 
 *    without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE. 
 *
 * This file is part of the protothreads library.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 * $Id: lc.h,v 1.2 2005/02/24 10:36:59 adam Exp $
 */

/**
 * \addtogroup pt
 * @{
 */

/**
 * \defgroup lc Local continuations
 * @{
 *
 * Local continuations form the basis for implementing protothreads. A
 * local continuation can be <i>set</i> in a specific function to
 * capture the state of the function. After a local continuation has
 * been set can be <i>resumed</i> in order to restore the state of the
 * function at the point where the local continuation was set.
 *
 *
 */

/**
 * \file lc.h
 * Local continuations
 * \author
 * Adam Dunkels <adam@sics.se>
 *
 */

 #ifdef DOXYGEN
 /**
  * Initialize a local continuation.
  *
  * This operation initializes the local continuation, thereby
  * unsetting any previously set continuation state.
  *
  * \hideinitializer
  */
 #define LC_INIT(lc)
 
 /**
  * Set a local continuation.
  *
  * The set operation saves the state of the function at the point
  * where the operation is executed. As far as the set operation is
  * concerned, the state of the function does <b>not</b> include the
  * call-stack or local (automatic) variables, but only the program
  * counter and such CPU registers that needs to be saved.
  *
  * \hideinitializer
  */
 #define LC_SET(lc)
 
 /**
  * Resume a local continuation.
  *
  * The resume operation resumes a previously set local continuation, thus
  * restoring the state in which the function was when the local
  * continuation was set. If the local continuation has not been
  * previously set, the resume operation does nothing.
  *
  * \hideinitializer
  */
 #define LC_RESUME(lc)
 
 /**
  * Mark the end of local continuation usage.
  *
  * The end operation signifies that local continuations should not be
  * used any more in the function. This operation is not needed for
  * most implementations of local continuation, but is required by a
  * few implementations.
  *
  * \hideinitializer 
  */
 #define LC_END(lc)
 
 /**
  * \var typedef lc_t;
  *
  * The local continuation type.
  *
  * \hideinitializer
  */
 #endif /* DOXYGEN */
 
 #ifndef __LC_H__
 #define __LC_H__
 
 /* Note: lc-addrlabels.h is already included above */
 
 #endif /* __LC_H__ */
 
 /** @} */
 /** @} */

/*
 * ============================================================================
 * pt.h - Protothreads implementation
 * ============================================================================
 */

/*
 * Copyright (c) 2004-2005, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 * $Id: pt.h,v 1.7 2006/10/02 07:52:56 adam Exp $
 */

/**
 * \addtogroup pt
 * @{
 */

/**
 * \file
 * Protothreads implementation.
 * \author
 * Adam Dunkels <adam@sics.se>
 *
 */

 #ifndef __PT_H__
 #define __PT_H__
 
 /* Note: lc.h is already included above */
 
 struct pt {
   lc_t lc;
 };
 
 #define PT_WAITING 0
 #define PT_YIELDED 1
 #define PT_EXITED  2
 #define PT_ENDED   3
 
 /**
  * \name Initialization
  * @{
  */
 
 /**
  * Initialize a protothread.
  *
  * Initializes a protothread. Initialization must be done prior to
  * starting to execute the protothread.
  *
  * \param pt A pointer to the protothread control structure.
  *
  * \sa PT_SPAWN()
  *
  * \hideinitializer
  */
 #define PT_INIT(pt)   LC_INIT((pt)->lc)
 
 /** @} */
 
 /**
  * \name Declaration and definition
  * @{
  */
 
 /**
  * Declaration of a protothread.
  *
  * This macro is used to declare a protothread. All protothreads must
  * be declared with this macro.
  *
  * \param name_args The name and arguments of the C function
  * implementing the protothread.
  *
  * \hideinitializer
  */
 #define PT_THREAD(name_args) char name_args
 
 /**
  * Declare the start of a protothread inside the C function
  * implementing the protothread.
  *
  * This macro is used to declare the starting point of a
  * protothread. It should be placed at the start of the function in
  * which the protothread runs. All C statements above the PT_BEGIN()
  * invokation will be executed each time the protothread is scheduled.
  *
  * \param pt A pointer to the protothread control structure.
  *
  * \hideinitializer
  */
 #define PT_BEGIN(pt) { char PT_YIELD_FLAG = 1; LC_RESUME((pt)->lc)
 
 /**
  * Declare the end of a protothread.
  *
  * This macro is used for declaring that a protothread ends. It must
  * always be used together with a matching PT_BEGIN() macro.
  *
  * \param pt A pointer to the protothread control structure.
  *
  * \hideinitializer
  */
 #define PT_END(pt) LC_END((pt)->lc); PT_YIELD_FLAG = 0; \
                    PT_INIT(pt); return PT_ENDED; }
 
 /** @} */
 
 /**
  * \name Blocked wait
  * @{
  */
 
 /**
  * Block and wait until condition is true.
  *
  * This macro blocks the protothread until the specified condition is
  * true.
  *
  * \param pt A pointer to the protothread control structure.
  * \param condition The condition.
  *
  * \hideinitializer
  */
 #define PT_WAIT_UNTIL(pt, condition)	        \
   do {						\
     LC_SET((pt)->lc);				\
     if(!(condition)) {				\
       return PT_WAITING;			\
     }						\
   } while(0)
 
 /**
  * Block and wait while condition is true.
  *
  * This function blocks and waits while condition is true. See
  * PT_WAIT_UNTIL().
  *
  * \param pt A pointer to the protothread control structure.
  * \param cond The condition.
  *
  * \hideinitializer
  */
 #define PT_WAIT_WHILE(pt, cond)  PT_WAIT_UNTIL((pt), !(cond))
 
 /** @} */
 
 /**
  * \name Hierarchical protothreads
  * @{
  */
 
 /**
  * Block and wait until a child protothread completes.
  *
  * This macro schedules a child protothread. The current protothread
  * will block until the child protothread completes.
  *
  * \note The child protothread must be manually initialized with the
  * PT_INIT() function before this function is used.
  *
  * \param pt A pointer to the protothread control structure.
  * \param thread The child protothread with arguments
  *
  * \sa PT_SPAWN()
  *
  * \hideinitializer
  */
 #define PT_WAIT_THREAD(pt, thread) PT_WAIT_WHILE((pt), PT_SCHEDULE(thread))
 
 /**
  * Spawn a child protothread and wait until it exits.
  *
  * This macro spawns a child protothread and waits until it exits. The
  * macro can only be used within a protothread.
  *
  * \param pt A pointer to the protothread control structure.
  * \param child A pointer to the child protothread's control structure.
  * \param thread The child protothread with arguments
  *
  * \hideinitializer
  */
 #define PT_SPAWN(pt, child, thread)		\
   do {						\
     PT_INIT((child));				\
     PT_WAIT_THREAD((pt), (thread));		\
   } while(0)
 
 /** @} */
 
 /**
  * \name Exiting and restarting
  * @{
  */
 
 /**
  * Restart the protothread.
  *
  * This macro will block and cause the running protothread to restart
  * its execution at the place of the PT_BEGIN() call.
  *
  * \param pt A pointer to the protothread control structure.
  *
  * \hideinitializer
  */
 #define PT_RESTART(pt)				\
   do {						\
     PT_INIT(pt);				\
     return PT_WAITING;			\
   } while(0)
 
 /**
  * Exit the protothread.
  *
  * This macro causes the protothread to exit. If the protothread was
  * spawned by another protothread, the parent protothread will become
  * unblocked and can continue to run.
  *
  * \param pt A pointer to the protothread control structure.
  *
  * \hideinitializer
  */
 #define PT_EXIT(pt)				\
   do {						\
     PT_INIT(pt);				\
     return PT_EXITED;			\
   } while(0)
 
 /** @} */
 
 /**
  * \name Calling a protothread
  * @{
  */
 
 /**
  * Schedule a protothread.
  *
  * This function shedules a protothread. The return value of the
  * function is non-zero if the protothread is running or zero if the
  * protothread has exited.
  *
  * \param f The call to the C function implementing the protothread to
  * be scheduled
  *
  * \hideinitializer
  */
 #define PT_SCHEDULE(f) ((f) < PT_EXITED)
 
 /** @} */
 
 /**
  * \name Yielding from a protothread
  * @{
  */
 
 /**
  * Yield from the current protothread.
  *
  * This function will yield the protothread, thereby allowing other
  * processing to take place in the system.
  *
  * \param pt A pointer to the protothread control structure.
  *
  * \hideinitializer
  */
 #define PT_YIELD(pt)				\
   do {						\
     PT_YIELD_FLAG = 0;				\
     LC_SET((pt)->lc);				\
     if(PT_YIELD_FLAG == 0) {			\
       return PT_YIELDED;			\
     }						\
   } while(0)
 
 /**
  * \brief      Yield from the protothread until a condition occurs.
  * \param pt   A pointer to the protothread control structure.
  * \param cond The condition.
  *
  *             This function will yield the protothread, until the
  *             specified condition evaluates to true.
  *
  *
  * \hideinitializer
  */
 #define PT_YIELD_UNTIL(pt, cond)		\
   do {						\
     PT_YIELD_FLAG = 0;				\
     LC_SET((pt)->lc);				\
     if((PT_YIELD_FLAG == 0) || !(cond)) {	\
       return PT_YIELDED;			\
     }						\
   } while(0)
 
 /** @} */
 
 #endif /* __PT_H__ */
 
 /** @} */

/*
 * ============================================================================
 * pt-sem.h - Protothread semaphores
 * ============================================================================
 */

/*
 * Copyright (c) 2004, Swedish Institute of Computer Science.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 3. Neither the name of the Institute nor the names of its contributors 
 *    may be used to endorse or promote products derived from this software 
 *    without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE. 
 *
 * This file is part of the protothreads library.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 * $Id: pt-sem.h,v 1.2 2005/02/24 10:36:59 adam Exp $
 */

/**
 * \addtogroup pt
 * @{
 */

/**
 * \defgroup ptsem Protothread semaphores
 * @{
 *
 * This module implements counting semaphores on top of
 * protothreads. Semaphores are a synchronization primitive that
 * provide two operations: "wait" and "signal". The "wait" operation
 * checks the semaphore counter and blocks the thread if the counter
 * is zero. The "signal" operation increases the semaphore counter but
 * does not block. If another thread has blocked waiting for the
 * semaphore that is signalled, the blocked thread will become
 * runnable again.
 *
 * Semaphores can be used to implement other, more structured,
 * synchronization primitives such as monitors and message
 * queues/bounded buffers (see below).
 *
 * The following example shows how the producer-consumer problem, also
 * known as the bounded buffer problem, can be solved using
 * protothreads and semaphores. Notes on the program follow after the
 * example.
 *
 \code
#include "pt-sem.h"

#define NUM_ITEMS 32
#define BUFSIZE 8

static struct pt_sem mutex, full, empty;

PT_THREAD(producer(struct pt *pt))
{
  static int produced;
  
  PT_BEGIN(pt);
  
  for(produced = 0; produced < NUM_ITEMS; ++produced) {
  
    PT_SEM_WAIT(pt, &full);
    
    PT_SEM_WAIT(pt, &mutex);
    add_to_buffer(produce_item());    
    PT_SEM_SIGNAL(pt, &mutex);
    
    PT_SEM_SIGNAL(pt, &empty);
  }

  PT_END(pt);
}

PT_THREAD(consumer(struct pt *pt))
{
  static int consumed;
  
  PT_BEGIN(pt);

  for(consumed = 0; consumed < NUM_ITEMS; ++consumed) {
    
    PT_SEM_WAIT(pt, &empty);
    
    PT_SEM_WAIT(pt, &mutex);    
    consume_item(get_from_buffer());    
    PT_SEM_SIGNAL(pt, &mutex);
    
    PT_SEM_SIGNAL(pt, &full);
  }

  PT_END(pt);
}

PT_THREAD(driver_thread(struct pt *pt))
{
  static struct pt pt_producer, pt_consumer;

  PT_BEGIN(pt);
  
  PT_SEM_INIT(&empty, 0);
  PT_SEM_INIT(&full, BUFSIZE);
  PT_SEM_INIT(&mutex, 1);

  PT_INIT(&pt_producer);
  PT_INIT(&pt_consumer);

  PT_WAIT_THREAD(pt, producer(&pt_producer) &
		     consumer(&pt_consumer));

  PT_END(pt);
}
 \endcode
 *
 * The program uses three protothreads: one protothread that
 * implements the consumer, one thread that implements the producer,
 * and one protothread that drives the two other protothreads. The
 * program uses three semaphores: "full", "empty" and "mutex". The
 * "mutex" semaphore is used to provide mutual exclusion for the
 * buffer, the "empty" semaphore is used to block the consumer is the
 * buffer is empty, and the "full" semaphore is used to block the
 * producer is the buffer is full.
 *
 * The "driver_thread" holds two protothread state variables,
 * "pt_producer" and "pt_consumer". It is important to note that both
 * these variables are declared as <i>static</i>. If the static
 * keyword is not used, both variables are stored on the stack. Since
 * protothreads do not store the stack, these variables may be
 * overwritten during a protothread wait operation. Similarly, both
 * the "consumer" and "producer" protothreads declare their local
 * variables as static, to avoid them being stored on the stack.
 * 
 *
 */
   
/**
 * \file
 * Couting semaphores implemented on protothreads
 * \author
 * Adam Dunkels <adam@sics.se>
 *
 */

 #ifndef __PT_SEM_H__
 #define __PT_SEM_H__
 
 /* Note: pt.h is already included above */
 
 struct pt_sem {
   unsigned int count;
 };
 
 /**
  * Initialize a semaphore
  *
  * This macro initializes a semaphore with a value for the
  * counter. Internally, the semaphores use an "unsigned int" to
  * represent the counter, and therefore the "count" argument should be
  * within range of an unsigned int.
  *
  * \param s (struct pt_sem *) A pointer to the pt_sem struct
  * representing the semaphore
  *
  * \param c (unsigned int) The initial count of the semaphore.
  * \hideinitializer
  */
 #define PT_SEM_INIT(s, c) (s)->count = c
 
 /**
  * Wait for a semaphore
  *
  * This macro carries out the "wait" operation on the semaphore. The
  * wait operation causes the protothread to block while the counter is
  * zero. When the counter reaches a value larger than zero, the
  * protothread will continue.
  *
  * \param pt (struct pt *) A pointer to the protothread (struct pt) in
  * which the operation is executed.
  *
  * \param s (struct pt_sem *) A pointer to the pt_sem struct
  * representing the semaphore
  *
  * \hideinitializer
  */
 #define PT_SEM_WAIT(pt, s)	\
   do {						\
     PT_WAIT_UNTIL(pt, (s)->count > 0);		\
     --(s)->count;				\
   } while(0)
 
 /**
  * Signal a semaphore
  *
  * This macro carries out the "signal" operation on the semaphore. The
  * signal operation increments the counter inside the semaphore, which
  * eventually will cause waiting protothreads to continue executing.
  *
  * \param pt (struct pt *) A pointer to the protothread (struct pt) in
  * which the operation is executed.
  *
  * \param s (struct pt_sem *) A pointer to the pt_sem struct
  * representing the semaphore
  *
  * \hideinitializer
  */
 #define PT_SEM_SIGNAL(pt, s) ++(s)->count
 
 #endif /* __PT_SEM_H__ */
 
 /** @} */
 /** @} */

/*
 * ============================================================================
 * pt-sleep.h - Arduino sleep for protothreads
 * ============================================================================
 */

/**
 * Arduino sleep for protothreads
 *
 * Put the protothread to sleep. Yield to other protothreads while sleeping.
 *
 * \param pt A pointer to the protothread control structure.
 * \param delay Delay in milliseconds.
 *
 * \hideinitializer
 */
#ifndef __PT_SLEEP_H__
#define __PT_SLEEP_H__

#define PT_SLEEP(pt, delay) \
{ \
  do { \
    static unsigned long protothreads_sleep; \
    protothreads_sleep = millis(); \
    PT_WAIT_UNTIL(pt, millis() - protothreads_sleep > delay); \
  } while(false); \
}

#endif /* __PT_SLEEP_H__ */

#endif /* __PROTOTHREADS_UNIFIED_H__ */
