#include "emulator.h"
#include <qemu/osdep.h>
#include <qemu/main.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

#define MAX_FIRMWARE_FILES 50
#define MAX_PATH 256

static int collect_firmware_files(const char* firmware_path, char** firmware_files, int* file_count) {
    char src_pattern[MAX_PATH];
    snprintf(src_pattern, sizeof(src_pattern), "%s\\*.*", firmware_path);
    WIN32_FIND_DATA ffd;
    HANDLE hFind = FindFirstFile(src_pattern, &ffd);
    if (hFind == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Failed to open firmware directory: %s\n", firmware_path);
        return 0;
    }

    *file_count = 0;
    do {
        if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            const char* ext = strrchr(ffd.cFileName, '.');
            if (ext && (_stricmp(ext, ".img") == 0 || _stricmp(ext, ".bin") == 0)) {
                if (*file_count < MAX_FIRMWARE_FILES) {
                    firmware_files[*file_count] = malloc(MAX_PATH);
                    if (!firmware_files[*file_count]) {
                        fprintf(stderr, "Memory allocation failed for firmware file path\n");
                        FindClose(hFind);
                        return 0;
                    }
                    snprintf(firmware_files[*file_count], MAX_PATH, "%s\\%s", firmware_path, ffd.cFileName);
                    (*file_count)++;
                }
            }
        }
    } while (FindNextFile(hFind, &ffd) != 0);
    FindClose(hFind);

    if (*file_count == 0) {
        fprintf(stderr, "No .img or .bin files found in %s\n", firmware_path);
        return 0;
    }

    return 1;
}

int start_emulator(const char* firmware_path, int is_new_device) {
    char* firmware_files[MAX_FIRMWARE_FILES] = {0};
    int file_count = 0;

    // Collect all .img and .bin files from the firmware directory
    if (!collect_firmware_files(firmware_path, firmware_files, &file_count)) {
        return -1;
    }

    // Allocate memory for QEMU arguments
    char** qemu_args = malloc((9 + 2 * file_count + 5) * sizeof(char*));
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
    qemu_args[arg_index++] = "cortex-a53"; // Default Cortex-A53, supports all Cortex-A
    qemu_args[arg_index++] = "-vga";
    qemu_args[arg_index++] = "virtio";
    qemu_args[arg_index++] = "-soundhw";
    qemu_args[arg_index++] = "hda";

    // Add all firmware files as -drive
    for (int i = 0; i < file_count; i++) {
        qemu_args[arg_index++] = "-drive";
        qemu_args[arg_index++] = firmware_files[i];
    }

    // Add parameters for old or new devices
    qemu_args[arg_index++] = "-enable-kvm";
    qemu_args[arg_index++] = "-append";
    qemu_args[arg_index++] = is_new_device ? "root=/dev/ram0 androidboot.hardware=generic_arm64 androidboot.dynamic_partitions" : "root=/dev/ram0 androidboot.hardware=generic_arm64";
    qemu_args[arg_index++] = NULL;

    // Start QEMU
    if (qemu_main(arg_index - 1, qemu_args, NULL) < 0) {
        fprintf(stderr, "QEMU initialization failed\n");
        for (int i = 0; i < file_count; i++) free(firmware_files[i]);
        free(qemu_args);
        return -1;
    }

    // Cleanup
    for (int i = 0; i < file_count; i++) free(firmware_files[i]);
    free(qemu_args);
    return 0;
}

void cleanup_emulator(void) {
    // Cleanup QEMU resources
}