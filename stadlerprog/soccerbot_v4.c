/********************************************/
/*          SOCCERBOT V4   04.01.2010       */
/********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "soccerbot_v4.h"

#include "i2cmaster.h"

#define IRSEEKER  0x10
#define KOMPASS  0xC0


/*************************************/
/*          INITIALISIERUNG          */
/*************************************/

void SOCCERBOT_INIT()
{

  // PORT A 
  

  // PORT B
  
  DDRB |= (1 << PB4); 		// PB4 (SCHUSS) als Ausgang festlegen
  DDRB |= (1 << PB5); 		// PB5 (PWM OCR1A Motor links) als Ausgang festlegen
  DDRB |= (1 << PB6); 		// PB6 (PWM OCR1B Motor rechts) als Ausgang festlegen
  DDRB |= (1 << PB7); 		// PB7 (PWM OCR1C Motor hinten) als Ausgang festlegen

	

  // PORT C (Motoren)
  
  DDRC = 0xFF;				// alle Pins als Ausgang festlegen

  
  // PORT D 
  
 
  DDRD &= ~(1 << PD2);		// PD2 TASTER GELB als Eingang festlegen
  DDRD &= ~(1 << PD3);		// PD3 TASTER GELB als Eingang festlegen
  DDRD &= ~(1 << PD4);		// PD4 TASTER GELB als Eingang festlegen
  DDRD &= ~(1 << PD5);		// PD5 TASTER GELB als Eingang festlegen
  DDRD &= ~(1 << PD6);		// PD5 TASTER BALL als Eingang festlegen
  
  PORTD |= (1 << PD2);		// PullUpwiederstand PD2 aktivieren
  PORTD |= (1 << PD3);		// PullUpwiederstand PD3 aktivieren
  PORTD |= (1 << PD4);		// PullUpwiederstand PD4 aktivieren
  PORTD |= (1 << PD5);		// PullUpwiederstand PD5 aktivieren
  PORTD |= (1 << PD6);		// PullUpwiederstand PD6 aktivieren
  
  
  
  DDRD |= (1 << PD7); 		// PD7 (LCD EN) als Ausgang festlegen
  

  // PORT E (IR - Schalteingänge)
  
  DDRE = 0x00;				// alle Pins als Eingang festlegen
  PORTE = 0xFF;				// alle PullUpwiederstände aktivieren
  


  // PORT F (ADC)
  
  
  // PORT G (LCD)
  
  DDRG = 0xFF;				// alle Pins as output
  
  
  // PWM Motoren
 
  TCCR1A = (1<<COM1A1)|(1<<COM1B1)|(1<<COM1C1)|(1<<WGM10);
  TCCR1B = (1<<CS10);
  TIMSK &= ~0x3c;

  
  OCR1A = 0;
  OCR1B = 0;
  OCR1C = 0;
  
  
  
  // LCD
   
  _delay_ms(30);						// Power-on Delay
  init_lcd();							// Initial LCD


  // Taster
  
  TASTER1_GEDRUECKT = 0;
  TASTER2_GEDRUECKT = 0;
  TASTER3_GEDRUECKT = 0;
  TASTER4_GEDRUECKT = 0;


  // IR Seeker
  
  IR_DIRECTION = 0;
  IR_S1 = 0;
  IR_S2 = 0;
  IR_S3 = 0;
  IR_S4 = 0;
  IR_S5 = 0;
  
  //Startwinkel auf über 380 stellen
  ANGLE_START=400;


  // ADC initialisieren
  
  uint16_t bullshit=0;
  
  ADCSRA |= ((1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2) | (1<<ADEN));
  ADMUX = (1<<REFS0) | (1<<REFS1);   // Interne 2,56V als Referenz nehmen 
  ADCSRA |= (1<<ADSC);
  while ( ADCSRA & (1<<ADSC) )
  {
    ;
  }
  bullshit=ADC;

}


/*****************************/
/*          KOMPASS          */
/*****************************/

void KOMPASS_MESSUNG(void)
{
  uint8_t high_byte, low_byte;
  i2c_start(KOMPASS + I2C_WRITE);
  i2c_write(2);
  i2c_stop();
  i2c_start(KOMPASS + I2C_READ);
  high_byte = i2c_readAck();
  low_byte = i2c_readNak();
  i2c_stop();
  WINKEL = (high_byte * 256 + low_byte) / 10;
}


/*****************************/
/*         IR SEEKER V2      */
/*****************************/

