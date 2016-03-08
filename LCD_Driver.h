/*** Driver for HD44780 Character LCD ***/

/** Starts the display */
void LCD_Init();

/** Busy Flag test */
void BF_Test();

/** Send command */
void LCD_WriteCommand(unsigned char cmd);

/** Write a character */
void LCD_WriteCharacter(unsigned char character);

/** Write a character to a specific position */
void LCD_PlaceCharacter(unsigned char position, unsigned char character);

/** Write a string to the display */
void LCD_PlaceString(unsigned char position, const unsigned char *address);

/** Write a byte to the display */
void LCD_PlaceByte(unsigned char position, unsigned char byte);

/** Place cursor */
void LCD_PlaceCursor(unsigned char position);

/** Clears the display */
void LCD_Clear();

/** Backlight on/off (1 or 0) */
void LCD_Light(unsigned char c);
