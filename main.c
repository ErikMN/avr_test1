#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "LCD_Driver.h"

/****************************
 * DDRX    0: IN  1: OUT    *
 * PINX    READ             *
 * PORTX   WRITE            *
 * X       A,B,C,D          *
/****************************/

#define F_CPU 16000000UL

void
init()
{
  PORTA = 0x00;  DDRA = 0x00;
  PORTB = 0x00;  DDRB = 0xFF;
  PORTC = 0x00;  DDRC = 0x00;
  PORTD = 0x00;  DDRD = 0x00;
}

void
adc_init() // Function to initialise the ADC feature
{
  ADCSRA = 0x00; // Clear previous ADC results and status flags
  ADMUX  = 0x60; // 0x40 for 10 bits
  ADCSRA = 0x87; // We have set the ADSC bit to start a conversion, and the
                 // ADPS bits are set so that the prescaler is 128
  ADCSRA = 0x80; // ADEN is set, to enable the ADC
}

unsigned char
adc_start(unsigned char channel) // Function to perform an ADC conversion, Takes 0-8 as input
{                         // to select which input to convert
  unsigned char i;
  ADCH = 0x00;            // Clear the previous result

  i = channel & 0x07;     // Decide which line to perform ADC conversion on
  ADMUX = i | 0x60;       // Enter which line to perform in the ADC control register
  ADCSRA |= 1 << ADSC;

  while(ADCSRA & (1 << ADSC)) // wait for conv. to complete
    ;

  //unsigned char temp = ADCH; // unsigned int temp=ADC;   for 10 bits
  //return temp;

  return ADCH;
}

int
main()
{
  init();
  LCD_Init();
  SREG |= 0x80; // Global Interrupt Enable
  adc_init();
  LCD_Light(1);

  LCD_PlaceString(0, "ADC val: ");

  unsigned char val;
  for(;;)
  {
    val = adc_start(0);
    LCD_PlaceByte(9, val);

    if(val > 85)
      LCD_PlaceString(16, "Danger! > 85");
    else
      LCD_PlaceString(16, "            ");

    _delay_ms(50);

    //LCD_Clear();
    LCD_PlaceString(9, "     ");

  }

}

/*
  //srand(adc_start(0));
  //unsigned char val = rand() % 6 + 1;
  //LCD_PlaceString(0, "Hello World!");

  int i;
  for(i = 0; i < 3; i++)
  {
    LCD_Light(1);
    _delay_ms(5);
    LCD_Light(0);
    _delay_ms(5);
  }
  LCD_Light(1);


  #define LED_RD (PORTA ^= 0x01)
  #define RD_ON  (PORTA &= ~0x01)
  #define RD_OFF (PORTA |= 0x01)

  #define LED_YL (PORTA ^= 0x02)
  #define YL_ON  (PORTA &= ~0x02)
  #define YL_OFF (PORTA |= 0x02)

  #define LED_GR (PORTA ^= 0x04)
  #define GR_ON  (PORTA &= ~0x04)
  #define GR_OFF (PORTA |= 0x04)
*/