void IR_MESSUNG(void)
{
  i2c_start(0x10 + I2C_WRITE);
  i2c_write(0x49);
  i2c_stop();
  i2c_start(0x10 + I2C_READ);
  IR_DIRECTION = i2c_readAck();
  IR_S1 = i2c_readAck();
  IR_S2 = i2c_readAck();
  IR_S3 = i2c_readAck();
  IR_S4 = i2c_readAck();
  IR_S5 = i2c_readNak();
  i2c_stop();
}
	


/********************************************/
/*                    IR                    */
/********************************************/

void IR_EMPFANG(void)
{

  IR_1 = 0;
  IR_2 = 0; 
  IR_3 = 0; 
  IR_4 = 0; 
  IR_5 = 0;
  IR_6 = 0; 
  IR_7 = 0; 
  IR_8 = 0;

  if (PINE & (1<<PE3)) IR_1 = 1;
  if (PINE & (1<<PE1)) IR_2 = 1;
  if (PINE & (1<<PE0)) IR_3 = 1;
  if (PINE & (1<<PE2)) IR_4 = 1;
  if (PINE & (1<<PE4)) IR_5 = 1;
  if (PINE & (1<<PE6)) IR_6 = 1;
  if (PINE & (1<<PE7)) IR_7 = 1;
  if (PINE & (1<<PE5)) IR_8 = 1;

}



/*************************************/
/*          ABSTANDSENSOREN          */
/*************************************/

uint16_t ENTFERNUNG_VORNE()
{
  uint8_t i = 0;
  uint16_t ergebnis = 0;
  uint8_t kanal = 0;

  kanal = 7;
   
  ADMUX = kanal;			
  ADMUX |= (1<<REFS1)|(1<<REFS0);   //Interne 2,56 als Referenz	


  while (i!=3)
  {	
    ADCSRA |= (1<<ADSC);		
    while ( ADCSRA & (1<<ADSC) )
    {
      ;                 
    }
    i++;
    ergebnis += ADC;
  }
  ergebnis = ergebnis/3;	

  return ergebnis;

}

uint16_t ENTFERNUNG_LINKS()
{
  uint8_t i = 0;
  uint16_t ergebnis = 0;
  uint8_t kanal = 0;

  kanal = 5;
   
  ADMUX = kanal;			
  ADMUX |= (1<<REFS1)|(1<<REFS0);   		

  while (i!=3)
  {	
    ADCSRA |= (1<<ADSC);		
    while ( ADCSRA & (1<<ADSC) )
    {
      ;                 
    }
    i++;
    ergebnis += ADC;
  }
  ergebnis = ergebnis/3;	

  return ergebnis;

}

uint16_t ENTFERNUNG_HINTEN()
{
  uint8_t i = 0;
  uint16_t ergebnis = 0;
  uint8_t kanal = 0;

  kanal = 3;
   
  ADMUX = kanal;			
  ADMUX |= (1<<REFS1)|(1<<REFS0);   		

  while (i!=3)
  {	
    ADCSRA |= (1<<ADSC);		
    while ( ADCSRA & (1<<ADSC) )
    {
      ;                 
    }
    i++;
    ergebnis += ADC;
  }
  ergebnis = ergebnis/3;	

  return ergebnis;

}

uint16_t ENTFERNUNG_RECHTS()
{
  uint8_t i = 0;
  uint16_t ergebnis = 0;
  uint8_t kanal = 0;

  kanal = 1;
   
  ADMUX = kanal;			
  ADMUX |= (1<<REFS1)|(1<<REFS0);   		

  while (i!=3)
  {	
    ADCSRA |= (1<<ADSC);		
    while ( ADCSRA & (1<<ADSC) )
    {
      ;                 
    }
    i++;
    ergebnis += ADC;
  }
  ergebnis = ergebnis/3;	

  return ergebnis;

}


/*****************************/
/*          MOTOREN          */
/*****************************/

void SCHUSS(void)
{

  PORTB |= (1<<PB4);
  _delay_ms(100);
  PORTB &= ~(1<<PB4);


}

void DRIBBLER(int8_t Wert)
{

  if (Wert==1)
  
  {
  
     PORTC &= ~(1<<PC1);
     PORTC |= (1<<PC3);
  }
  
  else
  {
   if (Wert==-1)
  
  {
	 PORTC |= (1<<PC1);
     PORTC &= ~(1<<PC3);
  }
  else
  {
    PORTC |= (1<<PC1);
    PORTC |= (1<<PC3);
  }
  }

}

