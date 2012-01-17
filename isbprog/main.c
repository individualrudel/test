#include <avr/io.h>
#include <string.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "i2cmaster.h"
#include <util/delay.h>

#include "main.h"
#include "soccerbot_v5.h"
#include "eepr.h"


#define MAXSTAT 2
#define LICHTSCHRANKE 5
#define WINKEL_KORREKTUR 15






uint8_t Run_main(void)
{

  // Globale Variablen
  
  uint8_t Exit=0;			//Wenn 1 ==> Beenden
  
  uint8_t STATUS;			//Großer Status, mit Taster umschalten
  
  uint8_t stat;				//Status für Programm
  /*
  0=stop
  1=zurück
  2=vor
  
  3=vornelinks
  4=vornerechts
  
  5=zurücklinks
  6=zurückechts
  
  25=rechts(gammlig)
  26=links(gammlig)
  
  7=habeball
  
  8=binlinks drehrechts und baller
  9=bin rechts dreh links und baller
  
  10=bin links steh an und fahr zum tor
  11=bin rechts steh an und fahr zum tor
  
  20=ball is geballert und zurücksetzn
  */
  
  int8_t Spez_Feld_Pos;		//Position am Feld
  
  int16_t MotorLinksSpeed;	//Speed für Motor Links
  
  int16_t MotorRechtsSpeed;	//Speed für Motor Rechts
  
  int16_t MotorHintenSpeed;	//Speed für Motor Hinten
  
  int16_t KorrekturMotoren;	//Richtung und Stärke der Motorkorrektur
  int16_t RelativWinkel;	//Winkel relativ zu Startwinkel
  uint8_t smoothturn;
  int16_t oldwinkel;
  int16_t oldwinkel2;
  uint8_t twinkel;	//Timer für oldwinkel aktualisierung
  int8_t Iforyou;	//Timer für Ballverlust
  char Zahl[4];		//Puffer für Displayausgabe
  
  
  // Initialisierung
  SOCCERBOT_INIT();
  
  STATUS = 0;
  
  stat=0;
  
  Spez_Feld_Pos=0;
  /*
  0=???
  1=??? Links
  2=??? Rechts
  
  4=Vorne Mitte
  5=Links Vorne
  6=Rechts Vorne
  
  9=Hinten Links
  10=Hinten Rechts
  */
  
  BALL_ENTFERNUNG=0;	//klein==nahe  groß==weit 255=nicht da
  
  MotorLinksSpeed = 0;

  MotorRechtsSpeed = 0;

  MotorHintenSpeed = 0;
  
  KorrekturMotoren = 0;
  
  RelativWinkel = 0;
  smoothturn=2;
  twinkel=0;
  oldwinkel=0;
  oldwinkel2=0;
  Iforyou=0;
  
  strcpy(LINE_1,"Individual Rudel");
  if(Robot()==KURT)
  {
   strcpy(LINE_2,"Soccer A -Kurt  ");
  }
  else if(Robot()==DETLEF)
  {
   strcpy(LINE_2,"Soccer A -Detlef");
  }
  else
  {
   strcpy(LINE_2,"Soccer A -EEFail");
  }
  

  
  ANGLE_START=WINKEL;
  while(Exit==0)
  
  {
	

    // ========== Automatische Sensorauslesung ==========
	
	KOMPASS_MESSUNG();
	IR_MESSUNG();
	ENTFERNUNGSMESSUNG();
	//IR_BALL_MESSUNG();	==	Im Moment Sinnlos/Verbraucht nur Zeit, Da noch keine Sensoren
	
	//=================Sensoren Auswerten=================
	
	//Winkel
	RelativWinkel=WINKEL-ANGLE_START;	//Relativer Winkel zum Startwinkel
	
	
	if(RelativWinkel>180)
	{
	  RelativWinkel=RelativWinkel-360;		//Korrektur des Relativwinkels wenn größer||kleiner 180  ==> Roboter hat sich über hinten gedreht
	}
	else
	{
	  if(RelativWinkel<-180) RelativWinkel=RelativWinkel+360;
	}
	
	if(twinkel==8)				//Relativwinkel zum Vergleich speichern(wenn timer-winkel über 10)	
	{
		oldwinkel=oldwinkel2;
		twinkel=0;
	}
	if(twinkel==4)				//Relativwinkel zum Vergleich speichern(wenn timer-winkel über 10)	
	{
		oldwinkel2=RelativWinkel;
	}
	twinkel++;

	
	
	
	//Position am Feld
	if(ENTFERNUNGEN[VORNE]>120&&ENTFERNUNGEN[HINTEN]<80)		//Vorne näher als ca 60 cm && Hinten weiter als ca 60 cm !!weil Sensor hin!!	== 	Roboter vorne
	 {
		Spez_Feld_Pos=4;	//==Vorne
		if(ENTFERNUNGEN[LINKS]>190&&ENTFERNUNGEN[RECHTS]<90) 	//Links näher als ca 30-40 cm && Rechts weiter als ca 1m	==	Links Vorne
		{
			Spez_Feld_Pos=5;	//Links vorne
		}
		else	//Wenn keins von beiden
		{	
			if(ENTFERNUNGEN[RECHTS]>190&&ENTFERNUNGEN[LINKS]<90) Spez_Feld_Pos=6;		//Rechts näher als ca 30-40 cm && Links weiter als ca 1m	==	Rechts Vorne
		}
	 }
	 
	 
	 if(ENTFERNUNGEN[HINTEN]>110&&ENTFERNUNGEN[VORNE]<60)		//Hinten näher als ca 30 cm && Vorne weiter als ca 80-90 cm	==	Roboter Hinten
	 {
		if(ENTFERNUNGEN[LINKS]>ENTFERNUNGEN[RECHTS])	//Wenn Links näher als Rechts 
		{
			Spez_Feld_Pos=9;	//Hinten Links
		}
		else	//Wenn Rechts näher als Links
		{
			Spez_Feld_Pos=10;	//Hinten Rechts
		}
	 }
	 
	 
	 if(ENTFERNUNGEN[HINTEN]<110&&ENTFERNUNGEN[VORNE]<120)		//Hinten weiter als ca 30 cm !!Sensor hin!!  && Vorne weiter als 50-60 cm		==		???Mitte???
	 {
		Spez_Feld_Pos=0;	//???
		
		if(ENTFERNUNGEN[LINKS]>190&&ENTFERNUNGEN[RECHTS]<80)	//Links näher als ca 30-40 cm && Rechts weiter als ca 1m	==	Links
		{
			Spez_Feld_Pos=1;	//???
		}
		else
		{
			if(ENTFERNUNGEN[RECHTS]>190&&ENTFERNUNGEN[LINKS]<90) Spez_Feld_Pos=2;		//Rechts näher als ca 30-40 cm && Links weiter als ca 1m 	==	Rechts
		}
	 }
	 
	 
	/*
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
	
	
	if((IR_DIRECTION==6)&&(IR_S3<=35)&&(IR_S4<=35))
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
	
	// wo is da Ball?
	if (STATUS==1)							
	{
	
	 if(IR_DIRECTION==0)		//Wenn Ball nicht gefunden
     {
		if(stat==7)		//Wenn vorher ball gehabt
		{
			SCHUSS();	//Amal nachballern
			stat=1;		//zurück
		}
		else
		{
			stat=1;		//zurück
		}
	 }
	 
	 if(BALL_ENTFERNUNG!=255)	//Wenn Ball gefunden
	 {
		 if(stat<7)	//Roboter hat ball
		 {
			 if(IR_DIRECTION==4)		//Ball leicht links
			 {
			 
			  if(BALL_ENTFERNUNG>25)	//Toleranz (wenn drunter wahrscheinlich irgendwo in der Pampa hinten)
			  {
				if(BALL_ENTFERNUNG>140)	//Ball nahe
				{
					stat=5;		//Links zruck
				}
				else
				{
					stat=3;		//Links vor
				}
			  }
			  else		//Wenn Scheisswert
			  {
				//stat=6;			//Rechts zruck
				stat=1;				//zruck
			  }
			  
			 }
			 
			 
			 if(IR_DIRECTION==6)		//Ball leicht rechts
			 {
			  if(BALL_ENTFERNUNG>25)	//Toleranz (wenn drunter wahrscheinlich irgendwo in der Pampa hinten)
			  {
				if(BALL_ENTFERNUNG>140)	//Ball nahe
				{
					stat=6;		//Rechts zruck
				}
				else
				{
					stat=4;		//Rechts vor
				}
			  }
			  else		//Wenn Scheisswert
			  {
				stat=5;			//Links zruck
			  }
			 }
			 
			 
			 if(IR_DIRECTION==5) //Ball vorne
			 {
			  if(BALL_ENTFERNUNG>25)	//Toleranz (wenn drunter wahrscheinlich irgendwo in der Pampa hinten)
			  {
				stat=2;  //GEMMA!! Ball vorne  == Vorfahren
			  }
			  else	//Wenn Scheisswert
			  {
				stat=1;		//zurück
			  }
			 }
			 
			 
		 }
		 
		 
		 
		 if(IR_DIRECTION==1) //Ball links hinten
		 {
		  if(BALL_ENTFERNUNG>120)	//Wenn Ball nahe
			{
				stat=1;		//zurück
			}
			else	//Wenn weiter weg
			{
				stat=5;		//Links zruck
			}
		 }
		 
		 
		 if(IR_DIRECTION==9) //Ball rechts hinten
		 {
		  if(BALL_ENTFERNUNG>120)	//Wenn Ball nahe
			{
				stat=1;		//zurück
			}
			else	//Wenn weiter weg
			{
				stat=6;		//Rechts zruck
			}
		 }
		 
		 
		 if(IR_DIRECTION==2) //Ball links seitlich
		 {
		  if(BALL_ENTFERNUNG>120)	//Wenn Ball nahe
			{
				stat=1;		//zurück
			}
			else	//Wenn weiter weg
			{
				stat=5;		//Links zruck
			}
		 }
		 
		 
		 if(IR_DIRECTION==8) //Ball rechts hinten 
		 {
		  if(BALL_ENTFERNUNG>120)	//Wenn Ball nahe
			{
				stat=1;		//zurück
			}
			else	//Wenn weiter weg
			{
				stat=6;		//Rechts zruck
			}
		 }
		 
		 
		 if(IR_DIRECTION==3) //Ball links vorne
		 {
		  if(BALL_ENTFERNUNG>30)	//Toleranz
		  {
			//stat=3;			//Vorne links				!!!Links Zruck probieren(stat=5)!!!
			stat=26;		//Links
			//stat=5;		//Links Zruck
		  }
		  else	//Wenn Scheisswert
		  {
			stat=1;		//zurück
		  }
		 }
		 
		 
		 if(IR_DIRECTION==7) //Ball rechts vorne
		 {
		  if(BALL_ENTFERNUNG>30)	//Toleranz
		  {
			//stat=4;			//Vorne rechts				!!!Rechts Zruck probieren(stat=6)!!!
			stat=25; 		//Rechts
			//stat=6;		//Rechts zruck
		  }
		  else	//Wenn Scheisswert
		  {
			stat=1;		//zurück
		  }
		 }
		 
	 }
	 else	//Ball nicht gefunden
	 {
		stat=1;			//zurück
		
	/*	if(Spez_Feld_Pos==9)	//Wenn Ball nicht gefunden und Hinten Links
		{
		 stat=25;	//Rechts Fahren
		}
		
		if(Spez_Feld_Pos==10)	//Wenn Ball nicht gefunden und Hinten Rechts
		{
		 stat=26;	//Links Fahren
		}*/
		
		
	/*	if(Spez_Feld_Pos==9&&ENTFERNUNGEN[HINTEN]>185)	//Wenn Ball nicht gefunden und Hinten Links an der Wand							!!!AUSPROBIEREN!!!
		{
		 stat=4;	//Rechts vor Fahren
		}
		
		if(Spez_Feld_Pos==10&&ENTFERNUNGEN[HINTEN]>185)//Wenn Ball nicht gefunden und Hinten Rechts an der Wand						!!!AUSPROBIEREN!!!
		{
		 stat=3;	//Links vor Fahren
		}*/
		
	 }
	 
	 
	 
	 //Ball bam Roboter
	 if(ADCMESSUNG(LICHTSCHRANKE)>300&&(stat==2||stat==3||stat==4))	//Wenn was in der Lichtschranke und Roboter fährt grad nach vorne (gradeaus, links oder Rechts)
	 {
		stat=7;		//Roboter hat Ball
		Iforyou=0;	//Ball Timer == 0
	 }
	 
	 
	 if((Spez_Feld_Pos==9||Spez_Feld_Pos==1)&&(stat==1||stat==5))		//Wenn Roboter grad beim zrückfahren oder Links zruckfahren und Roboter Hinten Links oder in der Mitte Links
	 {
		stat=6;		//Zurückrechts fahren stattdessen == net zur Wand
	 }
	 
	 
	 if((Spez_Feld_Pos==10||Spez_Feld_Pos==2)&&(stat==1||stat==6))	//Wenn Roboter grad beim zrückfahren oder Rechts zruckfahren und Roboter Hinten Rechts oder in der Mitte Rechts
	 {
 		stat=5;		//Zurücklinks fahren stattdessen == net zur Wand
	 }
	 
	 
	 if(stat==7)	//Wenn in Ballbesitz und Vorne is was					
	 {																								
		if(ENTFERNUNGEN[VORNE]>500)
		{
			if(Spez_Feld_Pos==4)	//Wenn Vorne in da Mitte										
			{																						
				SCHUSS();	//Draufballern															
				stat=20;	//Zruck																	
			}																						
			if(Spez_Feld_Pos==5)	//Wenn Vorne Links												
			{																						
				stat=10;	//bin links steh an und fahr zum tor									
			}																						
			if(Spez_Feld_Pos==6)	//Wenn Vorne Rechts												
			{																						
				stat=11;	//bin rechts steh an und fahr zum tor									
			}
		}																					
		else
		{																							
			if(Spez_Feld_Pos==4||Spez_Feld_Pos==0)	//Wenn vorne in da Mitte oder In der Mitte		
			{																						
				SCHUSS();	//Draufballern															
				stat=20;	//ball is geballert und zurücksetzn										
			}																						
			if(Spez_Feld_Pos==5)	//Wenn Vorne Links												
			{																						
				stat=8;		//bin links dreh rechts und baller										
			}																						
			if(Spez_Feld_Pos==6)	//Wenn vorne Rechts												
			{																						
				stat=9;		//bin rechts dreh links und baller										
			}																						
		}																							
	 }																								
	 if(stat==1&&ENTFERNUNGEN[HINTEN]>400) stat=0;
	 
	 
	 //---------------------------------------------------------------------------
	 //jetzt wiad gehandlt
	 if(stat==0)
	 {
		//stop
		MotorLinksSpeed = 0;
		MotorRechtsSpeed = 0;
		MotorHintenSpeed = 0;
	 }
	 
	 if(stat==1)
	 {
		MotorLinksSpeed = Topspeed;
		MotorRechtsSpeed = -Topspeed;
		MotorHintenSpeed = 0;
		//zurück
	 }
	 
	 if(stat==2)
	 {
		MotorLinksSpeed = -Topspeed;
		MotorRechtsSpeed = Topspeed;
		MotorHintenSpeed = 0;
		//vor
	 }
	 
	 if(stat==3)
	 {
		MotorLinksSpeed = 0;
		MotorRechtsSpeed = Topspeed;
		MotorHintenSpeed = -Topspeed;
		//vornelinks
	 }
	 
	 if(stat==4)
	 {
		MotorLinksSpeed = -Topspeed;
		MotorRechtsSpeed = 0;
		MotorHintenSpeed = Topspeed;
		//vornerechts
	 }
	 
	 if(stat==5)
	 {
		MotorLinksSpeed = Topspeed;
		MotorRechtsSpeed = 0;
		MotorHintenSpeed = -Topspeed;
		//zurücklinks
	 }
	 
	 if(stat==6)
	 {
		MotorLinksSpeed = 0;
		MotorRechtsSpeed = -Topspeed;
		MotorHintenSpeed = Topspeed;
		//zurückrechts
	 }
	 
	 if(stat==25)
	 {
		//Rechts fahren(ziemlich gammlig)== -20 -20 250
		MotorLinksSpeed = -160;
		MotorRechtsSpeed = -170;
		MotorHintenSpeed = 210;
		//Rechts
	 }
	 
	 if(stat==26)
	 {
		//Links fahren(ziemlich gammlig)== 20 20 -250
		MotorLinksSpeed = 160;
		MotorRechtsSpeed = 170;
		MotorHintenSpeed = -210;
		//Links
	 }
	 
	 if(stat==7)
	 {
		//habeball == vorwärts
		MotorLinksSpeed = -Topspeed;
		MotorRechtsSpeed = Topspeed;
		MotorHintenSpeed = 0;
		
		if(ADCMESSUNG(LICHTSCHRANKE)<300)	//Wenn nix in der Lichtschranke ==> Vorher ball gehabt, jetzt nicht mehr
		{
			Iforyou++;	//Timer um 1 erhöhen
		}
		else Iforyou=0;	//Wenn was in der Lichtschranke(Ball), dann Timer zurücksetzen
		
		if(Iforyou>10)	//Wenn Ballverlusttimer > 20 ==> Ball Verloren
		{
			SCHUSS();	//Amal nachballern
	//		stat=1;
			Iforyou=0;	//Timer 0 setzen
		}
	 }
	 
	 if(stat==8)
	 {
		//binlinks drehrechts und baller
		if(RelativWinkel>45)				//Wenn mehr als 45° nach Rechts gedreht
		{
			MotorLinksSpeed = -Topspeed;	//Vorwärts fahren
			MotorRechtsSpeed = Topspeed;
			MotorHintenSpeed = 0;
			
			if(Spez_Feld_Pos==5&&ENTFERNUNGEN[LINKS]>300)	//Wenn Roboter vorne Links und Links an der Wand(oder anderer Roboter)
			{
				SCHUSS();	//Ballern
				stat=20;	//Zurück	???NOTWENDIG???
			}
		}
		else								//Wenn weniger gedreht
		{
			MotorLinksSpeed = -(Topspeed-25);	//Nach Rechts drehen
			MotorRechtsSpeed = 0;
			MotorHintenSpeed = -Topspeed;
		}
		if(Spez_Feld_Pos==0) stat=1;	//Wenn in der Mitte ==> zurück
	 }
	 
	 if(stat==9)
	 {
		//bin rechts dreh links und baller
		if(RelativWinkel<-45)				//Wenn mehr als 45° nach Links gedreht
		{
			MotorLinksSpeed = -165;	//Vorwärts fahren
			MotorRechtsSpeed = 165;
			MotorHintenSpeed = 0;
			if(Spez_Feld_Pos==6&&ENTFERNUNGEN[RECHTS]>300)	//Wenn Roboter vorne Rechts und Rechts an der Wand(oder anderer Roboter)
			{
				SCHUSS();	//Ballern
				stat=20;	//Zurück	???NOTWENDIG???
			}
		}
		else								//Wenn weniger gedreht
		{
			MotorLinksSpeed = 0;	//Nach Links drehen
			MotorRechtsSpeed = 155;
			MotorHintenSpeed = 175;
		}
		if(Spez_Feld_Pos==0) stat=1;	//Wenn in der Mitte ==> zurück
	 }
	 
	 
	 
	 if(stat==10)
	 {
		//bin links steh an und fahr zum tor
		if(RelativWinkel>45)				//Wenn mehr als 45° nach Rechts gedreht
		{
			MotorLinksSpeed = -165;	//Vorwärts fahren
			MotorRechtsSpeed = 165;
			MotorHintenSpeed = 0;
			if(Spez_Feld_Pos==5&&ENTFERNUNGEN[LINKS]<350)	//Wenn Roboter vorne Links und Links nicht ganz so nahe(Tor)
			{
				SCHUSS();	//Ballern
				stat=20;	//Zurück	???NOTWENDIG???
			}
		}
		else								//Wenn weniger gedreht
		{
			MotorLinksSpeed = -155;	//Nach Rechts drehen
			MotorRechtsSpeed = 0;
			MotorHintenSpeed = -175;
		}
		if(Spez_Feld_Pos==0) stat=1;		//Wenn in der Mitte ==> zurück
	 }
	 
	 if(stat==11)
	 {
		//bin rechts steh an und fahr zum tor
		if(RelativWinkel<-45)					//Wenn Roboter stärker als 45° nach Links gedreht
		{
			MotorLinksSpeed = -165;	//Vorwärts fahren
			MotorRechtsSpeed = 165;
			MotorHintenSpeed = 0;
			
			if(Spez_Feld_Pos==6&&ENTFERNUNGEN[RECHTS]<350)	//Wenn Roboter vorne Rechts und Rechts nicht ganz so nahe(Tor)
			{
				SCHUSS();	//Ballern
				stat=20;	//Zurück	???NOTWENDIG???
			}
		}
		else								//Wenn weniger gedreht
		{
			MotorLinksSpeed = 0;	//Nach Links Drehen
			MotorRechtsSpeed = 155;
			MotorHintenSpeed = 175;
		}
		if(Spez_Feld_Pos==0) stat=1;		//Wenn in der Mitte ==> zurück
	 }
	 
	 if(stat==20)
	 {
		//ball is geballert und zurücksetzn
		MotorLinksSpeed = 165;
		MotorRechtsSpeed = -165;
		MotorHintenSpeed = 0;
		if(Spez_Feld_Pos<3) stat=1;		//Wenn Roboter in der Mitte, dann schneller zurückfahren
	 }
	 
	 
	 
	 //------------------------------------------------------------------------------------------------------------------------------------
     //Winkelkorrektur
		if(RelativWinkel>WINKEL_KORREKTUR)
		{
			KorrekturMotoren=185;
			if(RelativWinkel>90) KorrekturMotoren=220;
		}
		else
		{
			if(RelativWinkel<-WINKEL_KORREKTUR)
			{
				KorrekturMotoren=-185;
				if(RelativWinkel<90) KorrekturMotoren=-220;
			}
			else
			{
				KorrekturMotoren=0;
			}
		}
		if(0)//stat<8||stat>11)
		{
			switch (smoothturn)
			{
				case 2:
					KorrekturMotoren=RelativWinkel;
					if((RelativWinkel<-WINKEL_KORREKTUR)||(RelativWinkel>WINKEL_KORREKTUR)) smoothturn=0;
				break;
				case 0:
					if(RelativWinkel<10&&RelativWinkel>-10)
					{
						KorrekturMotoren = -KorrekturMotoren;
						if(RelativWinkel>0) smoothturn=1;
						else smoothturn=3;
					}
				break;
				case 1:
					if(oldwinkel<=oldwinkel2)
					{
						smoothturn=2;
						KorrekturMotoren=0;
					}
				break;
				case 3:
					if(oldwinkel>=oldwinkel2)
					{
						smoothturn=2;
						KorrekturMotoren=0;
					}
				break;
				default:
				break;
			}
		}
		//KorrekturMotoren=RelativWinkel/2;
		/*if(0)
		{
			if(RelativWinkel<60&&RelativWinkel>-60)
			{
				KorrekturMotoren=RelativWinkel;
			}
			else
			{
				KorrekturMotoren=46+RelativWinkel/4;
			}
		}*/
		
		
			//if((RelativWinkel<-WINKEL_KORREKTUR/2)||(RelativWinkel>WINKEL_KORREKTUR/2))  KorrekturMotoren=RelativWinkel/2;
			
	}
	

	if (STATUS==2)							
	{
		MotorLinksSpeed = 0;
		MotorRechtsSpeed = 0;
		MotorHintenSpeed = 0;
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
	LCD_CLEAR(0);
	/*for(uint8_t i=0; i<16; i++)
	{
		if(i==0) sprintf(Zahl,"%3d",stat);
		if(i<2)
		{
			LINE_1[i]=Zahl[i];
		}
		if(i==2) sprintf(Zahl,"%3d",RelativWinkel)
		if(i>2&&i<5) Line_1[i]=Zahl[(i-2)];
		
	}
	*/
	sprintf(Zahl,"%3d",stat);
	 for (uint8_t i=0; i<=3; i++) LINE_1[i]=Zahl[i];
	 
	 /*sprintf(Zahl,"%3d",STATUS);
	 for (uint8_t i=0; i<=3; i++) LINE_1[i+4]=Zahl[i];
	 
	 sprintf(Zahl,"%3d",RelativWinkel);
	 for (uint8_t i=0; i<=3; i++) LINE_1[i+8]=Zahl[i];
	 
	 sprintf(Zahl,"%3d",KorrekturMotoren);
	 for (uint8_t i=0; i<=3; i++) LINE_1[i+12]=Zahl[i];*/
	 
	 sprintf(Zahl,"%3d",Spez_Feld_Pos);
	 for (uint8_t i=0; i<=3; i++) LINE_2[i]=Zahl[i];
	 
	 /*sprintf(Zahl,"%3d",BALL_ENTFERNUNG);
	 for (uint8_t j=0; j<=3; j++) LINE_2[j+4]=Zahl[j];
	 
	 sprintf(Zahl,"%3d",twinkel);
	 for (uint8_t i=0; i<=3; i++) LINE_2[i+8]=Zahl[i];
	 
	 sprintf(Zahl,"%3d",Spez_Feld_Pos);
	 for (uint8_t i=0; i<=3; i++) LINE_2[i+12]=Zahl[i];*/
	 
	 /*strcpy(LINE_1,"Individual Rudel");
  if(Robot()==KURT)
  {
   strcpy(LINE_2,"Soccer A -Kurt  ");
  }
  else if(Robot()==DETLEF)
  {
   strcpy(LINE_2,"Soccer A -Detlef");
  }
  else
  {
   strcpy(LINE_2,"Soccer A -EEFail");
  }*/
		
	// ===================== Steuerung ====================
	if(RelativWinkel!=0&&(stat<8||stat>11))
	{
		if(MotorLinksSpeed==0)
		{
			MotorLinksSpeed=KorrekturMotoren;
		}
		else
		{
			if(MotorRechtsSpeed==0)
			{
				MotorRechtsSpeed=KorrekturMotoren;
			}
			else
			{
				if(MotorHintenSpeed==0)
				{
					MotorHintenSpeed=KorrekturMotoren;
				}
				if(0)
				{
					if(KorrekturMotoren<0)
					{
						MotorLinksSpeed=MotorLinksSpeed-50;
						MotorRechtsSpeed=MotorRechtsSpeed-50;
						MotorHintenSpeed=MotorHintenSpeed-50;
					}
					if(KorrekturMotoren>0)
					{
						MotorLinksSpeed=MotorLinksSpeed+50;
						MotorRechtsSpeed=MotorRechtsSpeed+50;
						MotorHintenSpeed=MotorHintenSpeed+50;
					}
				}
			}
		}
	}
	MOTOR_LINKS(MotorLinksSpeed);
	MOTOR_RECHTS(MotorRechtsSpeed);
	MOTOR_HINTEN(MotorHintenSpeed);
	SHOW_LINES();
  }
	MOTOR_LINKS(0);
	MOTOR_RECHTS(0);
	MOTOR_HINTEN(0);
  return 0;
}
