#include "input.h"

volatile static uint8_t changed_buttons;
volatile static uint8_t current_buttons_state;

void Buttons_Init() {
	BUTTONS_DDR &= ~BUTTONS_MASK;
	BUTTONS_PORT |= BUTTONS_MASK;
}

int Buttons_Handler() {
	static uint8_t last_buttons_state = 0xFF;
	static uint8_t buttons_pressed_state = 0xFF;

	current_buttons_state = BUTTONS_STATE;
	changed_buttons = current_buttons_state ^ last_buttons_state;
	uint8_t pressed_buttons = changed_buttons & ~current_buttons_state;

	last_buttons_state = current_buttons_state;

	if (pressed_buttons != buttons_pressed_state) {
		buttons_pressed_state = pressed_buttons;

		if (pressed_buttons & 0b00001) return BUTTON_INCREASE;
		if (pressed_buttons & 0b00010) return BUTTON_DECREASE;
		if (pressed_buttons & 0b00100) return BUTTON_NEXT_MODE;
		if (pressed_buttons & 0b01000) return BUTTON_PREVIOUS_MODE;
		if (pressed_buttons & 0b10000) return BUTTON_EMERGENCY_STOP;
	}

	return BUTTON_NONE;
}