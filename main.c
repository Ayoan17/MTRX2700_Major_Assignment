#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "AsmFunctions.h"
#define bit(x) (1<<x)
#define SMPL 20
#define REFRESHRATE 500
#define THRESHOLD 15
#define SIZE 22

unsigned int abs(int);
void SerialBegin(int);
void SerialPrintChar(char);
void SerialPrintInt(int);
void initSevenSeg(int);
void intToPrint(int);
interrupt 10 void multiplex();

const char segLookUp[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
char segDigits[5]= {0, 0, 0, 0};
void main(void) {
   int lastRange =  0;
   int direction = 0;
   int size = SIZE;
   int mode = 0;
   int i = 8;
   int j = 100;
   char stage = 0;
    initSevenSeg(REFRESHRATE);
    initServos(2400);
    
    SerialBegin(9600);
    asm cli;
    while(j--){
      
      lastRange = getRange(SMPL)/SMPL;
      intToPrint(lastRange);
      delay(100);
    }
    while(1){//Main loop
      int range = getRange(SMPL)/SMPL;
      int dif = abs(range - lastRange);
      intToPrint(range);
      
      if((range > 110)&&(lastRange < 110)&&((stage == 0)||(stage == 2))){
         direction += 4; //Flip direction
         stage = 1;
         size = 10;
      }else if((range < 110)&&(lastRange > 110)&&(stage == 1)){
         stage = 2;
         size = SIZE;
         i=8;
         SerialPrintInt((PWMDTY45-900)/5);
         SerialPrintChar(',');
         SerialPrintInt((PWMDTY67-900)/5);
         SerialPrintChar(13);
         SerialPrintChar('\n');
         SerialPrintChar(0);
      }
      
  
      if(stage == 2) direction++;
      if(i-- == 0){
       size += 15;  
      }
      stepServo(size, direction%8); 
      delay(size/2);
      
      lastRange = range;
      
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


void SerialBegin(int baudRate){
   long SBR;
   SBR = 1,500,000/baudRate;
   SCI1BDL = 156;
   SCI1BDH = 0;
   SCI1CR1 = 0;
   SCI1CR2 = 0b00001100;
}

void SerialPrintChar(char character){
   while( (SCI1SR1 & bit(7)) == 0 ){  
   }
   SCI1DRL = character;
}

void SerialPrintInt(int number){
   int div = 100;
   while( number/div <= 0 ){
      div /= 10;
      SerialPrintChar('0');
   }
   while( div > 0){
      SerialPrintChar(number/div + 48);
      number %= div;
      div /= 10; 
   }
}

//Initiates seven segment display
void initSevenSeg(int refreshRate){
  TIE    |= 0b00000100;
  TIOS |= 0b00000100;
  TSCR1  = 0x90;
  DDRB    = 0xff;
  DDRP   |= 0x0f;
  TC2 = TCNT + refreshRate;
  intToPrint(0);
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
