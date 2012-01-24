//Header Files inkludieren
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>



#include "menu.h"
#include "i2cmaster.h"
#include "soccerbot_v5.h"
#include "main.h"
#include "addons.h"





//Definitions





uint8_t mainMenuNo=MAINMENUMIN;/*1,2,3,4*/ 
uint8_t subMenuNo=SUBMENUMIN;/*1,2,3*/
uint8_t in_submenu=0;





int main(void)
{
  INTERRUPTS_OFF
 
 i2c_init();
 SOCCERBOT_INIT();
// INIT();
 
 SHOW_CURRENT_STATUS();
 
 INTERRUPTS_ON			  //Um ganz sicher zu sein
 
 while(1==1)
 {
  
   if(TASTER1()==1) //BUTTON LEFT PRESSED
{
  if(in_submenu==0)//Wenn nicht in Untermenu
  {
    if(mainMenuNo==MAINMENUMIN){mainMenuNo=MAINMENUMAX;}
    else{mainMenuNo--;}
    SHOW_CURRENT_STATUS();
  }
  else //Wenn in untermenu
  {
    if(subMenuNo==SUBMENUMIN){subMenuNo=SUBMENUMAX;}
    else{subMenuNo--;}
    SHOW_CURRENT_STATUS();
  }
}


if(TASTER2()==1) //BUTTON RIGHT PRESSED
{
  if(in_submenu==0)//Wenn nicht in Untermenu
  {
    if(mainMenuNo==MAINMENUMAX){mainMenuNo=MAINMENUMIN;}
    else{mainMenuNo++;}
    SHOW_CURRENT_STATUS();
  }
  else //Wenn in submenu
  {
    if(subMenuNo==SUBMENUMAX){subMenuNo=SUBMENUMIN;}
    else{subMenuNo++;}
    SHOW_CURRENT_STATUS();
  }
}


if(TASTER3()==1) //BUTTON ENTER PRESSED
{
  //Im Mainmenu
 if(mainMenuNo==0)
 {
  if(ANGLE_START>380)
  {
   Calibrate_Compass(GUI);
  }
  Run_main(); 
 }
 
 if(mainMenuNo==1)
 {
  //Calibrate Compass
  Calibrate_Compass(GUI);
 }
 
 
 
 
 //Im Submenu von 2
 
 //Submenu1
 if((mainMenuNo==2)&&(in_submenu==1)&&(subMenuNo==1))
 {
   //Programm1
   SUB1();
 }
 
 if((mainMenuNo==2)&&(in_submenu==1)&&(subMenuNo==2))
 {
   //Programm1
   SUB2();
 }
 
 //Submenu3
 if((mainMenuNo==2)&&(in_submenu==1)&&(subMenuNo==3))
 {
   //Programm3
   SUB3();
 }
 
 //Submenu4
 if((mainMenuNo==2)&&(in_submenu==1)&&(subMenuNo==4))
 {
   //Programm4
   SUB4();
 }
 
 //Submenu5
 if((mainMenuNo==2)&&(in_submenu==1)&&(subMenuNo==5))
 {
   SUB5();
 }
 
 //Submenu6
 if((mainMenuNo==2)&&(in_submenu==1)&&(subMenuNo==6))
 {
   SUB6();
 }
 
 //Submenu7
 if((mainMenuNo==2)&&(in_submenu==1)&&(subMenuNo==7))
 {
   SUB7();
 }
 
 
 
 if((mainMenuNo==2)&&(in_submenu==0))
 {
  in_submenu=1;	
 }
 
 SHOW_CURRENT_STATUS();
}



if(TASTER4()==1) //BUTTON BACK PRESSED
{
  if(in_submenu==0)//Wenn in Mainmenu
  {
    mainMenuNo=0;
	DRIBBLER(0);
	MOTOR_LINKS(0);
	MOTOR_RECHTS(0);
	MOTOR_HINTEN(0);
  }
  else
  {
   if(in_submenu==1)//Wenn in Submenu
   {
     in_submenu=0;
	 subMenuNo=1;
   }
  }
  SHOW_CURRENT_STATUS();
}
 
 //---------------------
 }
 
 return 0;
}



void SHOW_CURRENT_STATUS(void)
 {
   LCD_CLEAR();
   
   if(in_submenu==0)//Wenn in Mainmenu
   {
  if((mainMenuNo==0))
  {
   strcpy(LINE_1,"<<< Run Main >>>");
  }

   if((mainMenuNo==1))
  {
   strcpy(LINE_1,"Calibr. Compass ");
  }
  
  if((mainMenuNo==2))
  {
   strcpy(LINE_1,"Subprograms     ");
  }
   }
  
  
  if((in_submenu==1)) //Wenn in Submenu
  {
    if(subMenuNo==1)
    {
	 strcpy(LINE_1,NAME_SUB1);
    }
    
    if(subMenuNo==2)
    {
	 strcpy(LINE_1,NAME_SUB2);
    }
    
    if(subMenuNo==3)
    {
	 strcpy(LINE_1,NAME_SUB3);
    }
    
    if(subMenuNo==4)
    {
	 strcpy(LINE_1,NAME_SUB4);
    }
    
    if(subMenuNo==5)
    {
	 strcpy(LINE_1,NAME_SUB5);
    }
	
	if(subMenuNo==6)
    {
	 strcpy(LINE_1,NAME_SUB6);
    }
	
	if(subMenuNo==7)
    {
	 strcpy(LINE_1,NAME_SUB7);
    }
  }
  SHOW_LINES();
 }