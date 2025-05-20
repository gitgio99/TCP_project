#ifndef LOGGER_H
#define LOGGER_H

void log_info(const char* fmt, ...);
void log_error(const char* fmt, ...);
void init_log();
void close_log();

#endif
