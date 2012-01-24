#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>


#include "addons.h"
#include "menu.h"
#include "i2cmaster.h"
#include "soccerbot_v5.h"
#include "main.h"

/*
sprintf(Zahl,"%3d",VARIABLE);
for (uint8_t i=0; i<=LAENGE-1; i++) LINE_2[i+3]=Zahl[i];

SHOW_LINES();
*/


void SUB1(void)	//IR Sensoren
{

 //sei();
 
 uint8_t exit=0;
 uint8_t input=0;
// uint16_t output=0;
 
 
 
 
 strcpy(LINE_1,"IR Sensoren     ");
 strcpy(LINE_2,"                ");
 
 while(exit==0)
 {
 
 for(uint8_t i=0;i<8;i++)
 {
  MUX_INPUT_SELECT(i);
  IR_BALL[i]=ADCMESSUNG(5);
 }
 
 
 
 
/* sprintf(Zahl,"%3d",IR_BALL[0]);
 for (uint8_t i=0; i<=3; i++) LINE_1[i]=Zahl[i];
 
 sprintf(Zahl,"%3d",IR_BALL[1]);
 for (uint8_t i=0; i<=3; i++) LINE_1[i+4]=Zahl[i];
 
 sprintf(Zahl,"%3d",IR_BALL[2]);
 for (uint8_t i=0; i<=3; i++) LINE_1[i+8]=Zahl[i];
 
 sprintf(Zahl,"%3d",IR_BALL[3]);
 for (uint8_t i=0; i<=3; i++) LINE_1[i+12]=Zahl[i];
 
 sprintf(Zahl,"%3d",IR_BALL[4]);
 for (uint8_t i=0; i<=3; i++) LINE_2[i]=Zahl[i];
 
 sprintf(Zahl,"%3d",IR_BALL[5]);
 for (uint8_t i=0; i<=3; i++) LINE_2[i+4]=Zahl[i];
 
 sprintf(Zahl,"%3d",IR_BALL[6]);
 for (uint8_t i=0; i<=3; i++) LINE_2[i+8]=Zahl[i];
 
 sprintf(Zahl,"%3d",IR_BALL[7]);
 for (uint8_t i=0; i<=3; i++) LINE_2[i+12]=Zahl[i];*/
 
 
 sprintf(Zahl,"%3d",input);
 for (uint8_t i=0; i<=2; i++) LINE_1[i+11]=Zahl[i];
 
 sprintf(Zahl,"%3d",IR_BALL[input]);
 for (uint8_t i=0; i<=3; i++) LINE_2[i]=Zahl[i];

 

 SHOW_LINES();

 
 
 
  if(TASTER1()==1) //BUTTON LEFT PRESSED
	{
	 if(input>0)
	 {
	  input--;
	 }
	}
	
	if(TASTER2()==1) //BUTTON RIGHT PRESSED
	{
	 if(input<15)
	 {
	 input++;
	 }
	}
 
 if(TASTER4()==1) //BUTTON BACK PRESSED
	{
	 exit=1;
	}
	
 }
 exit=0;
 
 
}

void SUB2(void)	//Lego IR
{
 
 uint8_t exit=0;
 
 strcpy(LINE_1,"Lego IR         ");
 strcpy(LINE_2,"                ");
 
 while(exit==0)
 {
 

	  IR_MESSUNG();
      
	  
	  
	  sprintf(Zahl,"%3d",IR_DIRECTION);
	  for (uint8_t i=0; i<=2; i++) LINE_1[9]=Zahl[i];
	  
	  sprintf(Zahl,"%3d",BALL_ENTFERNUNG);
	  for (uint8_t i=0; i<=2; i++) LINE_1[i+12]=Zahl[i];
	  
      sprintf(Zahl,"%3d",IR_S1);
	  for (uint8_t i=0; i<=2; i++) LINE_2[i]=Zahl[i];
      
	  sprintf(Zahl,"%3d",IR_S2);
	  for (uint8_t i=0; i<=2; i++) LINE_2[i+3]=Zahl[i];
      
	  sprintf(Zahl,"%3d",IR_S3);
	  for (uint8_t i=0; i<=2; i++) LINE_2[i+6]=Zahl[i];
	  
	  sprintf(Zahl,"%3d",IR_S4);
	  for (uint8_t i=0; i<=2; i++) LINE_2[i+9]=Zahl[i];
	
	  sprintf(Zahl,"%3d",IR_S5);
	  for (uint8_t i=0; i<=2; i++) LINE_2[i+12]=Zahl[i];
 
 
 
 SHOW_LINES();
 
 
 
 if(TASTER4()==1) //BUTTON BACK PRESSED
	{
	 exit=1;
	}
 }
 exit=0;
 
}

