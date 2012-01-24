#include <avr/io.h>
#include <string.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "i2cmaster.h"
#include <util/delay.h>

#include "main.h"
#include "soccerbot_v5.h"


#define MAXSTAT 2

#define WINKEL_KORREKTUR 20

uint8_t Run_main(void)
{

  // Globale Variablen
  
  uint8_t Exit=0;
  
  uint8_t STATUS;
  
  uint8_t stat;
  
  int8_t Rel_Ball_Pos;
  
  int16_t MotorLinksSpeed;
  
  int16_t MotorRechtsSpeed;
  
  int16_t MotorHintenSpeed;
  
  int16_t KorrekturMotoren;
  
  int16_t RelativWinkel;
  uint8_t smoothturn;
  uint8_t oldwinkel;
  uint8_t twinkel;
  char Zahl[4];
  
  
  // Initialisierung
  
 
  SOCCERBOT_INIT();
  
  STATUS = 0;
  
  stat=0;
  
  Rel_Ball_Pos=0;
  
  BALL_ENTFERNUNG=0;
  
  MotorLinksSpeed = 0;

  MotorRechtsSpeed = 0;

  MotorHintenSpeed = 0;
  
  KorrekturMotoren = 0;
  
  RelativWinkel = 0;
  smoothturn=2;
  twinkel=200;
  oldwinkel=0;
  
  strcpy(LINE_1,"Individual Rudel");
  strcpy(LINE_2,"Soccer A - V0.1 ");

  
  ANGLE_START=WINKEL;
  while(Exit==0)
  
  {
	if(twinkel==200)
	{
		oldwinkel=WINKEL;
		twinkel=0;
	}
	twinkel++;
    // ========== Automatische Sensorauslesung ==========
	
	KOMPASS_MESSUNG();
	IR_MESSUNG();
	IR_BALL_MESSUNG();
	
	//=================Sensoren Auswerten=================
	
	RelativWinkel=WINKEL-ANGLE_START;
	if(RelativWinkel>180)
	{
	  RelativWinkel=RelativWinkel-360;
	}
	else
	{
	  if(RelativWinkel<-180) RelativWinkel=RelativWinkel+360;
	}
	
	if((IR_DIRECTION==1))
	{
	 //Ball hinten links(5° bis 85°)
	}
	
	if(IR_DIRECTION==2)
	{
	 //Ball seitlich links (85° bis 100°)
	}
	
	if(IR_DIRECTION==3)
	{
	 //Ball schräg links vorne(100° bis verlängerung balleinbuchtung links)
	}
	
	if(IR_DIRECTION==4)
	{
	 //Ball leicht links vorne(Verlängerung Balleinbuchtung links bis + 10°)
	}
	
	if(IR_DIRECTION==5)
	{
	 //Ball direkt vorne
	 //>90 = 20 cm - am roboter
	 //<20 = sehr weit entfernt, fast nicht mehr sichtbar
	 //20-50 = noch immer weit vorne
	 //50-70 = mittelweit 30-40 cm
	 //50-90 = 20-30 cm
	}
	
	if(IR_DIRECTION==6)
	{
	 //leicht rechts vorne
	}
	
	if(IR_DIRECTION==7)
	{
	 //ball rechts, verlängerung einbuchtung rechts bis ca 90°
	}
	
	if(IR_DIRECTION==8)
	{
	 //Ball rechts seitlich ca 90°
	}
	
	if(IR_DIRECTION==9)
	{
	 //Ball rechts hinten ca 90° bis 25°
	}
	
	//Hinten nur scheisse, auf 8, 7 oder 0
	
	
	/*if((IR_DIRECTION==6)&&(IR_S3<=35)&&(IR_S4<=35))
	{
	 //ca 25° rechts hinten
	}
	
	if((IR_DIRECTION==0)&&(IR_S1>50)&&(IR_S2>50)&&(IR_S3>50)&&(IR_S4>50)&&(IR_S5>50))
	{
	 //Ball hinten
	}
	
	if((IR_DIRECTION==7)&&(IR_S4<35))
	{
	 //Ball hinten , ganz wenig rechts(10° bis 20°)
	}
	
	if((IR_DIRECTION==8)&&(IR_S4<35)&&(IR_S5<35))
	{
	 //Ball hinten, schräg rechts(45°bis 20°)
	}*/
	
	
	
	
  

    // ==================== Ereignisse ===================

    /*if (TASTER4()==1)
	
	{
	
	  STATUS = 0;							
	  
	}*/
	
	if (TASTER2()==1)
	
	{
	 
	  if (STATUS<MAXSTAT)
	  
	  {
	  
	    STATUS++;
		
	  }
	 
	}
	
	
	if (TASTER1()==1)
	
	{
	  if (STATUS>0)
	  
	  {
	  
	    STATUS--;
		
	  }
	}

   if (TASTER3()==1)
   {
      //SCHUSS();
   }
   
   if((TASTER4()==1))
  {
   Exit=1;
  }
	
	// ==================== Reaktionen ====================
	
	if (STATUS==0)							// ALLES AUS
	{
	 
	 MotorLinksSpeed=0;
	 MotorRechtsSpeed=0;
	 MotorHintenSpeed=0;
	}
	

	if (STATUS==1)							
	{
	 if(IR_DIRECTION==0&&stat!=2)
     {
		stat=1;
	 }
	 if(IR_DIRECTION==0&&stat==2)
	 {
		stat=0;
	 }
	 if(stat!=7)
	 {
		 if(IR_DIRECTION==4)//Ball leicht links
		 {
		  if(BALL_ENTFERNUNG>24)
		  {
			stat=3;
		  }
		  else
		  {
			stat=6;
		  }
		 }
		 
		 if(IR_DIRECTION==6)
		 {
		  if(BALL_ENTFERNUNG>30)
		  {
			stat=4;
		  }
		  else
		  {
			stat=5;
		  }
		 }
		 if(IR_DIRECTION==5) //Ball vorne
		 {
		  if(BALL_ENTFERNUNG>22)
		  {
			stat=2;
		  }
		  else
		  {
			stat=1;
		  } //GEMMA!!!
		 }
	 }
	 if(IR_DIRECTION==1) //Ball links hinten
	 {
	  stat=5;
	 }
	 
	 if(IR_DIRECTION==9)
	 {
	  stat=6;
	 }
	 
	 if(IR_DIRECTION==2)
	 {
	  stat=5;
	 }
	 
	 if(IR_DIRECTION==8)
	 {
	  stat=6;
	 }
	 
	 if(IR_DIRECTION==3)
	 {
	  if(BALL_ENTFERNUNG>30)
	  {
		stat=3;
	  }
	  else
	  {
		stat=1;
	  }
	 }
	 
	 if(IR_DIRECTION==7)
	 {
	  if(BALL_ENTFERNUNG>30)
	  {
		stat=4;
	  }
	  else
	  {
		stat=1;
	  }
	 }
	 
	 //if(stat==2&&BALL_ENTFERNUNG>140) stat=7;
	 if((stat==5||stat==6)&&BALL_ENTFERNUNG>120) stat=1;
	 if(stat==0)
	 {
		//stop
		MotorLinksSpeed = 0;
		MotorRechtsSpeed = 0;
		MotorHintenSpeed = 0;
	 }
	 if(stat==1)
	 {
		MotorLinksSpeed = 165;
		MotorRechtsSpeed = -155;
		MotorHintenSpeed = 0;
		//zurück
	 }
	 if(stat==2)
	 {
		MotorLinksSpeed = -165;
		MotorRechtsSpeed = 165;
		MotorHintenSpeed = 0;
		//vor
	 }
	 if(stat==3)
	 {
		MotorLinksSpeed = 0;
		MotorRechtsSpeed = 165;
		MotorHintenSpeed = -165;
		
		
		//vornelinks
	 }
	 if(stat==4)
	 {
		MotorLinksSpeed = -165;
		MotorRechtsSpeed = 0;
		MotorHintenSpeed = 165;
		
		
		
		//vornerechts
	 }
	 if(stat==5)
	 {
		MotorLinksSpeed = 165;
		MotorRechtsSpeed = 0;
		MotorHintenSpeed = -165;
		/*
		MotorLinksSpeed = 190;
		MotorRechtsSpeed = -140;
		MotorHintenSpeed = -170;*/
		//zurücklinks
	 }
	 if(stat==6)
	 {
		MotorLinksSpeed = 0;
		MotorRechtsSpeed = -165;
		MotorHintenSpeed = 165;
		/*
		MotorLinksSpeed = -140;
		MotorRechtsSpeed = 190;
		MotorHintenSpeed = 170;*/
		//zurückrechts
	 }
	/* if(stat==7)
	 {
		//ballapproach
		MotorLinksSpeed = -150;
		MotorRechtsSpeed = 150;
		MotorHintenSpeed = 0;
		if(IR_DIRECTION==4)//Ball leicht links
		 {
			MotorLinksSpeed = 150;
			MotorRechtsSpeed = 150;
			MotorHintenSpeed = 150;
		 }
		 
		 if(IR_DIRECTION==6)
		 {
			MotorLinksSpeed = -150;
			MotorRechtsSpeed = -150;
			MotorHintenSpeed = -150;
		 }
		if(BALL_ENTFERNUNG<130)
		{
			SCHUSS();
			stat=1;
		}
	 }*/
    //Winkelkorrektur
		if(smoothturn!=1)
		{
			if((RelativWinkel>WINKEL_KORREKTUR*(-1))||(RelativWinkel<WINKEL_KORREKTUR))
			{
				if(smoothturn==2)
				{
					KorrekturMotoren=RelativWinkel;
				}
				if(smoothturn==0)
				{
					KorrekturMotoren=-KorrekturMotoren*7;
					smoothturn=1;
				}
			}
			else
			{
				if((RelativWinkel>WINKEL_KORREKTUR*(-1)-30)||(RelativWinkel<WINKEL_KORREKTUR+30))
				{
					if(smoothturn==2)
					{
						KorrekturMotoren=RelativWinkel;
					}
					if(smoothturn==0)
					{
						KorrekturMotoren=-KorrekturMotoren*2;
						smoothturn=1;
					}
				}
				else
				{
					KorrekturMotoren=RelativWinkel;
					if((RelativWinkel<WINKEL_KORREKTUR*(-1)-40)||(RelativWinkel>WINKEL_KORREKTUR+40)) smoothturn=0;
				}
			}
		}
		else
		{
			if(oldwinkel==WINKEL)
			{
				smoothturn=2;
				KorrekturMotoren=-KorrekturMotoren;
			}
		}
		
		//KorrekturMotoren=RelativWinkel/2;
		if(0)
		{
			if(RelativWinkel<60&&RelativWinkel>-60)
			{
				KorrekturMotoren=RelativWinkel;
			}
			else
			{
				KorrekturMotoren=46+RelativWinkel/4;
			}
		}
	}
	

	if (STATUS==2)							
	{
		MotorLinksSpeed = 0;
		MotorRechtsSpeed = 0;
		MotorHintenSpeed = 0;
		if(smoothturn!=1)
		{
			if((RelativWinkel>WINKEL_KORREKTUR*(-1))||(RelativWinkel<WINKEL_KORREKTUR))
			{
				if(smoothturn==2)
				{
					KorrekturMotoren=RelativWinkel;
				}
				if(smoothturn==0)
				{
					KorrekturMotoren=-KorrekturMotoren*7;
					smoothturn=1;
				}
			}
			else
			{
				if((RelativWinkel>WINKEL_KORREKTUR*(-1)-30)||(RelativWinkel<WINKEL_KORREKTUR+30))
				{
					if(smoothturn==2)
					{
						KorrekturMotoren=RelativWinkel;
					}
					if(smoothturn==0)
					{
						KorrekturMotoren=-KorrekturMotoren;
						smoothturn=1;
					}
				}
				else
				{
					KorrekturMotoren=RelativWinkel;
					if((RelativWinkel<WINKEL_KORREKTUR*(-1)-40)||(RelativWinkel>WINKEL_KORREKTUR+40)) smoothturn=0;
				}
			}
		}
		else
		{
			if(oldwinkel==WINKEL)
			{
				smoothturn=2;
				KorrekturMotoren=0;
			}
		}
		/*
		 if(RelativWinkel<WINKEL_KORREKTUR*(-1))
		 {
		  KorrekturMotoren=-160;
		 
		 if(RelativWinkel>WINKEL_KORREKTUR*(-1)-10)
		 {
		  KorrekturMotoren=-110;
		 }
		 }
		 
		 if(RelativWinkel>WINKEL_KORREKTUR)
		 {
		  KorrekturMotoren=160;
		 
		 if(RelativWinkel<WINKEL_KORREKTUR+10)
		 {
		  KorrekturMotoren=110;
		 }
		 }
		 if((RelativWinkel<WINKEL_KORREKTUR-10)&&(RelativWinkel>WINKEL_KORREKTUR*(-1)+10))
		 {
		  KorrekturMotoren=0;
		 }
		 */
	}	
	strcpy(LINE_1,"stat:  STATUS:  ");
	strcpy(LINE_2,"                ");
	sprintf(Zahl,"%3d",stat);
	for (uint8_t i=0; i<=1; i++) LINE_1[i+5]=Zahl[i];
	sprintf(Zahl,"%3d",STATUS);
	for (uint8_t i=0; i<=1; i++) LINE_1[i+14]=Zahl[i];
	sprintf(Zahl,"%3d",IR_DIRECTION);
	for (uint8_t i=0; i<=3; i++) LINE_2[i]=Zahl[i];
	sprintf(Zahl,"%3d",BALL_ENTFERNUNG);
	for (uint8_t i=0; i<=2; i++) LINE_2[i+5]=Zahl[i];
	sprintf(Zahl,"%3d",twinkel);
	for (uint8_t i=0; i<=2; i++) LINE_2[i+8]=Zahl[i];
	
	// ===================== Steuerung ====================
	
	MOTOR_LINKS(MotorLinksSpeed+KorrekturMotoren);
	MOTOR_RECHTS(MotorRechtsSpeed+KorrekturMotoren);
	MOTOR_HINTEN(MotorHintenSpeed+KorrekturMotoren);
	
	SHOW_LINES();
    
  }
  
  
  
	MOTOR_LINKS(0);
	MOTOR_RECHTS(0);
	MOTOR_HINTEN(0);
 
  return 0;

}
