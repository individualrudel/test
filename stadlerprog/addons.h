#ifndef ADDONS_H
#define ADDONS_H

#define NAME_SUB1 "1: IR Sensoren  "
#define NAME_SUB2 "2: Lego IR      "
#define NAME_SUB3 "3: Entfernungen "
#define NAME_SUB4 "4: Compass      "
#define NAME_SUB5 "5: Schuss       "
#define NAME_SUB6 "6: Dribbler     "
#define NAME_SUB7 "7: Fahren       "


void SUB1(void);
void SUB2(void);
void SUB3(void);
void SUB4(void);
void SUB5(void);
void SUB6(void);
void SUB7(void);


#define GUI 1
#define SILENT 0

void Calibrate_Compass(uint8_t gui);

uint16_t READ_IR(uint8_t sensor);

#endif