void SUB3(void) //Entfernungen
{
 uint8_t exit=0;
 
 strcpy(LINE_1,"V:  L:  H:  R:  ");
 
 while(exit==0)
 {
  
  ENTFERNUNGEN[VORNE]=ADCMESSUNG(IR_VORNE);
  ENTFERNUNGEN[LINKS]=ADCMESSUNG(IR_LINKS);
  ENTFERNUNGEN[HINTEN]=ADCMESSUNG(IR_HINTEN);
  ENTFERNUNGEN[RECHTS]=ADCMESSUNG(IR_RECHTS);
  
  
  
  
  sprintf(Zahl,"%3d",ENTFERNUNGEN[VORNE]);
  for (uint8_t i=0; i<=2; i++) LINE_2[i+0]=Zahl[i];
  
  sprintf(Zahl,"%3d",ENTFERNUNGEN[LINKS]);
  for (uint8_t i=0; i<=2; i++) LINE_2[i+4]=Zahl[i];
  
  sprintf(Zahl,"%3d",ENTFERNUNGEN[HINTEN]);
  for (uint8_t i=0; i<=2; i++) LINE_2[i+8]=Zahl[i];
  
  sprintf(Zahl,"%3d",ENTFERNUNGEN[RECHTS]);
  for (uint8_t i=0; i<=2; i++) LINE_2[i+12]=Zahl[i];

SHOW_LINES();
  
  
  
  if(TASTER4()==1) //BUTTON BACK PRESSED
	{
	 exit=1;
	}
 }
 exit=0;
}

void SUB4(void)	//Kompass
{
 
 uint8_t exit=0;
 
 strcpy(LINE_1,"Kompass         ");
 
 while(exit==0)
 {
 
 KOMPASS_MESSUNG();
 
 
 sprintf(Zahl,"%3d",WINKEL);
 for (uint8_t i=0; i<=2; i++) LINE_2[i+3]=Zahl[i];
 
 
 SHOW_LINES();
 
 
 
 if(TASTER4()==1) //BUTTON BACK PRESSED
	{
	 exit=1;
	}
 }
 exit=0;
}

void SUB5(void)	//Schuss
{
 SCHUSS();
}

