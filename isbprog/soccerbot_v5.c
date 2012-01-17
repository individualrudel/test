/********************************************/
/*          SOCCERBOT V5   25.03.2011       */
/********************************************/

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>  
#include <avr/interrupt.h>
#include <avr/eeprom.h>
//#include <inttypes.h>


#include "soccerbot_v5.h"
#include "eepr.h"

#include "i2cmaster.h"

#define IRSEEKER  0x10
#define KOMPASS  0xC0
#define USDIST1  0xE0
#define USDIST2  0xE4
#define USDIST3  0xE5
#define USDIST4  0xE8

#define ANZAHL_IR_MESSUNGEN 4


volatile uint16_t IR_LOW_DURATION=0;
volatile uint16_t IR_HIGH_DURATION=0;
volatile uint8_t IR_READY=0;


/*************************************/
/*          INITIALISIERUNG          */
/*************************************/

void SOCCERBOT_INIT()
{
  INTERRUPTS_OFF
  i2c_init();
  INTERRUPTS_ON

  // PORT A 
  

  // PORT B
  
  DDRB |= (1 << PB4); 		// PB4 (SCHUSS) als Ausgang festlegen
  DDRB |= (1 << PB5); 		// PB5 (PWM OCR1A Motor links) als Ausgang festlegen
  DDRB |= (1 << PB6); 		// PB6 (PWM OCR1B Motor rechts) als Ausgang festlegen
  DDRB |= (1 << PB7); 		// PB7 (PWM OCR1C Motor hinten) als Ausgang festlegen

	

  // PORT C (Motoren)
  
  DDRC = 0xFF;				// alle Pins als Ausgang festlegen

  
  // PORT D 
 
  DDRD &= ~(1 << PD2);		// PD2 TASTER BALL als Eingang festlegen
  DDRD &= ~(1 << PD3);		// PD3 TASTER RECHTS als Eingang festlegen
  DDRD &= ~(1 << PD4);		// PD4 TASTER LINKS als Eingang festlegen
  DDRD &= ~(1 << PD5);		// PD5 TASTER MITTE RECHTS als Eingang festlegen
  DDRD &= ~(1 << PD6);		// PD5 TASTER MITTE LINKS als Eingang festlegen
  
  PORTD |= (1 << PD2);		// PullUpwiederstand PD2 aktivieren
  PORTD |= (1 << PD3);		// PullUpwiederstand PD3 aktivieren
  PORTD |= (1 << PD4);		// PullUpwiederstand PD4 aktivieren
  PORTD |= (1 << PD5);		// PullUpwiederstand PD5 aktivieren
  PORTD |= (1 << PD6);		// PullUpwiederstand PD6 aktivieren
  
  DDRD |= (1 << PD7); 		// PD7 (LCD EN) als Ausgang festlegen
  

  // PORT E 
  DDRE = 0xFF;	//PORTB als Ausgang
  DDRE &= ~(1 << PE7);	//PORTB PIN 7 als Eingang

  // PORT F (ADC)
  // DDRE = 0x00;		//PORTF als Eingang
  
  // PORT G (LCD)
  
  DDRG = 0xFF;				// alle Pins as output
  
  
  // PWM Motoren
 
  TCCR1A = (1<<COM1A1)|(1<<COM1B1)|(1<<COM1C1)|(1<<WGM10);
  TCCR1B = (1<<CS10);
  TIMSK &= ~0x3c;
  
  OCR1A = 0;
  OCR1B = 0;
  OCR1C = 0;
  
  /*
  //Setup Timer/Counter für IR_Sensoren
  
  // TCCR3A = 0;                      // normal mode, keine PWM Ausgänge
  ETIMSK = (1<<TICIE3)|(1<<TOIE3); 							// Enable Timer3 Capture Event Interrupt + overflow
  TCCR3B |=(0<<CS32)|(1<<CS31)|(0<<CS30)|(0<<ICES3)|(1<<ICNC3);			//Set Prescaler to 0 - Edge detect to falling edge - Input capture noise canceler
  
  
  
  TIFR = (1<<ICF3)|(1<<TOV3); 								// Clear ICF3/clear pending interrupts
  */
  
  
    
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
  
  
  //IR_BALL
  
  uint16_t IR_BALL[] = {0,0,0,0,0,0,0,0};
  
  
  //Startwinkel auf über 360 stellen
  ANGLE_START=400;


  // ADC initialisieren
  
  ADCSRA |= ((1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2) | (1<<ADEN));
  ADMUX = (1<<REFS0) | (1<<REFS1);   // Interne 2,56V als Referenz nehmen 
  ADCSRA |= (1<<ADSC);
  while ( ADCSRA & (1<<ADSC) )
  {
    ;
  }

  
  int8_t robot=Robot();
  
}






