cmake_minimum_required(VERSION 3.10) project(KSNAndroid C)
set(CMAKE_C_STANDARD 11)
Find SDL2
find_package(SDL2 REQUIRED)
Find QEMU
find_library(QEMU_LIBRARY NAMES qemu-system-aarch64)
Find FFmpeg
find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h) find_library(AVCODEC_LIBRARY avcodec) find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h) find_library(AVFORMAT_LIBRARY avformat)
Find tinyfiledialogs
find_library(TINYFD_LIBRARY tinyfiledialogs) find_path(TINYFD_INCLUDE_DIR tinyfiledialogs.h)
Include directories
include_directories(${SDL2_INCLUDE_DIRS} ${AVCODEC_INCLUDE_DIR} ${AVFORMAT_INCLUDE_DIR} ${TINYFD_INCLUDE_DIR} include)
Source files
set(SOURCES src/main.c src/emulator/emulator.c src/ui/ui.c src/audio/audio.c src/video/video.c src/utils/file_utils.c )
Executable
add_executable(ksnandroid ${SOURCES})
raries
target_link_libraries(ksnandroid PRIVATE ${SDL2_LIBRARIES} ${QEMU_LIBRARY} ${AVCODEC_LIBRARY} ${AVFORMAT_LIBRARY} ${TINYFD_LIBRARY})