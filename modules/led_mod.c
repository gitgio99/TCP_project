#include <wiringPi.h>
#include <string.h>
#include <stdio.h>
#include "dev_interface.h"
// #include "logger.h"

void device_init() {
    wiringPiSetupGpio();  // BCM 번호 체계
    pinMode(4, OUTPUT);  // GPIO 7
}

void device_control(const char* cmd) {
    log_info("[LED] device_control() 호출됨, cmd = %s", cmd);

    if (strcmp(cmd, "LED ON") == 0) {
        // log_info("[LED] LED ON 분기 진입");
        puts("LED ON 동작함");
        // digitalWrite(4, HIGH);
    } else if (strcmp(cmd, "LED OFF") == 0) {
        // log_info("[LED] LED Off 분기 진입");
        puts("LED OFF 동작함");
        // digitalWrite(4, LOW);
    } else {
        // log_info("[LED] 명령 미일치");
        printf("[LED] 알 수 없는 명령어: %s\n", cmd);
    }
}
