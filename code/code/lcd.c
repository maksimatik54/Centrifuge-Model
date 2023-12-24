#define F_CPU 8000000

#include <avr/io.h>
#include <math.h>
#include <util/delay.h>

#include "lcd.h"

void LCD_Init() {
	LCD_PORT_DDR = 0xFF;
	LCD_COMMAND_DDR = 0x07;
	_delay_ms(15);
	lcd_command(0x38);  // DL = 8D, N = 2R, F = 5x7 style
	lcd_command(0x0C);  // display on, cursor off, blinking off
	lcd_command(0x01);  // clear screen
	_delay_ms(2);
}

void lcd_send(unsigned char data) {
	LCD_COMMAND |= (1 << 1);
	LCD_PORT = data;
	_delay_us(50);
	LCD_COMMAND &= ~(1 << 1);
	_delay_us(50);
}

void lcd_command(unsigned char cmd) {
	LCD_COMMAND &= ~(1 << 0);
	lcd_send(cmd);
}

void lcd_data(unsigned char data) {
	LCD_COMMAND |= (1 << 0);
	lcd_send(data);
}

void LCD_Put_String(char *string) {
	int i;
	for (i = 0; string[i] != '\0'; i++) {
		lcd_data(string[i]);
	}
}

void LCD_Put_Int(int target) {
	int length = 0;
	if (target == 0)
	length = 1;
	else if (target > 0)
	length = log10(target) + 1;
	else {
		lcd_data('-');
		target = -target;
		length = log10(target) + 1;
	}
	while (length > 0) {
		lcd_data('0' + target / ((int)pow(10, length - 1)) % 10);
		length--;
	}
}

void LCD_Put_Seconds(int seconds) {
	lcd_data('0' + seconds / 10);
	lcd_data('0' + seconds % 10);
}

void LCD_Clear() {
	lcd_command(1);
	_delay_ms(30);
	lcd_command(0b10000000);
}

void Display_Info(int rpm, int seconds, int active) {
	LCD_Clear();
	if (!active)
	LCD_Put_String("> ");
	else
	LCD_Put_String("  ");
	LCD_Put_Int(rpm);
	LCD_Put_String(" rpm");
	lcd_command(0b11000000);
	if (active)
	LCD_Put_String("> ");
	else
	LCD_Put_String("  ");
	LCD_Put_Int(seconds);
	LCD_Put_String(" minutes");
}

void Display_Time_Left(int seconds) {
	LCD_Clear();
	LCD_Put_String("Time left: ");
	LCD_Put_Int(seconds / 60);
	LCD_Put_String(":");
	LCD_Put_Seconds(seconds % 60);
}

void Update_Time_Left(int seconds) {
	lcd_command(0b10001101);
	LCD_Put_Seconds(seconds % 60);
}

void Display_Stopping_Motor() {
	LCD_Clear();
	LCD_Put_String("Time is up!");
	lcd_command(0b11000000);
	LCD_Put_String("Motor halting...");
}

void Display_Revving_Motor() {
	LCD_Clear();
	LCD_Put_String("Revving motor...");
}