void MOTOR_LINKS(int16_t Wert)
{

  if (Wert>255) Wert = 255;
  
  if (Wert<-255) Wert = -255;
  
  if (Wert==0)
  
  {
  
    PORTC &= ~(1<<PC5);
    PORTC &= ~(1<<PC7);
 	//PORTB &= ~(1<<PB5);
    OCR1A = 0; 
	
  }
  
  if (Wert>0)
  
  {
  
    PORTC |= (1<<PC5);
    PORTC &= ~(1<<PC7);
	//PORTB |= (1<<PB5);
    OCR1A = Wert; 
	
  }
  
  if (Wert<0)
  
  {
  
    PORTC &= ~(1<<PC5);
    PORTC |= (1<<PC7);
 	//PORTB |= (1<<PB5);
   OCR1A = -1 * Wert; 
	
  }

}


void MOTOR_RECHTS(int16_t Wert)
{

  if (Wert>255) Wert = 255;
  
  if (Wert<-255) Wert = -255;
  
  if (Wert==0)
  
  {
  
    PORTC &= ~(1<<PC4);
    PORTC &= ~(1<<PC6);
	//PORTB &= ~(1<<PB6);
    OCR1B = 0; 
	
  }
  
  if (Wert>0)
  
  {
  
    PORTC &= ~(1<<PC4);
    PORTC |= (1<<PC6);
   // 	PORTB |= (1<<PB6);
    OCR1B = Wert; 
	
  }
  
  if (Wert<0)
  
  {
  
    PORTC |= (1<<PC4);
    PORTC &= ~(1<<PC6);
    //	PORTB |= (1<<PB6);
    OCR1B = -1 * Wert; 
	
  }

}


void MOTOR_HINTEN(int16_t Wert)
{

  if (Wert>255) Wert = 255;
  
  if (Wert<-255) Wert = -255;
  
  if (Wert==0)
  
  {
  
    PORTC &= ~(1<<PC0);
    PORTC &= ~(1<<PC2);
	//    	PORTB &= ~(1<<PB7);

    OCR1C = 0; 
	
  }
  
  if (Wert>0)
  
  {
  
    PORTC &= ~(1<<PC0);
    PORTC |= (1<<PC2);
   // 	PORTB |= (1<<PB7);
   OCR1C = Wert; 
	
  }
  
  if (Wert<0)
  
  {
  
    PORTC |= (1<<PC0);
    PORTC &= ~(1<<PC2);
   //	PORTB |= (1<<PB7);
     OCR1C = -1 * Wert; 
	
  }

}


void FAHREN(int16_t Wert)
{

  MOTOR_HINTEN(0);
  MOTOR_LINKS(-1*Wert);
  MOTOR_RECHTS(Wert);  

}


void ROTIEREN(int16_t Wert)
{

  MOTOR_HINTEN(Wert);
  MOTOR_LINKS(Wert);
  MOTOR_RECHTS(Wert);  

}



/************************************************/
/*                    TASTER                    */
/************************************************/

uint8_t TASTER_BALL(void)
{
  uint8_t Wert = 1;
  
  if (PIND & (1<<PD6)) Wert = 0;
  return Wert;
}

uint8_t TASTER1(void)
{
  uint8_t Wert1 = 1;
  uint8_t Wert2 = 0;
  if (PIND & (1<<PD3)) Wert1 = 0;
  if (Wert1==1)
  {
    if (TASTER1_GEDRUECKT==0)
	{
      Wert2 = 1;
	}
	TASTER1_GEDRUECKT = 1;
  }
  else
  {
	TASTER1_GEDRUECKT = 0;
  } 
  return Wert2;
}

uint8_t TASTER2(void)
{
  uint8_t Wert1 = 1;
  uint8_t Wert2 = 0;
  if (PIND & (1<<PD2)) Wert1 = 0;
  if (Wert1==1)
  {
    if (TASTER2_GEDRUECKT==0)
	{
      Wert2 = 1;
	}
	TASTER2_GEDRUECKT = 1;
  }
  else
  {
    TASTER2_GEDRUECKT = 0;
  }
  return Wert2;  
}

uint8_t TASTER3(void)
{
  uint8_t Wert1 = 1;
  uint8_t Wert2 = 0;
  if (PIND & (1<<PD4)) Wert1 = 0;
  if (Wert1==1)
  {
    if (TASTER3_GEDRUECKT==0)
	{
      Wert2 = 1;
	}
    TASTER3_GEDRUECKT = 1;
  }
  else
  {
    TASTER3_GEDRUECKT = 0;
  }
  return Wert2;
}

