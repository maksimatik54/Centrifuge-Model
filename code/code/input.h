#ifndef INPUT_H_
#define INPUT_H_
#define BUTTONS_PORT PORTC
#define BUTTONS_DDR DDRC
#define BUTTONS_STATE PINC
#define BUTTONS_MASK 0b00011111

#include <avr/io.h>

enum ButtonPress {
	BUTTON_NONE = -1,
	BUTTON_INCREASE = 0,
	BUTTON_DECREASE = 1,
	BUTTON_NEXT_MODE = 2,
	BUTTON_PREVIOUS_MODE = 3,
	BUTTON_EMERGENCY_STOP = 4
};

void Buttons_Init();
int Buttons_Handler();

#endif /* INPUT_H_ */