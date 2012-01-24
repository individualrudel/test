#include <avr/eeprom.h>
#include "eepr.h"


// Byte aus dem EEPROM lesen
uint8_t EEPReadByte(uint16_t addr)
{
  return eeprom_read_byte((uint8_t *)addr);
}
 
// Byte in das EEPROM schreiben
void EEPWriteByte(uint16_t addr, uint8_t val)
{
  eeprom_write_byte((uint8_t *)addr, val);
}



uint8_t Robot(void)
{
  return EEPReadByte(EE_ROBOT);
}

uint8_t WriteRobot(uint8_t robot)
{
 if((Robot()!=1)&&(Robot()!=2))
 {
  EEPWriteByte(EE_ROBOT, robot);
  return 1;
 }
 
  return 0;
}