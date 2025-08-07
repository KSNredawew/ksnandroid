#include "file_utils.h"
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int file_exists(const char* path) {
    struct stat buffer;
    return stat(path, &buffer) == 0;
}

int copy_firmware_files(const char* src_path, const char* dest_path) {
    DIR* dir;
    struct dirent* entry;
    char src_file[256];
    char dest_file[256];

    // Создаем папку назначения, если не существует
    mkdir(dest_path, 0755);

    dir = opendir(src_path);
    if (!dir) {
        fprintf(stderr, "Failed to open source directory: %s\n", src_path);
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            const char* ext = strrchr(entry->d_name, '.');
            if (ext && (strcasecmp(ext, ".img") == 0 || strcasecmp(ext, ".bin") == 0)) {
                snprintf(src_file, sizeof(src_file), "%s/%s", src_path, entry->d_name);
                snprintf(dest_file, sizeof(dest_file), "%s/%s", dest_path, entry->d_name);

                FILE* src = fopen(src_file, "rb");
                FILE* dest = fopen(dest_file, "wb");
                if (!src || !dest) {
                    fprintf(stderr, "Failed to copy file: %s\n", entry->d_name);
                    if (src) fclose(src);
                    if (dest) fclose(dest);
                    continue;
                }

                char buffer[4096];
                size_t bytes;
                while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
                    fwrite(buffer, 1, bytes, dest);
                }

                fclose(src);
                fclose(dest);
            }
        }
    }

    closedir(dir);
    return 0;
}