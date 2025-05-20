#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>

static FILE* log_fp = NULL;

void init_log() {
    log_fp = fopen("server.log", "a");
    if (!log_fp) {
        fprintf(stderr, "[로그 초기화 실패] server.log 열기 실패\n");
        exit(1);
    }
}

void close_log() {
    if (log_fp) fclose(log_fp);
}

static void write_log(const char* level, const char* fmt, va_list args) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);

    fprintf(log_fp, "[%02d-%02d %02d:%02d:%02d] [%s] ",
        t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, level);
    vfprintf(log_fp, fmt, args);
    fprintf(log_fp, "\n");
    fflush(log_fp);
}

void log_info(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    write_log("INFO", fmt, args);
    va_end(args);
}

void log_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    write_log("ERROR", fmt, args);
    va_end(args);
}
