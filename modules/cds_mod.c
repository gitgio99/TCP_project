// buzzer_mod.c
#include "../include/dev_interface.h"
// void device_init() {}
// void device_control(const char* cmd) {}

// /*


// #include <wiringPi.h>
// #include <stdio.h>

// #define CDS 	24 	/*  */
// #define LED 	17 	/*  */

// int cdsControl( )
// {
//     int i;

//     // pinMode(SW, INPUT); 	/* Pin 모드를 입력으로 설정 */
//     pinMode(CDS, INPUT); 	/* Pin 모드를 입력으로 설정 */
//     pinMode(LED, OUTPUT); 	/* Pin 모드를 출력으로 설정 */
    
//     for (;;) { 			/* 조도 센서 검사를 위해 무한 루프를 실행한다. */
//         int val = digitalRead(CDS);
        

//         if(digitalRead(CDS) == HIGH) {
//             printf("CDS = %d\n", val); 	/* 빛이 감지되면(HIGH) */
//             digitalWrite(LED, LOW); 	/* LED 켜기(On) */
//             delay(1000);
//             digitalWrite(LED, HIGH); 	/* LED 끄기(Off) */
//             printf("CDS = %d\n", val);
//         }
//     }

//     return 0;
// }

// int main( )
// {
//     wiringPiSetupGpio( );
//     cdsControl( ); 		/* 조도 센서 사용을 위한 함수 호출 */
//     return 0;
// }

// */
#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>

#define CDS_PIN 17     // CDS 센서: BCM 17 (물리핀 11)
#define LED_PIN 4      // LED 제어용: BCM 4 (물리핀 7)

void device_init() {
    wiringPiSetupGpio();

    pinMode(CDS_PIN, INPUT);          // CDS: 입력 모드
    softPwmCreate(LED_PIN, 0, 255);   // LED: PWM 초기화
    softPwmWrite(LED_PIN, 0);         // 기본 OFF
}

void device_control_loop() {
    while (1) {
        int cds_val = digitalRead(CDS_PIN);  // LOW: 어두움, HIGH: 밝음
        if (cds_val == LOW) {
            softPwmWrite(LED_PIN, 255);
            printf("어두움 → LED ON\n");
        } else {
            softPwmWrite(LED_PIN, 0);
            printf("밝음 → LED OFF\n");
        }
        delay(500);
    }
}

#ifdef TEST_MAIN
int main() {
    device_init();
    device_control_loop();
    return 0;
}
#endif
