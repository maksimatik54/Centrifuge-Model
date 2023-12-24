#define F_CPU 8000000
#define PRESCALER 1024

#include "lcd.h"
#include "motor.h"

volatile static uint16_t encoder_counter = 0;
volatile static uint16_t rpm = 0;
int desired_rpm = 0;

void Setup_PWM() {
	DDRB |= (1 << PB4);
	TCCR0 |= (1 << WGM01) | (1 << WGM00);
	TCCR0 |= (1 << COM01);
	OCR0 = 0;
}

void start_timer(uint16_t seconds) {
	remaining_seconds = seconds;
	uint16_t compareValue = (F_CPU / PRESCALER) * .8;
	Display_Time_Left(remaining_seconds);
	TCCR1B |= (1 << WGM12);
	TCCR1B |= (1 << CS12) | (1 << CS10);
	OCR1A = compareValue;
	TIMSK |= (1 << OCIE1A);
}

void Setup_Interruptions() {
	DDRD &= ~(1 << PD0);
	EIMSK |= (1 << INT0);
	EICRA |= (1 << ISC01) | (1 << ISC00);
	sei();
}

void Setup_RPM_Counter() {
	TCCR3B |= (1 << WGM32);
	OCR3A = 780;
	ETIMSK |= (1 << OCIE3A);
	TCCR3B |= (1 << CS32) | (1 << CS30);
}

ISR(TIMER1_COMPA_vect) {
	if (remaining_seconds > 1) {
		remaining_seconds--;

		Update_Time_Left(remaining_seconds);
		} else {
		TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));
		TIMSK &= ~(1 << OCIE1A);

		Display_Stopping_Motor();
		OCR0 = 0;
		TCCR0 &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
		desired_rpm = 0;
	}
}

ISR(TIMER3_COMPA_vect) {
	rpm = (encoder_counter * 300) / 180;
	encoder_counter = 0;
}

ISR(INT0_vect) { encoder_counter++; }

void setMotorPWM(float value) {
	if (value > 100) value = 100;
	float pwmValue = (value * 2.55);

	if (pwmValue > 255) {
		pwmValue = 255;
		} else if (pwmValue < 0) {
		pwmValue = 0;
	}

	OCR0 = (uint8_t)pwmValue;
}

uint16_t updateMotorSpeed() {
	float Kp = 1.5;
	float Ki = 0.01;
	float Kd = 4.5;

	static float previous_error = 0.0;
	static float integral = 0.0;

	float error = ((float)desired_rpm * 1.087) - rpm;

	float P_out = Kp * error;

	integral += error;

	if (integral > 100) {
		integral = 100;
		} else if (integral < -100) {
		integral = -100;
	}

	float I_out = Ki * integral;

	float derivative = error - previous_error;
	float D_out = Kd * derivative;

	float motorOutput = P_out + I_out + D_out;
	setMotorPWM(motorOutput);

	previous_error = error;
	return rpm;
}