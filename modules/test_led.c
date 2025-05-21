#include <wiringPi.h>
#include <stdio.h>
#include <unistd.h>  // for sleep()

#define LED_GPIO 4  // BCM 번호 (WiringPi 기준 아님)

int main() {
    printf("device_init() 테스트\n");

    int ret = wiringPiSetupGpio();  // BCM 모드 사용
    printf("wiringPiSetupGpio() 결과: %d\n", ret);
    if (ret == -1) {
        printf("wiringPi 초기화 실패\n");
        return 127;  // 서버에서 사용하는 실패 코드
    }

    pinMode(LED_GPIO, OUTPUT);
    int n=5;
    while(n--){

    // LED ON
    digitalWrite(LED_GPIO, HIGH);
    printf("LED ON\n");
    sleep(2);  // 2초 유지

    // LED OFF
    digitalWrite(LED_GPIO, LOW);
    printf("LED OFF\n");
    sleep(2); 
    }

    return 0;
}
