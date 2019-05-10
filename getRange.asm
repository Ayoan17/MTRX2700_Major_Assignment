

    INCLUDE 'derivative.inc'

  xdef getRange

 getRange:

  pshd     ;Push the sample number to the stack
  ldd #0   ;Push 0 to the stack, this will be used
  pshd     ;To hold the sum of samples

  ;Setup
  movw #0, $1010
  movb #%00000001, DDRH ; Port H, PH0 is output

  movb #%00000000, DDRT ; Port T all inputs

  bclr TIOS,  #%00000010 ;Set TC1 to input compare
  bset TCTL4, #%00001100 ;Trigger on change of PT1's state

  movb #%00000011, TSCR2; Timer prescaler 4  3m = 3ms = 3*24000 = 72000
                        ; So prescaler > 2 should work but as we are taking
                        ; multiple readings a greater scaler will stop overflow
  movb #%10010000, TSCR1; Start timer in quick flag reset

  movb #2, TFLG1 ;clear flag
  ldy #3         ;Y = 3 will be used to get the low wave length and also ignore
                 ;the first reading


  bset PTH,  #%00000001 ; Then set PH0
  bclr PTH,  #%00000001 ;Trigger by pulling PH0 low

  rePoll:
  movw TC1, 2, -SP ;Push TC1 to the stack
  
  poll:
    brclr TFLG1, #$02, poll ;Poll until TFLG1 bit 1 goes high

  ldd TC1      ;Determine the difference from the last TC1  trigger
  subd 2, SP+  ;and return the stack pointer back to the sample sum position


  dey          ;Decrement y and if Y == 0 then we will add the sample to the
  bne rePoll   ;sample sum in the stack

  ldx #30      ;Devide by 30 to get into cm
  idiv
  pshx         ;Push the result to the stack
  puld         ;Pull the result into D ... D = X = sampleLength/30
  addd 2, SP+  ;Add the sample sum to result D = D + sampleSum and return the
               ;Stack pointer to the sample number
  pshd         ;Push the new sample sum

  ldy #2       ;y = 2 to get every second value
  dec [2, SP] ;Decreament the sample sum
  bne rePoll   ;If all samples have been taken
  puld         ;Load d with the sample sum and return
  rts
