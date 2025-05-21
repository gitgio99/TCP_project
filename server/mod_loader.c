#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "dev_interface.h"
#include "logger.h"


void* load_module(const char* path) {
    dlerror();  // 에러 클리어

    void* handle = dlopen(path, RTLD_NOW);  // RTLD_LAZY - dlsym()호출 자체는 통과되어도 실제 실행시 segfault가능성있음
    printf("[디버그] handle 포인터: %p\n", handle); // demon안했을시 보이는 디버그
    if (!handle) {
        log_error("dlopen 실패: %s", dlerror());
        return NULL;
    }
    log_info("모듈 로딩 성공: %s", path);

    dlerror();  // dlsym 전에 에러 클리어
    log_info("dlsym 호출 직전");
    void (*init_func)() = dlsym(handle, "device_init");
    log_info("dlsym 호출 직후");
    char* err = dlerror();
    if (err) {
        log_error("device_init 없음: %s", err);
    } else {
        init_func();
        log_info("device_init() 호출 완료");
    }

    return handle;
}


// void run_device_command(void* handle, const char* cmd) {
//     log_info("run_device_command() 진입. 명령어: %s", cmd);  // ✅
//     dlerror();  // 에러 초기화
//     log_info("dlsym 호출 직전");
//     void (*control_func)(const char*) = dlsym(handle, "device_control");
//     log_info("dlsym 호출 직후");
//     char* error = dlerror();

//     if (!control_func || error) {
//         log_error("device_control 심볼 로딩 실패: %s", error ? error : "알 수 없음");
//         return;
//     }

//     log_info("device_control() 호출 준비 완료: 명령어 = \"%s\"", cmd);
//     control_func(cmd);
//     log_info("device_control() 실행 완료");
// }

void run_device_command(void* handle, const char* device, const char* cmd) {
    log_info("run_device_command() 진입. device: %s, 명령어: %s", device, cmd);

    // 심볼 이름 만들기: 예 "led_control"
    char symbol_name[64];
    snprintf(symbol_name, sizeof(symbol_name), "%s_control", device);

    dlerror();  // 에러 초기화
    void (*control_func)(const char*) = (void (*)(const char*)) dlsym(handle, symbol_name);
    char* error = dlerror();

    if (!control_func || error) {
        log_error("%s 심볼 로딩 실패: %s", symbol_name, error ? error : "알 수 없음");
        return;
    }

    log_info("%s() 호출 준비 완료: \"%s\"", symbol_name, cmd);
    control_func(cmd);
    log_info("%s() 실행 완료", symbol_name);
}