void SUB6(void)	//Dribbler
{
 
 uint8_t exit=0;
 
 strcpy(LINE_1,"Dribbler        ");
 
 DRIBBLER(1);
 
 int8_t direction=1;
 int8_t old_direction=1;
 
 while(exit==0)
 {
 
 
 sprintf(Zahl,"%3d",direction);
 for (uint8_t i=0; i<=2; i++) LINE_2[i+3]=Zahl[i];
 
 
 
 SHOW_LINES();
 
 if(TASTER1()==1) //BUTTON
	{
	  direction=-1;
	}
	
	if(TASTER2()==1) //BUTTON
	{
	 direction=0;
	}
	
	if(TASTER3()==1) //BUTTON
	{
	 direction=1;
	}
	
	if(direction!=old_direction)
	{
	 DRIBBLER(direction);
	}
	
	old_direction=direction;
 
 if(TASTER4()==1) //BUTTON BACK PRESSED
	{
	 exit=1;
	}
 }
 
 exit=0;
 
}
void SUB7(void)	//Fahren
{


//**********************************************************
//1: Speed links  2: Speed rechts  3: Speed hinten  4: Fahren
//**********************************************************
 
 uint8_t exit=0;
 
// strcpy(LINE_1,"IR Sensoren     ");


int16_t Speedl=0;
int16_t Speedr=0;
int16_t Speedh=0;
uint8_t State=0;
uint8_t change=0;
 
 while(exit==0)
 {
 
 
 if(TASTER1()==1) //BUTTON LEFT PRESSED
	{
	
	
	 if(change==0)
	 {
	  
	  if(State!=0)
	  {
	   State--;
	  }
	  
	 }
	 else
	 {
	 
	 
	  if(State==0)
	  {
	   Speedl=Speedl-10;
	  }
	  
	  if(State==1)
	  {
	   Speedr=Speedr-10;
	  }
	  
	  if(State==2)
	  {
	   Speedh=Speedh-10;
	  }
	  
	  if(State==3)
	  {
	   change=0;
	  }
	  
	  
	  if(Speedl>255) Speedl=-250;
	  if(Speedl<-255) Speedl=250;
	  
	  if(Speedr>255) Speedr=-250;
	  if(Speedr<-255) Speedr=250;
	  
	  if(Speedh>255) Speedh=-250;
	  if(Speedh<-255) Speedh=250;
	  
	  
	 }
	 
	 strcpy(LINE_2,"                ");
	 SHOW_LINES();
	}
	
 if(TASTER2()==1) //BUTTON RIGHT PRESSED
	{
	 if(change==0)
	 {
	  
	  if(State!=3)
	  {
	   State++;
	  }
	  
	 }
	 else
	 {
	 
	 
	  if(State==0)
	  {
	   Speedl=Speedl+10;
	  }
	  
	  if(State==1)
	  {
	   Speedr=Speedr+10;
	  }
	  
	  if(State==2)
	  {
	   Speedh=Speedh+10;
	  }
	  
	  if(State==3)
	  {
	   change=0;
	  }
	  
	  
	  if(Speedl>255) Speedl=-250;
	  if(Speedl<-255) Speedl=250;
	  
	  if(Speedr>255) Speedr=-250;
	  if(Speedr<-255) Speedr=250;
	  
	  if(Speedh>255) Speedh=-250;
	  if(Speedh<-255) Speedh=250;
	  
	 }
	 
	 strcpy(LINE_2,"                ");
	 SHOW_LINES();
	}
	
 if(TASTER3()==1) //BUTTON ACCCEPT PRESSED
	{
	 if(change==0)
	 {
	  change=1;
	 }
	 else
	 {
	  change=0;
	 }
	 
	 if(State==3)
	 {
	  MOTOR_LINKS(Speedl);
	  MOTOR_RECHTS(Speedr);
	  MOTOR_HINTEN(Speedh);
	 }
	 strcpy(LINE_2,"                ");
	 SHOW_LINES();
	}
	
	if(change==0)
	{
	 MOTOR_LINKS(0);
	 MOTOR_RECHTS(0);
	 MOTOR_HINTEN(0);
	}
	
	
	
  if(State==0)
  {
   strcpy(LINE_1,"Speed links     ");
   
   sprintf(Zahl,"%3d",Speedl);
   for (uint8_t i=0; i<=3; i++) LINE_2[i+3]=Zahl[i];
  }
  
  if(State==1)
  {
   strcpy(LINE_1,"Speed rechts    ");
   
   sprintf(Zahl,"%3d",Speedr);
   for (uint8_t i=0; i<=3; i++) LINE_2[i+3]=Zahl[i];
  }
  
  if(State==2)
  {
   strcpy(LINE_1,"Speed hinten    ");
   
   sprintf(Zahl,"%3d",Speedh);
   for (uint8_t i=0; i<=3; i++) LINE_2[i+3]=Zahl[i];
  }
  
  if(State==3)	//Fahren
  {
   strcpy(LINE_1,"Fahren          ");
   
   sprintf(Zahl,"%3d",Speedl);
   for (uint8_t i=0; i<=3; i++) LINE_2[i+0]=Zahl[i];
   
   sprintf(Zahl,"%3d",Speedr);
   for (uint8_t i=0; i<=3; i++) LINE_2[i+5]=Zahl[i];
   
   sprintf(Zahl,"%3d",Speedh);
   for (uint8_t i=0; i<=3; i++) LINE_2[i+10]=Zahl[i];
  }
 

 SHOW_LINES();
 
 if(TASTER4()==1) //BUTTON BACK PRESSED
	{
	 exit=1;
	}
 }
 exit=0;
 
}



void Calibrate_Compass(uint8_t gui)
{
 if(gui==1)
 {
  uint8_t exit=0;
 
 strcpy(LINE_1,"Starting Angle  ");
 
 while(exit==0)
 {
 
 KOMPASS_MESSUNG();
 
 
 
 sprintf(Zahl,"%3d",WINKEL);
 for (uint8_t i=0; i<=2; i++) LINE_2[i+3]=Zahl[i];
 
 sprintf(Zahl,"%3d",ANGLE_START);
 for (uint8_t i=0; i<=2; i++) LINE_2[i+9]=Zahl[i];
 
 SHOW_LINES();
 
 if(TASTER3()==1) //BUTTON BACK PRESSED
	{
	ANGLE_START=WINKEL;
	exit=1;
	}
 
 if(TASTER4()==1) //BUTTON BACK PRESSED
	{
	 exit=1;
	}
 }
 exit=0;
 }
 else
 {
  KOMPASS_MESSUNG();
  ANGLE_START=WINKEL;
 }
}


/*uint16_t READ_IR(uint8_t sensor)
{
  uint8_t i = 0;
  uint16_t ergebnis = 0;
  uint8_t kanal = 0;

  kanal = 2;
   
  ADMUX = kanal;			
 // ADMUX |= (1<<REFS1) | (1<<REFS0);   	
  ADMUX |= (0<<REFS1) | (1<<REFS0);      // AVcc als Referenz benutzen	

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

}*/