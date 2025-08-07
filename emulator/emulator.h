#ifndef EMULATOR_H
#define EMULATOR_H

int start_emulator(const char* firmware_path, int is_new_device);
void cleanup_emulator(void);

#endif