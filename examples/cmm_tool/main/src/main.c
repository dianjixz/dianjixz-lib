#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#define PAGE_SIZE 4096

void usage(const char *progname) {
    printf("Usage: %s <physical_address> <size> <output_file>\n", progname);
    printf("  physical_address: 物理内存地址 (十六进制，如 0x1000)\n");
    printf("  size: 要读取的字节数\n");
    printf("  output_file: 输出文件名\n");
    printf("\n注意: 需要root权限运行\n");
}

int read_physical_memory(uint64_t phys_addr, size_t size, const char *output_file) {
    int mem_fd = -1;
    int out_fd = -1;
    void *mapped_base = NULL;
    void *mapped_dev_base = NULL;
    int ret = -1;
    
    // 计算页对齐的地址和偏移
    uint64_t page_base = phys_addr & ~(PAGE_SIZE - 1);
    uint64_t page_offset = phys_addr - page_base;
    size_t map_size = size + page_offset;
    
    // 确保映射大小是页大小的倍数
    if (map_size % PAGE_SIZE != 0) {
        map_size = ((map_size / PAGE_SIZE) + 1) * PAGE_SIZE;
    }
    
    printf("物理地址: 0x%lx\n", phys_addr);
    printf("页对齐基址: 0x%lx\n", page_base);
    printf("页内偏移: 0x%lx\n", page_offset);
    printf("读取大小: %zu 字节\n", size);
    printf("映射大小: %zu 字节\n", map_size);
    
    // 打开 /dev/mem 设备文件
    mem_fd = open("/dev/mem", O_RDONLY);
    if (mem_fd == -1) {
        fprintf(stderr, "错误: 无法打开 /dev/mem: %s\n", strerror(errno));
        fprintf(stderr, "提示: 需要root权限，或者检查 /dev/mem 是否存在\n");
        goto cleanup;
    }
    
    // 映射物理内存到虚拟地址空间
    mapped_base = mmap(NULL, map_size, PROT_READ, MAP_SHARED, mem_fd, page_base);
    if (mapped_base == MAP_FAILED) {
        fprintf(stderr, "错误: mmap 失败: %s\n", strerror(errno));
        fprintf(stderr, "可能原因: 物理地址无效或权限不足\n");
        goto cleanup;
    }
    
    // 计算实际数据的起始地址
    mapped_dev_base = (char *)mapped_base + page_offset;
    
    // 打开输出文件
    out_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd == -1) {
        fprintf(stderr, "错误: 无法创建输出文件 %s: %s\n", output_file, strerror(errno));
        goto cleanup;
    }
    
    // 读取内存数据并写入文件
    ssize_t bytes_written = write(out_fd, mapped_dev_base, size);
    if (bytes_written != (ssize_t)size) {
        fprintf(stderr, "错误: 写入文件失败: %s\n", strerror(errno));
        goto cleanup;
    }
    
    printf("成功读取 %zu 字节数据并写入到 %s\n", size, output_file);
    
    // 可选：显示前64字节的十六进制数据
    printf("\n前64字节数据预览 (十六进制):\n");
    unsigned char *data = (unsigned char *)mapped_dev_base;
    size_t preview_size = (size < 64) ? size : 64;
    
    for (size_t i = 0; i < preview_size; i++) {
        if (i % 16 == 0) {
            printf("%08lx: ", phys_addr + i);
        }
        printf("%02x ", data[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    if (preview_size % 16 != 0) {
        printf("\n");
    }
    
    ret = 0;  // 成功

cleanup:
    if (mapped_base != NULL && mapped_base != MAP_FAILED) {
        munmap(mapped_base, map_size);
    }
    if (mem_fd != -1) {
        close(mem_fd);
    }
    if (out_fd != -1) {
        close(out_fd);
    }
    
    return ret;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        usage(argv[0]);
        return 1;
    }
    
    // 检查是否为root用户
    if (geteuid() != 0) {
        fprintf(stderr, "警告: 建议以root权限运行此程序\n");
    }
    
    // 解析物理地址
    char *endptr;
    uint64_t phys_addr = strtoull(argv[1], &endptr, 0);
    if (*endptr != '\0') {
        fprintf(stderr, "错误: 无效的物理地址格式: %s\n", argv[1]);
        return 1;
    }
    
    // 解析大小
    size_t size = strtoull(argv[2], &endptr, 0);
    if (*endptr != '\0' || size == 0) {
        fprintf(stderr, "错误: 无效的大小: %s\n", argv[2]);
        return 1;
    }
    
    const char *output_file = argv[3];
    
    // 执行内存读取
    if (read_physical_memory(phys_addr, size, output_file) != 0) {
        return 1;
    }
    
    return 0;
}