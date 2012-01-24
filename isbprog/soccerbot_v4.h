/********************************************/
/*          SOCCERBOT V4   04.01.2010       */
/********************************************/


/*************************************/
/*          INITIALISIERUNG          */
/*************************************/

// PORTA ... I/O


// PORTB ... SPI, Solenoid, PWM Motoren
// PB0   ... 
// PB1   ... 
// PB2   ... 
// PB3   ... 
// PB4   ... OUTPUT Solenoid
// PB5   ... PWM Motor links
// PB6   ... PWM Motor rechts
// PB7   ... PWM Motor hinten

// PORTC ... I/O
// PC0   ... OUTPUT Motor hinten L
// PC1   ... OUTPUT Motor Dribbler R
// PC2   ... OUTPUT Motor hinten R
// PC3   ... OUTPUT Motor Dribbler L
// PC4   ... OUTPUT Motor rechts L
// PC5   ... OUTPUT Motor links R
// PC6   ... OUTPUT Motor rechts R
// PC7   ... OUTPUT Motor links L


// PORTD ... I2C, I/O, LCD
// PD0   ... I2C SCL
// PD1   ... I2C SDA
// PD2   ... TASTER LINKS
// PD3   ... TASTER MITTE LINKS
// PD4   ... TASTER MITTE RECHTS
// PD5   ... TASTER RECHTS
// PD6   ... TASTER BALL
// PD7   ... LCD EN


// PORTE ... I/O
// PE0   ... INPUT IR
// PE1   ... INPUT IR
// PE2   ... INPUT IR
// PE3   ... INPUT IR
// PE4   ... INPUT IR
// PE5   ... INPUT IR
// PE6   ... INPUT IR
// PE7   ... INPUT IR


// PORTF ... ADC
// PF0   ... ADC IR GP2D12 vorne
// PF1   ... ADC IR GP2D12 links
// PF2   ... ADC IR GP2D12 rechts
// PF3   ... ADC IR GP2D12 hinten
// PF4   ...
// PF5   ...
// PF6   ...
// PF7   ...

// PORTG ... LCD
// PG0   ... LCD D4
// PG1   ... LCD D5
// PG2   ... LCD D6
// PG3   ... LCD D7
// PG4   ... LCD RS

void SOCCERBOT_INIT(void);

/*****************************/
/*          KOMPASS          */
/*****************************/

uint16_t WINKEL;
uint16_t ANGLE_START;
void KOMPASS_MESSUNG(void);


/*****************************/
/*          MOTOREN          */
/*****************************/

void DRIBBLER(int8_t Wert);
void MOTOR_LINKS(int16_t Wert);
void MOTOR_RECHTS(int16_t Wert);
void MOTOR_HINTEN(int16_t Wert);
void FAHREN(int16_t Wert);
void ROTIEREN(int16_t Wert);
void SCHUSS(void);

 
/*****************************/
/*       IR Seeker V2        */
/*****************************/

uint8_t IR_DIRECTION, IR_S1, IR_S2, IR_S3, IR_S4, IR_S5;
void IR_MESSUNG(void);


/*****************************/
/*       IR Transistoren     */
/*****************************/

uint8_t IR_1, IR_2, IR_3, IR_4, IR_5, IR_6, IR_7, IR_8;
void IR_EMPFANG(void);



/****************************/
/*          TASTER          */
/****************************/
 
uint8_t TASTER1_GEDRUECKT;
uint8_t TASTER2_GEDRUECKT;
uint8_t TASTER3_GEDRUECKT;
uint8_t TASTER4_GEDRUECKT;

uint8_t TASTER_BALL(void);
uint8_t TASTER1(void);
uint8_t TASTER2(void);
uint8_t TASTER3(void);
uint8_t TASTER4(void);


/********************************/
/*          ENTFERNUNG          */
/********************************/
 
uint16_t ENTFERNUNGEN[4];

#define VORNE 0
#define LINKS 1
#define HINTEN 2
#define RECHTS 3
 
uint16_t ENTFERNUNG_VORNE(void);
uint16_t ENTFERNUNG_LINKS(void);
uint16_t ENTFERNUNG_HINTEN(void);
uint16_t ENTFERNUNG_RECHTS(void);


/*************************/
/*          LCD          */
/*************************/

char Zahl[4];


char lcdbuf[16+1];					// Buffer Display
char LINE_1[16+1];					// Zeile 1
char LINE_2[16+1];					// Zeile 2

/* pototype  section */
void init_lcd(void);				// LCD Display initialisieren (4-Bit Interface)
void gotolcd(unsigned char);		// Set Cursor LCD
void write_ins(unsigned char);     	// Write Instruction LCD
void write_data(unsigned char);		// Write Data LCD
void printlcd(void);				// Zeichen ausgeben

void SHOW_LINES(void);				// Zeilen anzeigen

void LCD_CLEAR(void);
