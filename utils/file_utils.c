#include "file_utils.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>

int file_exists(const char* path) {
    DWORD attrib = GetFileAttributesA(path);
    return attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY);
}

int copy_firmware_files(const char* src_path, const char* dest_path) {
    char src_pattern[256];
    snprintf(src_pattern, sizeof(src_pattern), "%s\\*.*", src_path);
    WIN32_FIND_DATA ffd;
    HANDLE hFind = FindFirstFile(src_pattern, &ffd);
    if (hFind == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Failed to open source directory: %s\n", src_path);
        return -1;
    }

    CreateDirectory(dest_path, NULL);

    do {
        if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            const char* ext = strrchr(ffd.cFileName, '.');
            if (ext && (_stricmp(ext, ".img") == 0 || _stricmp(ext, ".bin") == 0)) {
                char src_file[256];
                char dest_file[256];
                snprintf(src_file, sizeof(src_file), "%s\\%s", src_path, ffd.cFileName);
                snprintf(dest_file, sizeof(dest_file), "%s\\%s", dest_path, ffd.cFileName);

                if (!CopyFile(src_file, dest_file, FALSE)) {
                    fprintf(stderr, "Failed to copy file: %s\n", ffd.cFileName);
                }
            }
        }
    } while (FindNextFile(hFind, &ffd) != 0);
    FindClose(hFind);
    return 0;
}