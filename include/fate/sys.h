#ifndef FATE_SYS_H
#define FATE_SYS_H

#include <stdbool.h>
#include <stdint.h>

bool fate_sys_file_exists(const char *path);
uint64_t fate_sys_get_last_write_time(const char *path);
bool fate_sys_set_current_directory(const char *path);
char *fate_sys_getgamepath(void);
void fate_sys_crash_handler_setup(void);
void fate_sys_log_stacktrace(void (*logfunc)(const char *fmt, ...));

#endif /* FATE_SYS_H */
