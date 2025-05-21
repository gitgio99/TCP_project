#include <wiringPi.h>
#include <softPwm.h>
#include <string.h>
#include <stdio.h>

#include "dev_interface.h"
#include "logger.h"


#define LED_GPIO 4

void led_device_init() {
    wiringPiSetupGpio();
    pinMode(LED_GPIO, OUTPUT);
    softPwmCreate(LED_GPIO, 0, 255);
}

void led_control(const char* cmd) {
    log_info("[LED] device_control 진입: \"%s\"", cmd);
    char action[16], option[16];
    int matched = sscanf(cmd, "LED %s %s", action, option);

    if (strcasecmp(action, "OFF") == 0) {
        softPwmWrite(LED_GPIO, 0);
        log_info("[LED] OFF");
    }
    else if (strcasecmp(action, "ON") == 0) {
        if (matched == 2) {
            // 세부 옵션 있음: MAX, MID, MIN
            if (strcasecmp(option, "MAX") == 0)
                softPwmWrite(LED_GPIO, 255);
            else if (strcasecmp(option, "MID") == 0)
                softPwmWrite(LED_GPIO, 127);
            else if (strcasecmp(option, "MIN") == 0)
                softPwmWrite(LED_GPIO, 64);
            else
                softPwmWrite(LED_GPIO, 200);  // 기본값
        } else {
            // LED ON만 있을 경우 → 기본값
            softPwmWrite(LED_GPIO, 255);
        }
        log_info("[LED] ON (%s)", matched == 2 ? option : "DEFAULT");
    } else {
        log_error("[LED] 잘못된 명령: %s", cmd);
    }
}


int main() {
    wiringPiSetupGpio();  // BCM 모드
    softPwmCreate(LED_GPIO, 0, 255);  // PWM 초기화

    // 밝기 조절 테스트
    softPwmWrite(LED_GPIO, 255);  // 최대 밝기
    delay(1000);
    softPwmWrite(LED_GPIO, 127);  // 중간 밝기
    delay(1000);
    softPwmWrite(LED_GPIO, 64);   // 최소 밝기
    delay(1000);
    softPwmWrite(LED_GPIO, 0);    // OFF
    delay(1000);

    return 0;
}