/*****************************/
/*          KOMPASS          */
/*****************************/

void KOMPASS_MESSUNG(void)
{
   INTERRUPTS_OFF
   
   
  uint8_t high_byte, low_byte;
  i2c_start(KOMPASS + I2C_WRITE);
  i2c_write(2);
  i2c_stop();
  i2c_start(KOMPASS + I2C_READ);
  high_byte = i2c_readAck();
  low_byte = i2c_readNak();
  i2c_stop();
  WINKEL = (high_byte * 256 + low_byte) / 10;
  
  INTERRUPTS_ON
}


/*****************************/
/*         IR SEEKER V2      */
/*****************************/

void IR_MESSUNG(void)
{

  uint8_t Anzahl=0;
  uint16_t Durchschnitt=0;
  
   INTERRUPTS_OFF
   
   
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
  
  INTERRUPTS_ON
  
  
  if(IR_S1>10)
  {
   Anzahl++;
   Durchschnitt=Durchschnitt+IR_S1;
  }
  
  if(IR_S2>10)
  {
   Anzahl++;
   Durchschnitt=Durchschnitt+IR_S2;
  }
  
  if(IR_S3>10)
  {
   Anzahl++;
   Durchschnitt=Durchschnitt+IR_S3;
  }
  
  if(IR_S4>10)
  {
   Anzahl++;
   Durchschnitt=Durchschnitt+IR_S4;
  }
  
  if(IR_S5>10)
  {
   Anzahl++;
   Durchschnitt=Durchschnitt+IR_S5;
  }
  
  BALL_ENTFERNUNG=Durchschnitt/Anzahl;
  
}
	


/*************************************/
/*          ABSTANDSENSOREN          */
/*************************************/

uint16_t ADCMESSUNG(uint8_t Buchse)
{
 INTERRUPTS_OFF

  uint8_t i = 0;
  uint16_t ergebnis = 0;
  uint8_t kanal = 0;

  if (Buchse==1) kanal = 7;
  if (Buchse==2) kanal = 5;
  if (Buchse==3) kanal = 3;
  if (Buchse==4) kanal = 1;
  if (Buchse==5) kanal = 0;
  if (Buchse==6) kanal = 2;
  if (Buchse==7) kanal = 4;
  if (Buchse==8) kanal = 6;
   
  ADMUX = kanal;
  
  ADMUX |= (1<<REFS1)|(1<<REFS0);   //2,56V als Referenz

if((Buchse==5))
{
 ADMUX  |= (0<<REFS1)|(1<<REFS0);      // AVcc als Referenz benutzen
}
 
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
	
	INTERRUPTS_ON
  return ergebnis;
 
 
}



void ENTFERNUNGSMESSUNG(void)
{
 ENTFERNUNGEN[VORNE]=ADCMESSUNG(IR_VORNE);
 ENTFERNUNGEN[LINKS]=ADCMESSUNG(IR_LINKS);
 ENTFERNUNGEN[HINTEN]=ADCMESSUNG(IR_HINTEN);
 ENTFERNUNGEN[RECHTS]=ADCMESSUNG(IR_RECHTS);
}
 


/*****************************/
/*          MOTOREN          */
/*****************************/

void SCHUSS(void)
{
  INTERRUPTS_OFF

 
  PORTB |= (1<<PB4);
  _delay_ms(100);
  PORTB &= ~(1<<PB4);
  INTERRUPTS_ON
 

}

