#define F_CPU 8000000

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "input.h"
#include "lcd.h"
#include "motor.h"

void Interface_Init() {
	Buttons_Init();
	LCD_Init();
}

void Motor_Init() {
	Setup_PWM();
	Setup_RPM_Counter();
	Setup_Interruptions();
}

int main(void) {
	Interface_Init();
	Motor_Init();

	int rpms[] = {0, 500, 1000, 1500, 2000, 2500, 3000};
	int seconds[] = {10, 20, 30, 40, 50, 60};
	int currentRPM = 0;

	uint8_t rpm_mode = 1, seconds_mode = 0;
	int8_t mode = 0;
	int result = 0;

	uint8_t rpm_max = sizeof(rpms) / sizeof(rpms[0]) - 1;
	uint8_t seconds_max = sizeof(seconds) / sizeof(seconds[0]) - 1;

	Display_Info(rpms[rpm_mode], seconds[seconds_mode], mode);

	while (1) {
		result = Buttons_Handler();
		if (result == BUTTON_EMERGENCY_STOP && mode >= REVVING) {
			mode = WORKING;
			rpm_mode = 0;
			remaining_seconds = 0;
			TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));
			TIMSK &= ~(1 << OCIE1A);
			desired_rpm = 0;
			Display_Stopping_Motor();
		}
		switch (mode) {
			case SETUP_RPM:
			if (result == BUTTON_INCREASE && rpm_mode < rpm_max) {
				rpm_mode++;
				Display_Info(rpms[rpm_mode], seconds[seconds_mode], mode);
			} else if (result == BUTTON_DECREASE && rpm_mode > 1) {
				rpm_mode--;
				Display_Info(rpms[rpm_mode], seconds[seconds_mode], mode);
			} else if (result == BUTTON_NEXT_MODE) {
				mode = SETUP_TIMER;
				Display_Info(rpms[rpm_mode], seconds[seconds_mode], mode);
			}
			break;

			case SETUP_TIMER:
			if (result == BUTTON_INCREASE && seconds_mode < seconds_max) {
				seconds_mode++;
				Display_Info(rpms[rpm_mode], seconds[seconds_mode], mode);
			} else if (result == BUTTON_DECREASE && seconds_mode > 0) {
				seconds_mode--;
				Display_Info(rpms[rpm_mode], seconds[seconds_mode], mode);
			} else if (result == BUTTON_NEXT_MODE) {
				mode = REVVING;
				desired_rpm = rpms[rpm_mode] / 10;
				TCCR0 |= (1 << CS01) | (1 << CS00);
				Display_Revving_Motor();
			} else if (result == BUTTON_PREVIOUS_MODE) {
				mode = SETUP_RPM;
				Display_Info(rpms[rpm_mode], seconds[seconds_mode], mode);
			}
			break;

			case REVVING:
			currentRPM = updateMotorSpeed();
			if (currentRPM >= rpms[rpm_mode] / 10 - 2 &&
				currentRPM <= rpms[rpm_mode] / 10 + 2)
				mode = START_TIMER;
			break;

			case START_TIMER:
			start_timer(seconds[seconds_mode]);
			mode = WORKING;
			break;

			case WORKING:
			if (updateMotorSpeed() < 25) {
				mode = SETUP_RPM;
				rpm_mode = 1;
				TCCR0 &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
				OCR0 = 0;
				Display_Info(rpms[rpm_mode], seconds[seconds_mode], mode);
			}
			break;
		}
	}
}