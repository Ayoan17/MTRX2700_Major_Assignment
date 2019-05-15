#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "AsmFunctions.h"
#define bit(x) (1<<x)
#define SMPL 20
#define REFRESHRATE 100
#define THRESHOLD 15

unsigned int abs(int);
void delay(long int);
void initSevenSeg(int);
void intToPrint(int);
interrupt 10 void multiplex();

const char segLookUp[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
char segDigits[4]= {0, 0, 0, 0};

void main(void) {
   int lastRange =  0;
   int direction = 0;
   int size = 50;

//  SCI1BDH = 0;
//  SCI1BDL = 0x9C;
//  SCI1CR1 = 0;
//  SCI1CR2 = 0b00001000;

    initSevenSeg(REFRESHRATE);
    initServos(2500);
    lastRange = getRange(SMPL)/SMPL;
    asm cli;

    while(1){//Main loop
      int range = getRange(SMPL)/SMPL;
      int dif = abs(range - lastRange);
      lastRange = range;

       if(dif > THRESHOLD){
          direction += 5;
          stepServo(size, direction%8);
       }
                                     //If no edge detected
       stepServo(size, direction%8);
       direction++;
    }
}

//Absolute value
unsigned int abs(int num){
 if(num < 0){
   return num*-1;
 }else{
   return num;
 }
}

//Basic Delay
void delay(long int time){
   while(time-->0);{
   }
}

//Initiates seven segment display
void initSevenSeg(int refreshRate){
  TIE    |= 0b00000100;
  TIOS   |= 0b00000100;
  DDRB    = 0xff;
  DDRP   |= 0x0f;
  TC2 = TCNT + refreshRate;
}

//Converts a 4 digit interger into 4 characters for the seven seg to print
void intToPrint(int range){
    int i = 0;
    while(range > 0){
      segDigits[i++] = segLookUp[range%10];
      range = range/10;
    }
    while(i < 4){
      segDigits[i++] = 0;
    }
}

//Multiplex's the 4 characters on the seven segment display at a speed of REFRESHRATE
char digPointer = 0;
interrupt 10 void multiplex(){
  TC2 = TCNT + REFRESHRATE;
  PORTB = 0;
  PTP = 15^(8>>digPointer);
  PORTB = segDigits[digPointer];
  if(digPointer++ > 3) digPointer = 0;
}
