#ifndef EEPR_H
#define EEPR_H

#include <avr/eeprom.h>     // Die EEPROM-Definitionen/Macros der avr-libc einbinden

// EEMEM wird bei aktuellen Versionen der avr-lib in eeprom.h definiert
// hier: definiere falls noch nicht bekannt ("alte" avr-libc)
#ifndef EEMEM
// alle Textstellen EEMEM im Quellcode durch __attribute__ ... ersetzen
#define EEMEM  __attribute__ ((section (".eeprom")))
#endif


#define   EE_ROBOT  0x010  // Welcher Roboter?	==>	1=Kurt, 2=Detlef

#define KURT 1
#define DETLEF 2

uint8_t vorher;
uint8_t nacher;
uint8_t written;

uint8_t EEPReadByte(uint16_t addr);
void EEPWriteByte(uint16_t addr, uint8_t val);


uint8_t Robot(void);

uint8_t WriteRobot(uint8_t robot);


#endif