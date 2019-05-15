      INCLUDE 'derivative.inc'

 xdef stepServo, initServos

 initServos:
    bset DDRP, #%10100000                ;Set PWM outputs
    movb #%10100000, PWMPOL        ;Set Duty Cycle High for pwm7-4
    movb #%11000000, PWMCTL        ;Concatenate 6-7 and 4-5

    movb #$44, PWMPRCLK                ; Set prescaler to 16 for clockA and clockB
    movw #30000, PWMPER67           ; Set period: 24,000,000/50/16 = 30,000
    movw #30000, PWMPER45
    clr PWMCLK
    clr PWMCAE
    std PWMDTY45
    std PWMDTY67
    ldy #30

    loop30:
    ldx #$FFFF
    dbne X, *
    dbne Y, loop30
    rts

 stepServo:

    movb #%10100000, PWME              ;Enable Chanels 7-4                                                                                                            ;cg - 2
    bitb   #1
    beq one_motion                                                                                          ;cg - 4
        ;If bit(0) == 1 then set A = 0b11001100                                                 ;c1 - 1
       ldaa #%11001100                                                                                     ;c1 - 3
       bra shifter
       ;Else i.e. bit(0) == 0 then set A = 0b10001000
       one_motion:
       ldaa #%10001000                                                                                    ;c2 - 1

    ;Esential A>>(B>>1)
    shifter:
    lsrb
       shiftLoop:
       decb
       bmi case_right
       lsra
       bra shiftLoop

    case_right:
    psha

    brclr SP,  #%00001000, case_down
    ldd PWMDTY67
    subd 3, SP
    std PWMDTY67

    case_down:
    brclr SP, #%00000100, case_left
    ldd PWMDTY45
    addd 3, SP
    std PWMDTY45

    case_left:
    brclr SP, #%00000010, case_up
    ldd PWMDTY67
    addd 3, SP
    std PWMDTY67
    
    case_up:
    brclr SP, #%00000001, return
    ldd PWMDTY45
    subd 3, SP
    std PWMDTY45
    return:
    pula

    ldd 2, SP
    waitForMove:
    ldx #60000
    dbne X, *
    dbne B, waitForMove
    brset PTP, #%10000000, *
    brset PTP, #%00100000, *
    clr PWME
    rts
