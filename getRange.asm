

    INCLUDE 'derivative.inc'

  xdef getRange

 getRange:;(D = SampleFrequency)
  xgdx         ;X = sampleFrequncy  
  ldy #0

  ;Trigger Lidar
  bset PTH,  #1 ; Then set PH0
  bclr PTH,  #1 ;Trigger by pulling PH0 low

  ;Setup  Timer
  movb #%00000001, DDRH ; Port H, PH0 is output
  movb #%00000000, DDRT ; Port T all inputs
  bclr TIOS,  #%00000010 ;Set TC1 to input compare
  movb #%00000100, TSCR2; Timer prescaler 16  hence 1cm => delatTC1 = 15 
                        
  
  ;stack |sampleFrequncy|sampleSum|
  ;Start Timer compare
  movb #%10010000, TSCR1  ; Start timer in quick flag reset
  movb #2, TFLG1 ;clear flag
  
  ;notes: The sum of samples will be stored in (Y:D) = SS 
  sample:
  pshd                    ; push lower word of sum to stack
  ldd #0                  ; D = 0
  
  movb #%00000100, TCTL4 ;Trigger on falling edge
  brclr TFLG1, #$02, *    ;Wait until PT1 goes low
  subd TC1                ; D = -TC1(old) 
  
  movb #%00001000, TCTL4  ;Trigger on rising edge 
  brclr TFLG1, #$02, * ;Wait until PT1 goes high   
  addd TC1                ; D = TC1(new) - TC1(old)

  addd 2, SP+             ; D = (TC1(new) - TC1(old)) + (SS+2:SS+3)
  bvc skip1               ; If D overflows
  iny                     ; Increment Y
  skip1:
  
  dbne X, sample          ;Repeat until all samples are taken
  
  ldx #15                 ;Devide by 15 to get into cm
  ediv
  bset PTH, #1
  xgdy                    ;D <=> Y
  rts
