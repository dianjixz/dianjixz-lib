#include <stdio.h>
#include <stdlib.h>
#include <libfdt.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <device_tree_blob.dtb>\n", argv[0]);
        return 1;
    }

    // Open the device tree blob file
    FILE *f = fopen(argv[1], "rb");
    if (!f) {
        perror("Failed to open device tree file");
        return 1;
    }

    // Determine the file size
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    // Allocate memory to hold the device tree blob
    void *dtb = malloc(fsize);
    if (!dtb) {
        perror("Failed to allocate memory");
        fclose(f);
        return 1;
    }

    // Read the file into memory
    fread(dtb, 1, fsize, f);
    fclose(f);

    // Check the device tree header
    if (fdt_check_header(dtb) != 0) {
        fprintf(stderr, "Invalid device tree header\n");
        free(dtb);
        return 1;
    }

    // Traverse the device tree nodes
    int nodeoffset = 0;
    while ((nodeoffset = fdt_next_node(dtb, nodeoffset, NULL)) >= 0) {
        const char *name = fdt_get_name(dtb, nodeoffset, NULL);
        printf("Node name: %s\n", name);

        int len;
        const char *compatible = fdt_getprop(dtb, nodeoffset, "compatible", &len);
        if (compatible) {
            printf("  Compatible: %s\n", compatible);
        } else {
            printf("  Compatible: not found\n");
        }
    }

    // Free the allocated memory
    free(dtb);

    return 0;
}
