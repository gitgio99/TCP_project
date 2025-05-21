// seg_buzzer_test.c

#include <wiringPi.h>
#include <softTone.h>
#include <stdio.h>

int seg_pins[4] = {23, 18, 15, 14};  // A, B, C, D
int numbers[10][4] = {
    {0,0,0,0}, 	/* 0 */
    {0,0,0,1}, 	/* 1 */
	{0,0,1,0}, 	/* 2 */
    {0,0,1,1}, 	/* 3 */
    {0,1,0,0}, 	/* 4 */
    {0,1,0,1}, 	/* 5 */
    {0,1,1,0}, 	/* 6 */
    {0,1,1,1}, 	/* 7 */
    {1,
        
        0,0,0}, 	/* 8 */
    {1,0,0,1}
};

#define BUZZER 27
#define NOTE_C 261
#define NOTE_E 329
#define NOTE_G 392

void display_number(int n) {
    for (int i = 0; i < 4; i++)
        digitalWrite(seg_pins[i], numbers[n][i]);
}

void play_buzzer_alert() {
    softToneCreate(BUZZER);
    softToneWrite(BUZZER, NOTE_C); delay(200);
    softToneWrite(BUZZER, NOTE_E); delay(200);
    softToneWrite(BUZZER, NOTE_G); delay(200);
    softToneWrite(BUZZER, 0);  // OFF
}

int main() {
    wiringPiSetupGpio();
    for (int i = 0; i < 4; i++) pinMode(seg_pins[i], OUTPUT);
    pinMode(BUZZER, OUTPUT);

    int input_num;
    printf("카운트다운 시작 숫자 입력 (0 ~ 9): ");
    scanf("%d", &input_num);

    if (input_num < 0 || input_num > 9) {
        printf("잘못된 숫자입니다. 0~9 범위만 가능합니다.\n");
        return 1;
    }

    for (int n = input_num; n >= 0; n--) {
        display_number(n);
        delay(1000);
    }

    play_buzzer_alert();

    return 0;
}