void DRIBBLER(int8_t Wert)
{

  if (Wert==1)
  
  {
  
    PORTC &= ~(1<<PC1);
    PORTC |= (1<<PC3);
  }
  
  else if(Wert==-1)
  {
   
    PORTC &= ~(1<<PC3);
    PORTC |= (1<<PC1);
  
  }
  else
  {
  
	PORTC |= (1<<PC1);
    PORTC |= (1<<PC3);
  
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
    OCR1A = 255; 
	
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
    OCR1B = 255; 
	
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

    OCR1C = 255; 
	
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
  
  if (PIND & (1<<PD2)) Wert = 0;
  return Wert;
}

uint8_t TASTER1(void)
{
INTERRUPTS_OFF

           
  uint8_t Wert1 = 1;
  uint8_t Wert2 = 0;
  if (PIND & (1<<PD4)) Wert1 = 0;
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
  INTERRUPTS_ON
  return Wert2;
  
 
}

uint8_t TASTER2(void)
{
INTERRUPTS_OFF

 
  uint8_t Wert1 = 1;
  uint8_t Wert2 = 0;
  if (PIND & (1<<PD6)) Wert1 = 0;
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
  INTERRUPTS_ON
  return Wert2; 
 
 
}

uint8_t TASTER3(void)
{
INTERRUPTS_OFF

 
  uint8_t Wert1 = 1;
  uint8_t Wert2 = 0;
  if (PIND & (1<<PD5)) Wert1 = 0;
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
  INTERRUPTS_ON
  return Wert2;
  
 
}

uint8_t TASTER4(void)
{
INTERRUPTS_OFF

 
  uint8_t Wert1 = 1;
  uint8_t Wert2 = 0;
  if (PIND & (1<<PD3)) Wert1 = 0;
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
  INTERRUPTS_ON
  return Wert2;
  
 
}




/*********************************************/
/*                    LCD                    */
/*********************************************/


// ----------- LCD Display initialisieren ----------

void init_lcd(void)
{
  INTERRUPTS_OFF

 
  PORTD &= 0b01111111;			// Start LCD Control   EN=0  (PD7)
  _delay_ms(1);					// Wait LCD Ready
  write_ins(0x33);				// Initial (Set DL=1 3 Time, Reset DL=0 1 Time)
  write_ins(0x32);  
  write_ins(0x28);  			// Function Set (DL=0 4-Bit,N=1 2 Line,F=0 5X7)
  write_ins(0x0C);  			// Display on/off Control (Entry Display,Cursor off,Cursor not Blink)
  write_ins(0x06);  			// Entry Mode Set (I/D=1 Increment,S=0 Cursor Shift)
  write_ins(0x01);  			// Clear Display  (Clear Display,Set DD RAM Address=0)
  _delay_ms(1);			        // Wait Initial Complete
  INTERRUPTS_ON
  return;
  
 
}


// ----------- Set LCD Cursor ----------

void gotolcd(unsigned char i)
{
  INTERRUPTS_OFF
  i |= 0x80;					// Set DD-RAM Address Command
  write_ins(i);
  INTERRUPTS_ON
  return;
  
}


// ---------- Write Instruction to LCD ----------

void write_ins(unsigned char i)
{
INTERRUPTS_ON
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
INTERRUPTS_ON
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
 INTERRUPTS_OFF

 
 
  char *p;
 
  p = lcdbuf;
 
  do 							// Get ASCII & Write to LCD Until null
  {
    write_data(*p); 			// Write ASCII to LCD
    p++;						// Next ASCII
  }
  while(*p != '\0');			// End of ASCII (null)
 
 INTERRUPTS_ON
  return;
  
  
 
}


// ---------- Displayanzeige ----------

void SHOW_LINES()
{
 
 INTERRUPTS_OFF

 

  gotolcd(0);					// Set Cursor Line-1
  strcpy(lcdbuf,LINE_1);
  printlcd();
  gotolcd(0x40);				// Set Cursor Line-2
  strcpy(lcdbuf,LINE_2);
  printlcd();
  
  INTERRUPTS_ON
 	
}


void LCD_CLEAR(uint8_t clear)
{
 strcpy(LINE_1,"                ");
 strcpy(LINE_2,"                ");
 if(clear==1) SHOW_LINES();
}


//---------------
//IR BALLSENSOREN
//---------------


void MUX_INPUT_SELECT(uint8_t input)
{
 if(input>15) input=15;
 
 INTERRUPTS_OFF
 
 PORTE |= (1<<PE7);	//Set MUX Enable Pin to HIGH >> OFF
 
 PORTE = input+16;		//Choose MUX input with pins 0-3 + keep Enable(pin 4) high(+16==0b00010000)
 
 PORTE |= (0<<PE7);	//Set MUX Enable Pin to LOW >> ON

 INTERRUPTS_ON
}

void IR_BALL_MESSUNG(void)
{
 for(uint8_t i=0;i<8;i++)
 {
  MUX_INPUT_SELECT(i);
  IR_BALL[i]=ADCMESSUNG(5);
 }
}

/*
//INTERUPT SERVICE ROUTINES

ISR(TIMER3_CAPT_vect)
{
 TCNT3=0;
 TCCR3B ^=(1<<ICES3); //Toggle Edge detect
 
 if((TCCR3B & (1<<ICES3))==1)		//Falling Edge
 {
  IR_HIGH_DURATION=ICR3;	//Copy Counter value
 }
 else							//Rising Edge
 {
  IR_LOW_DURATION=ICR3;	//Copy Counter value
 }
 		//Clear Counter
 IR_READY++;
 if(IR_READY>200)
 {
  IR_READY=20;
 }
}*/
