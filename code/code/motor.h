#ifndef MOTOR_H_
#define MOTOR_H_

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

volatile static uint16_t remaining_seconds = 0;
extern int desired_rpm;

enum mode {
	SETUP_RPM = 0,
	SETUP_TIMER = 1,
	REVVING = 2,
	START_TIMER = 3,
	WORKING = 4
};

void Setup_PWM();
void start_timer(uint16_t seconds);
void Setup_RPM_Counter();
void Setup_Interruptions();
void setMotorPWM(float value);
uint16_t updateMotorSpeed();

#endif /* MOTOR_H_ */