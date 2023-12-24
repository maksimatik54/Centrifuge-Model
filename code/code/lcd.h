#ifndef LCD_H_
#define LCD_H_

#define LCD_PORT PORTA
#define LCD_PORT_DDR DDRA
#define LCD_COMMAND PORTB
#define LCD_COMMAND_DDR DDRB

void LCD_Init();

void lcd_data(unsigned char data);
void lcd_command(unsigned char cmd);
void lcd_send(unsigned char data);

void LCD_Put_String(char *string);
void LCD_Put_Int(int target);
void LCD_Clear();
void Display_Info(int rpm, int seconds, int active);
void Display_Time_Left(int seconds);
void Update_Time_Left(int seconds);
void Display_Revving_Motor();
void Display_Stopping_Motor();

#endif /* LCD_H_ */