uint8_t TASTER4(void)
{
  uint8_t Wert1 = 1;
  uint8_t Wert2 = 0;
  if (PIND & (1<<PD5)) Wert1 = 0;
  if (Wert1==1)
  {
    if (TASTER4_GEDRUECKT==0)
	{
      Wert2 = 1;
	}
	TASTER4_GEDRUECKT = 1;
  }
  else
  {
    TASTER4_GEDRUECKT = 0;
  }
  return Wert2;
}




/*********************************************/
/*                    LCD                    */
/*********************************************/


// ----------- LCD Display initialisieren ----------

void init_lcd(void)
{
  PORTD &= 0b01111111;			// Start LCD Control   EN=0  (PD7)
  _delay_ms(1);					// Wait LCD Ready
  write_ins(0x33);				// Initial (Set DL=1 3 Time, Reset DL=0 1 Time)
  write_ins(0x32);  
  write_ins(0x28);  			// Function Set (DL=0 4-Bit,N=1 2 Line,F=0 5X7)
  write_ins(0x0C);  			// Display on/off Control (Entry Display,Cursor off,Cursor not Blink)
  write_ins(0x06);  			// Entry Mode Set (I/D=1 Increment,S=0 Cursor Shift)
  write_ins(0x01);  			// Clear Display  (Clear Display,Set DD RAM Address=0)
  _delay_ms(1);			        // Wait Initial Complete
  return;
}


// ----------- Set LCD Cursor ----------

void gotolcd(unsigned char i)
{
  i |= 0x80;					// Set DD-RAM Address Command
  write_ins(i);  
  return;
}


// ---------- Write Instruction to LCD ----------

void write_ins(unsigned char i)
{
  PORTG  &= 0b11101111;			// Instruction Select RS=0(PG4)

  PORTG &= 0xF0;           		// Clear old LCD Data (Bit[3..0])
  PORTG |= (i>>4) & 0x0F;     	// Strobe High Nibble Command
  PORTD |= 0b10000000;    		// Enable ON    EN=1(PD7)
  _delay_ms(1);
  PORTD &= 0b01111111;   		// Enable OFF   EN=0(PD7)
  
  PORTG &= 0xF0;           		// Clear old LCD Data (Bit[3..0])
  PORTG |= i & 0x0F;  			// Strobe Low Nibble Command
  PORTD |= 0b10000000;    		// Enable ON    EN=1(PD7)
  _delay_ms(1);
  PORTD &= 0b01111111;   		// Enable OFF   EN=0(PD7)
  _delay_ms(1);					// Wait LCD Busy

  return;
}


// ---------- Write Data(ASCII) to LCD ----------

void write_data(unsigned char i)
{
  PORTG  |= 0b00010000;			// Instruction Select  RS=1(PG4)

  PORTG &= 0xF0;           		// Clear old LCD Data (Bit[3..0])
  PORTG |= (i>>4) & 0x0F;       	// Strobe High Nibble Command
  PORTD |= 0b10000000;    		// Enable ON    EN=1(PD7)
  _delay_ms(1);
  PORTD &= 0b01111111;   		// Enable OFF   EN=0(PD7)
  
  PORTG &= 0xF0;           		// Clear old LCD Data (Bit[3..0])
  PORTG |= i & 0x0F;  			// Strobe Low Nibble Command
  PORTD |= 0b10000000;    		// Enable ON    EN=1(PD7)
  _delay_ms(1);
  PORTD &= 0b01111111;   		// Enable OFF   EN=0(PD7) 
  _delay_ms(1);					// Wait LCD Busy

  return;						

}


// ---------- Print Data(ASCII) to LCD -----------

void printlcd(void)
{
  char *p;
 
  p = lcdbuf;
 
  do 							// Get ASCII & Write to LCD Until null
  {
    write_data(*p); 			// Write ASCII to LCD
    p++;						// Next ASCII
  }
  while(*p != '\0');			// End of ASCII (null)
 
  return;
}


// ---------- Displayanzeige ----------

void SHOW_LINES(void)
{
  gotolcd(0);					// Set Cursor Line-1
  strcpy(lcdbuf,LINE_1);
  printlcd();
  gotolcd(0x40);				// Set Cursor Line-2
  strcpy(lcdbuf,LINE_2);
  printlcd();
}


void LCD_CLEAR(void)
{
 strcpy(LINE_1,"                ");
 strcpy(LINE_2,"                ");
 SHOW_LINES();
}