// buzzer_mod.c


#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <softTone.h>

#include "../include/dev_interface.h"

#define BUZZER 27      // GPIO 25 (BCM 25 = 핀 22)
#define TOTAL 32

// 초기화
void buzzer_device_init() {
    wiringPiSetupGpio();
    pinMode(BUZZER, OUTPUT);
    softToneCreate(BUZZER);
}

// 학교종 멜로디 음계
int notes[TOTAL] = {
    391, 391, 440, 440, 391, 391, 330, 330,
    391, 391, 330, 330, 294, 294, 294, 0,
    391, 391, 440, 440, 391, 391, 330, 330,
    391, 330, 294, 330, 262, 262, 262, 0
};

// 음악 재생 함수
void musicPlay() {
    for (int i = 0; i < TOTAL; ++i) {
        softToneWrite(BUZZER, notes[i]);
        delay(280);
    }
    softToneWrite(BUZZER, 0);  // 종료 시 무음
}

// 제어 명령 처리
void buzzer_device_control(const char* cmd) {
    if (strcmp(cmd, "ON") == 0) {
        musicPlay();
    } else if (strcmp(cmd, "OFF") == 0) {
        softToneWrite(BUZZER, 0);
    } else {
        printf("[BUZZER] 알 수 없는 명령: %s\n", cmd);
    }
}