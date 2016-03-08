/*
 *  Driver for HD44780 Character LCD
 *  (c) 2012 ErikMN
 *
 *  Bit:   7    6     5  4     3     2    1      0
 *  OUT: |Data|Data|Data|Data|Light|  E  | R/W | RS  |
 *
 */

#include <avr/io.h>
#include <util/delay.h>
#include "LCD_Driver.h"

//#define F_CPU 16000000UL

#define OUT      PORTB
#define DATA_DIR DDRB
#define IN       PINB
#define E_HIGH   OUT  |=  0x04
#define E_LOW    OUT  &= ~0x04

void
E_Pulse()
{
  OUT  |= 0x04;             // e-pulse 1
  _delay_us(0.45);          // wait 0.45 ms
  OUT  &= ~0x04;            // e-pulse 0
}

void
BF_Test()                   // wait for display to become ready
{
  DATA_DIR   &=  0x0F;      // LCD-databus set as inport
  OUT  &= ~0x01;            // RS (Register Select) set as 0
  OUT  |=  0x02;            // R/W (Read/Write) set as 1 (read)
  E_HIGH;                   // enable-pulse set as 1

  while ((IN & 0x80))       // read BF (bit 7 OUT) wait until it returns 0
    ;

  _delay_us(0.45);
  E_LOW;                    // enable-pulse set as 0
  //E_Pulse();              // don't need if we wait before setting E to low
}

/** The 4 MSB in OUT gets the 4 MSB of cmd and then the 4 LSB of cmd, E_Pulse between. */
void
LCD_WriteCommand(unsigned char cmd)
{
  DATA_DIR  |=  0xF0;       // LCD-databus set as outport
  OUT &= ~0x03;             // RS and R/W set as 0 (write)

  OUT |=  0xF0;
  OUT &=  (cmd | 0x0F);

  E_Pulse();

  OUT |=  0xF0;
  OUT &=  ((cmd << 4) | 0x0F);

  E_Pulse();
  BF_Test();                // wait until display gets ready for next instr.
}

/** Same as LCD_WriteCommand, difference: RS = 1 */
void
LCD_WriteCharacter(unsigned char character)
{
  E_HIGH;

  DATA_DIR  |=  0xF0;       // LCD-databus set as outport
  OUT &= ~0x02;             // R/W set as 0 (write)
  OUT |=  0x01;             // RS set as 1

  OUT |=  0xF0;
  OUT &=  (character | 0x0F);

  E_Pulse();

  OUT |=  0xF0;
  OUT &=  ((character << 4) | 0x0F);

  E_Pulse();
  BF_Test();
}

void
LCD_PlaceCharacter(unsigned char position, unsigned char character)
{
  if(position > 15) position += 48;  // Place text on next line
  LCD_WriteCommand(0x80 | position);
  LCD_WriteCharacter(character);
}

void
LCD_PlaceString(unsigned char position, const unsigned char *address)
{
  if(position > 15) position += 48;
  LCD_WriteCommand(0x80 | position);
  while (*address != '\0') LCD_WriteCharacter(*address++);
}

void
LCD_PlaceByte(unsigned char position, unsigned char byte)
{
  if(position > 15) position += 48;
  LCD_WriteCommand(0x80 | position);

  unsigned char hundred, ten, one;
  hundred = byte / 100;
  ten = (byte % 100) / 10;
  one = (byte % 100) % 10;

  hundred |=  0x30; // creates ASCII
  ten   |=  0x30;
  one   |=  0x30;

  if (hundred != 0x30) LCD_WriteCharacter(hundred);
  LCD_WriteCharacter(ten);
  LCD_WriteCharacter(one);
}

void
LCD_PlaceCursor(unsigned char position)
{
  if(position > 15) position += 48;
  LCD_WriteCommand(0x80 | position);
}

void
LCD_Init()
{
  OUT &= ~0x01; // RS 0
  _delay_ms(15);

  //OUT = 0x30; // 8-bit interface
  //E_Pulse();
  //_delay_ms(5);

  OUT = 0x30;
  E_Pulse();
  _delay_us(100);

  OUT = 0x30;
  E_Pulse();
  _delay_us(40);

  OUT = 0x20;
  E_Pulse();
  _delay_us(40);

  LCD_WriteCommand(0x28);        // 4-bit, 2-lines 0x28
  //LCD_WriteCommand(0x20);      // 4-bit, 1-line 0x20
  LCD_WriteCommand(0x01);        // display clear
  _delay_us(1640);
  LCD_WriteCommand(0x06);        // cursor increment 0x06
  LCD_WriteCommand(0x0C);        // disp on, cursor blink 0x0E, no cursor 0x0C
}

void LCD_Clear()
{
  LCD_WriteCommand(0x01);
}

void LCD_Light(unsigned char c)
{
  if(c)
    OUT |=  0x08;
  else
    OUT &= ~0x08;
}
