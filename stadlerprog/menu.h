#ifndef MENU_H
#define MENU_H


#define MAINMENUMAX 2
#define MAINMENUMIN 0
#define SUBMENUMAX 7
#define SUBMENUMIN 1

uint8_t mainMenuNo;/*1,2,3,4*/ 
uint8_t subMenuNo;/*1,2,3*/
uint8_t in_submenu;
uint8_t in_programm;





int main(void);

void SHOW_CURRENT_STATUS(void);


#endif