#include "emulator.h"
#include <qemu/osdep.h>
#include <qemu/main.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#define MAX_FIRMWARE_FILES 50
#define MAX_PATH 256

static int collect_firmware_files(const char* firmware_path, char** firmware_files, int* file_count) {
    DIR* dir = opendir(firmware_path);
    if (!dir) {
        fprintf(stderr, "Failed to open firmware directory: %s\n", firmware_path);
        return 0;
    }

    *file_count = 0;
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL && *file_count < MAX_FIRMWARE_FILES) {
        if (entry->d_type == DT_REG) {
            const char* ext = strrchr(entry->d_name, '.');
            if (ext && (strcasecmp(ext, ".img") == 0 || strcasecmp(ext, ".bin") == 0)) {
                firmware_files[*file_count] = malloc(MAX_PATH);
                if (!firmware_files[*file_count]) {
                    fprintf(stderr, "Memory allocation failed for firmware file path\n");
                    closedir(dir);
                    return 0;
                }
                snprintf(firmware_files[*file_count], MAX_PATH, "%s/%s", firmware_path, entry->d_name);
                (*file_count)++;
            }
        }
    }

    closedir(dir);

    if (*file_count == 0) {
        fprintf(stderr, "No .img or .bin files found in %s\n", firmware_path);
        return 0;
    }

    return 1;
}

int start_emulator(const char* firmware_path, int is_new_device) {
    char* firmware_files[MAX_FIRMWARE_FILES] = {0};
    int file_count = 0;

    if (!collect_firmware_files(firmware_path, firmware_files, &file_count)) {
        return -1;
    }

    char** qemu_args = malloc((9 + 2 * file_count + 5) * sizeof(char*)); // Базовые аргументы + файлы + параметры
    if (!qemu_args) {
        fprintf(stderr, "Memory allocation failed for QEMU arguments\n");
        for (int i = 0; i < file_count; i++) free(firmware_files[i]);
        return -1;
    }

    int arg_index = 0;
    qemu_args[arg_index++] = "qemu-system-aarch64";
    qemu_args[arg_index++] = "-m";
    qemu_args[arg_index++] = "2048";
    qemu_args[arg_index++] = "-machine";
    qemu_args[arg_index++] = "virt";
    qemu_args[arg_index++] = "-cpu";
    qemu_args[arg_index++] = "cortex-a53";
    qemu_args[arg_index++] = "-vga";
    qemu_args[arg_index++] = "virtio";
    qemu_args[arg_index++] = "-soundhw";
    qemu_args[arg_index++] = "hda";

    for (int i = 0; i < file_count; i++) {
        qemu_args[arg_index++] = "-drive";
        qemu_args[arg_index++] = firmware_files[i];
    }

    qemu_args[arg_index++] = "-enable-kvm";
    qemu_args[arg_index++] = "-append";
    qemu_args[arg_index++] = is_new_device ? "root=/dev/ram0 androidboot.hardware=generic_arm64 androidboot.dynamic_partitions" : "root=/dev/ram0 androidboot.hardware=generic_arm64";
    qemu_args[arg_index++] = NULL;

    // Запуск QEMU
    if (qemu_main(arg_index - 1, qemu_args, NULL) < 0) {
        fprintf(stderr, "QEMU initialization failed\n");
        for (int i = 0; i < file_count; i++) free(firmware_files[i]);
        free(qemu_args);
        return -1;
    }

    // Очистка
    for (int i = 0; i < file_count; i++) free(firmware_files[i]);
    free(qemu_args);
    return 0;
}

void cleanup_emulator(void) {
    // Cleanup QEMU resources
}