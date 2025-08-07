#ifndef FILE_UTILS_H
#define FILE_UTILS_H

int file_exists(const char* path);
int copy_firmware_files(const char* src_path, const char* dest_path);

#endif