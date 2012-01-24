#include <avr/io.h>
#include <string.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "main.h"
#include "soccerbot_v4.h"


int Run_main(void)

{

  // Globale Variablen
  uint8_t Exit=0;
  
  uint8_t Status;
  
  int16_t MotorLinksSpeed;
  
  int16_t MotorRechtsSpeed;
  
  int16_t MotorHintenSpeed;
  
  
  
  
  // Initialisierung
  
 // i2c_init();
  
 // SOCCERBOT_INIT();
  
  Status = 0;
  
  MotorLinksSpeed = 0;

  MotorRechtsSpeed = 0;

  MotorHintenSpeed = 0;
  

  while(Exit==0)
  
  {
  
    // ========== Automatische Sensorauswertung =========
	
	
	KOMPASS_MESSUNG();
	IR_MESSUNG();
  

    // ==================== Ereignisse ===================

   /* if (TASTER4()==1)
	
	{
	
	  Status = 0;							
	  
	}*/
	
	if (TASTER2()==1)
	
	{
	
	  if (Status<8)
	  
	  {
	  
	    Status++;
		
	  }
	
	}
	
	
	if (TASTER1()==1)
	
	{
	
	  if (Status>0)
	  
	  {
	  
	    Status--;
		
	  }
	
	}

   if (TASTER3()==1)
   {
     SCHUSS();
  }
  
  //Exit from main program
  if((TASTER4()==1))
  {
   Exit=1;
  }
	
	// ==================== Reaktionen ====================
	
	if (Status==0)							// ALLES AUS
	{
	}
	

	if (Status==1)							// alle Motoren vorwärts
	{
	}
	

	if (Status==2)							// alle Motoren rückwärts
	{
	}
	
	
	if (Status==3)							// Kompass
	{
	}
	
	
	if (Status==4)							// IR Transistoren
	{
	}
	
	
	if (Status==5)							// Kompass
	{
	}
	
	
	if (Status==6)							// Taster Ball
	{
	}
	

	if (Status==7)							// IR Seeker
	{
	}
	
	
	if (Status==8)							// Entfernungen
	{
	}	
	
	// ===================== Steuerung ====================
	
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
