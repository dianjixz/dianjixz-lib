# Config enable component3 or not in Kconfig
set(PRIVATE_MODULE_DIR "../../github_source")
if(CONFIG_TBOX_ENABLED)


    list(APPEND ADD_SRCS  
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/tbox.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/hash/bkdr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/hash/fnv32.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/hash/adler32.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/math/fixed16.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/math/impl/math.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/math/int32.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/math/random/linear.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/math/random/random.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/misc/time/gmmktime.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/misc/time/gmtime.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/misc/time/localtime.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/misc/time/mktime.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/misc/time/time.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/stdio/getchar.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/stdio/printf.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/stdio/printf_object.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/stdio/putchar.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/stdio/puts.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/stdio/snprintf.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/stdio/sprintf.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/stdio/swprintf.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/stdio/vsnprintf.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/stdio/vswprintf.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/stdio/wprintf.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/stdio/wputs.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/stdlib/mbstowcs.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/stdlib/random.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/stdlib/stdlib.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/stdlib/wcstombs.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/memcmp.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/memcpy.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/memdup.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/memmov.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/memmem.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/memset.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/strcat.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/strchr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/strcmp.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/strcpy.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/strdup.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/strichr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/stricmp.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/strirchr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/strirstr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/stristr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/strlcpy.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/strlen.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/strncat.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/strnchr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/strncmp.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/strncpy.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/strndup.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/strnichr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/strnicmp.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/strnirchr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/strnirstr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/strnistr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/strnlen.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/strnrchr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/strnrstr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/strnstr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/strrchr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/strrstr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/wcscat.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/strstr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/wcschr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/wcscmp.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/wcscpy.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/wcsdup.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/wcsichr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/wcsicmp.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/wcsirchr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/wcsirstr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/wcsistr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/wcslcpy.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/wcslen.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/wcsncat.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/wcsncmp.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/wcsncpy.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/wcsndup.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/wcsnicmp.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/wcsnirchr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/wcsnirstr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/wcsnlen.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/wcsnrchr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/wcsnrstr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/wcsrchr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/wcsrstr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/wcsstr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/impl/libc.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/utils/base32.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/utils/base64.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/utils/bits.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/utils/dump.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/utils/lock_profiler.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/utils/singleton.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/utils/trace.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/utils/url.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/utils/used.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/prefix/state.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/memory/allocator.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/memory/buffer.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/memory/default_allocator.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/memory/fixed_pool.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/memory/impl/memory.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/memory/impl/native_large_allocator.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/memory/impl/prefix.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/memory/impl/static_fixed_pool.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/memory/impl/static_large_allocator.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/memory/large_allocator.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/memory/native_allocator.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/memory/queue_buffer.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/memory/small_allocator.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/memory/static_allocator.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/memory/static_buffer.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/memory/string_pool.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/memory/virtual_allocator.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/string/static_string.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/string/string.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/stream/filter.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/stream/static_stream.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/stream/stream.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/stream/transfer.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/stream/impl/stream/data.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/stream/impl/stream/file.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/stream/impl/stream/filter.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/stream/impl/stream/http.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/stream/impl/stream/sock.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/stream/impl/filter/cache.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/stream/impl/filter/chunked.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/network/cookies.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/network/http.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/network/hwaddr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/network/ipaddr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/network/ipv4.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/network/ipv6.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/network/unixaddr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/network/url.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/network/impl/network.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/network/impl/http/date.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/network/impl/http/method.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/network/impl/http/option.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/network/impl/http/status.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/network/dns/cache.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/network/dns/looker.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/network/dns/server.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/algorithm/binary_find.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/algorithm/binary_find_if.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/algorithm/bubble_sort.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/algorithm/count.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/algorithm/count_if.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/algorithm/distance.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/algorithm/find.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/algorithm/find_if.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/algorithm/heap_sort.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/algorithm/insert_sort.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/algorithm/predicate.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/algorithm/quick_sort.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/algorithm/remove.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/algorithm/remove_first.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/algorithm/remove_first_if.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/algorithm/remove_if.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/algorithm/rfind.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/algorithm/rfind_if.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/algorithm/rwalk.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/algorithm/sort.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/algorithm/walk.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/array_iterator.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/bloom_filter.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/circle_queue.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/hash_map.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/hash_set.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/heap.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/iterator.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/list.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/list_entry.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/priority_queue.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/queue.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/single_list.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/single_list_entry.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/stack.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/vector.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/element/hash.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/element/long.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/element/null.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/element/size.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/element/true.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/element/uint8.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/element/uint16.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/element/uint32.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/element/mem.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/element/ptr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/element/str.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/impl/libm.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/ilog2i.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/isqrti.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/isqrti64.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/idivi8.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/addrinfo.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/atomic64.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/backtrace.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/cache_time.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/cpu.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/directory.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/dynamic.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/environment.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/event.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/file.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/filelock.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/fwatcher.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/hostname.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/ifaddrs.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/ltimer.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/mutex.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/native_memory.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/page.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/path.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/pipe.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/poller.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/print.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/process.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/sched.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/semaphore.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/socket.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/stdfile.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/syserror.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/thread.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/thread_local.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/thread_pool.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/time.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/timer.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/virtual_memory.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/impl/platform.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/impl/pollerdata.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/impl/dns.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/acos.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/acosf.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/asin.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/asinf.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/atan.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/atan2.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/atan2f.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/atanf.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/cos.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/cosf.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/exp.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/exp1.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/exp1f.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/expf.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/expi.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/expif.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/fmod.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/fmodf.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/isfin.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/isfinf.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/isinf.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/isinff.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/isnan.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/isnanf.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/log2.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/log2f.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/pow.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/powf.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/sin.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/sincos.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/sincosf.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/sinf.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/sqrt.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/sqrtf.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/tan.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/tanf.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/demo.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/libc/mbstowcs.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/libc/stdlib.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/libc/string.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/libc/time.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/libc/wchar.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/libc/wcstombs.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/libm/integer.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/math/random.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/utils/base32.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/utils/base64.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/utils/bits.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/utils/dump.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/utils/url.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/other/test.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/other/test.cpp"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/string/static_string.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/string/string.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/memory/buffer.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/memory/check.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/memory/default_allocator.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/memory/fixed_pool.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/memory/impl/static_fixed_pool.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/memory/large_pool.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/memory/memops.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/memory/queue_buffer.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/memory/small_allocator.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/memory/static_buffer.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/memory/string_pool.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/container/bloom_filter.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/container/circle_queue.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/container/hash_map.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/container/hash_set.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/container/heap.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/container/list.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/container/list_entry.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/container/queue.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/container/single_list.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/container/single_list_entry.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/container/stack.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/container/vector.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/algorithm/find.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/algorithm/sort.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/stream/stream.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/stream/stream/cache.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/stream/stream/charset.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/stream/stream/null.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/stream/stream/zip.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/network/cookies.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/network/dns.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/network/http.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/network/hwaddr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/network/impl/date.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/network/ipaddr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/network/ipv4.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/network/ipv6.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/network/ping.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/network/unixaddr.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/network/unix_echo_client.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/network/unix_echo_server.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/network/url.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/network/whois.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/math/fixed.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/libm/float.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/libm/double.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/addrinfo.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/atomic.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/atomic32.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/atomic64.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/backtrace.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/cache_time.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/directory.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/environment.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/event.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/file.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/filelock.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/fwatcher.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/hostname.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/ifaddrs.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/lock.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/ltimer.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/named_pipe.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/path.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/pipe_pair.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/poller_client.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/poller_fwatcher.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/poller_pipe.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/poller_process.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/poller_server.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/process.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/sched.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/semaphore.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/stdfile.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/thread.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/thread_local.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/thread_pool.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/timer.c"
    "${PRIVATE_MODULE_DIR}/tbox/src/demo/platform/utils.c"
    )


    # file(GLOB_RECURSE TBOX_SOURCE_FILES "${PRIVATE_MODULE_DIR}/tbox/src/tbox/algorithm/*.c"
    #                                     "${PRIVATE_MODULE_DIR}/tbox/src/tbox/charset/*.c"
    #                                     "${PRIVATE_MODULE_DIR}/tbox/src/tbox/container/*.c"
    #                                     "${PRIVATE_MODULE_DIR}/tbox/src/tbox/coroutine/*.c"
    #                                     "${PRIVATE_MODULE_DIR}/tbox/src/tbox/hash/*.c"
    #                                     "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libm/*.c"
    #                                     "${PRIVATE_MODULE_DIR}/tbox/src/tbox/math/*.c"
    #                                     "${PRIVATE_MODULE_DIR}/tbox/src/tbox/memory/*.c"
    #                                     # # "${PRIVATE_MODULE_DIR}/tbox/src/tbox/network/*.c"
    #                                     "${PRIVATE_MODULE_DIR}/tbox/src/tbox/object/*.c"
    #                                     # "${PRIVATE_MODULE_DIR}/tbox/src/tbox/regex/*.c"
    #                                     "${PRIVATE_MODULE_DIR}/tbox/src/tbox/stream/*.c"
    #                                     "${PRIVATE_MODULE_DIR}/tbox/src/tbox/string/*.c"
    #                                     "${PRIVATE_MODULE_DIR}/tbox/src/tbox/utils/*.c"
    #                                     "${PRIVATE_MODULE_DIR}/tbox/src/tbox/xml/*.c"
    #                                     # "${PRIVATE_MODULE_DIR}/tbox/src/tbox/zip/*.c"                                        
    # )
    # list(APPEND ADD_SRCS  "${TBOX_SOURCE_FILES}")
    # append_srcs_dir(ADD_SRCS "${PRIVATE_MODULE_DIR}/tbox/src/tbox")


    #platform
    # list(APPEND ADD_PRIVATE_INCLUDE "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform"
    #                                 "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/impl"
    #                                 "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/misc"
    #                                 "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/misc/time"
    #                                 "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/stdio"
    #                                 "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/stdlib"
    #                                 "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string"
    #                                 "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/impl"
    #                                 "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/impl/x86"
    # )
    # append_srcs_dir(ADD_SRCS "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform"
    #                          "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/impl"
    #                          "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/libc"
    #                          "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/linux"
    #                          "${PRIVATE_MODULE_DIR}/tbox/src/tbox/platform/posix"
    # )   

    # database
    # list(APPEND ADD_PRIVATE_INCLUDE "${PRIVATE_MODULE_DIR}/tbox/src/tbox/database"
    #                                 "${PRIVATE_MODULE_DIR}/tbox/src/tbox/database/impl")
    # append_srcs_dir(ADD_SRCS "${PRIVATE_MODULE_DIR}/tbox/src/tbox/database"
    #                          "${PRIVATE_MODULE_DIR}/tbox/src/tbox/database/impl")

    # libc
    # list(APPEND ADD_PRIVATE_INCLUDE "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc"
    #                                 "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/impl"
    #                                 "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/misc"
    #                                 "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/misc/time"
    #                                 "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/stdio"
    #                                 "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/stdlib"
    #                                 "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string"
    #                                 "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/impl"
    #                                 "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/impl/x86"
    # )
    # append_srcs_dir(ADD_SRCS "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc"
    #                          "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/impl"
    #                          "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/misc"
    #                          "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/misc/time"
    #                          "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/stdio"
    #                          "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/stdlib"
    #                          "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string"
    #                          "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/impl"
    #                          "${PRIVATE_MODULE_DIR}/tbox/src/tbox/libc/string/impl/x86"
    # )



    ################# Add include #################

	list(APPEND ADD_INCLUDE "include")
    ###############################################

    ############## Add source files ###############
	append_srcs_dir(ADD_SRCS "src")
    # list(APPEND ADD_SRCS  "log.c/src/superlog.c"
    #     )
    # FILE(GLOB_RECURSE EXTRA_SRC  "src/*.c")
    # FILE(GLOB EXTRA_SRC  "src/*.c")
    # list(APPEND ADD_SRCS  ${EXTRA_SRC})
    # aux_source_directory(src ADD_SRCS)  # collect all source file in src dir, will set var ADD_SRCS
    # append_srcs_dir(ADD_SRCS "src")     # append source file in src dir to var ADD_SRCS
    # list(REMOVE_ITEM COMPONENT_SRCS "src/test.c")
    # set(ADD_ASM_SRCS "src/asm.S")
    # list(APPEND ADD_SRCS ${ADD_ASM_SRCS})
    # SET_PROPERTY(SOURCE ${ADD_ASM_SRCS} PROPERTY LANGUAGE C) # set .S  ASM file as C language
    # SET_SOURCE_FILES_PROPERTIES(${ADD_ASM_SRCS} PROPERTIES COMPILE_FLAGS "-x assembler-with-cpp -D BBBBB")
    ###############################################


    ###### Add required/dependent components ######
    # list(APPEND ADD_REQUIREMENTS component1)
    ###############################################

    ###### Add link search path for requirements/libs ######
    # list(APPEND ADD_LINK_SEARCH_PATH "${CONFIG_TOOLCHAIN_PATH}/lib")
    # list(APPEND ADD_REQUIREMENTS pthread m)  # add system libs, pthread and math lib for example here
    # set (OpenCV_DIR opencv/lib/cmake/opencv4)
    # find_package(OpenCV REQUIRED)
    ###############################################

    ############ Add static libs ##################
    # list(APPEND ADD_STATIC_LIB "lib/libtest.a")
    ###############################################

    ############ Add dynamic libs ##################
    # list(APPEND ADD_DYNAMIC_LIB "lib/arch/v831/libmaix_nn.so"
    #                             "lib/arch/v831/libmaix_cam.so"
    # )
    ###############################################

    #### Add compile option for this component ####
    #### Just for this component, won't affect other 
    #### modules, including component that depend 
    #### on this component
	# list(APPEND ADD_LINKOPTIONS_PRIVATE -w)
    #### Add compile option for this component
    #### and components denpend on this component
    # list(APPEND ADD_DEFINITIONS_PRIVATE -w)
    # list(APPEND ADD_DEFINITIONS -w)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ARCH_UNIX)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ARCH_WIN32)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ENABLE_LWIP)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ENABLE_SOCKET)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ENABLE_MBEDTLS)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ENABLE_OPENSSL)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ENABLE_IPV6)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ENABLE_MD5)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ENABLE_SSI)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ENABLE_CUSTOM_RANDOM)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ENABLE_CUSTOM_TLS)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ENABLE_CUSTOM_MILLIS)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ENABLE_PACKED_FS)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ENABLE_FATFS)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_ENABLE_LINES)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_IO_SIZE=2048)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_MAX_RECV_SIZE=(3 * 1024 * 1024))
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_MAX_HTTP_HEADERS=40)
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_HTTP_INDEX="index.html")
    # list(APPEND ADD_DEFINITIONS_PRIVATE -DMG_FATFS_ROOT="/")

    ###############################################

    ############ Add static libs ##################
    #### Update parent's variables like CMAKE_C_LINK_FLAGS
    # set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--start-group libmaix/libtest.a -ltest2 -Wl,--end-group" PARENT_SCOPE)
    ###############################################

    # register component, DYNAMIC or SHARED flags will make component compiled to dynamic(shared) lib
    if(CONFIG_TBOX_DYNAMIC)
        register_component(DYNAMIC)
    else()
        register_component()
    endif()
endif()

