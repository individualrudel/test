/********************************************/
/*          SOCCERBOT V5   25.03.2011       */
/********************************************/


/*************************************/
/*          INITIALISIERUNG          */
/*************************************/

// PORTA ... I/O
// PA0   ... 
// PA1   ... 
// PA2   ... 
// PA3   ... 
// PA4   ... 
// PA5   ... 
// PA6   ... 
// PA7   ... 

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
// PD2   ... TASTER BALL
// PD3   ... TASTER RECHTS
// PD4   ... TASTER LINKS
// PD5   ... TASTER MITTE RECHTS
// PD6   ... TASTER MITTE LINKS
// PD7   ... LCD EN


// PORTE ... I/O
// PE0   ... 
// PE1   ... 
// PE2   ... 
// PE3   ... 
// PE4   ... 
// PE5   ... 
// PE6   ... 
// PE7   ... 


// PORTF ... ADC
// PF0   ... ADC 5
// PF1   ... ADC 4
// PF2   ... ADC 6
// PF3   ... ADC 3
// PF4   ... ADC 7
// PF5   ... ADC 2
// PF6   ... ADC 8
// PF7   ... ADC 1

// PORTG ... LCD
// PG0   ... LCD D4
// PG1   ... LCD D5
// PG2   ... LCD D6
// PG3   ... LCD D7
// PG4   ... LCD RS


#define INTERRUPTS_OFF tmp_sreg=SREG; cli(); 
#define INTERRUPTS_ON SREG=tmp_sreg; 



#define VORNE 0
#define LINKS 1
#define HINTEN 2
#define RECHTS 3

#define IR_VORNE 1
#define IR_LINKS 2
#define IR_HINTEN 3
#define IR_RECHTS 4

/*volatile uint16_t IR_LOW_DURATION;
volatile uint16_t IR_HIGH_DURATION;
volatile uint8_t IR_READY;*/

uint8_t tmp_sreg;  // temporaerer Speicher fuer das Statusregister

uint8_t robot;


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

uint8_t BALL_ENTFERNUNG;


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
/*          EINGÄNGE ADC        */
/********************************/

uint16_t ENTFERNUNGEN[4];

uint16_t IR_BALL[8];

void IR_BALL_MESSUNG(void);
void ENTFERNUNGSMESSUNG(void);
 
uint16_t ADCMESSUNG(uint8_t Buchse);


/*************************/
/*          LCD          */
/*************************/
char Zahl[5];

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

void LCD_CLEAR(uint8_t clear);

/*************************/
/*    IR BALLSENSOREN    */
/*************************/

void MUX_INPUT_SELECT(uint8_t input);

uint16_t IR_BALL[8];
