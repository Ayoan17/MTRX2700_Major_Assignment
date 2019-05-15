      INCLUDE 'derivative.inc'

      xdef delay
      
delay:
  brn    delay      ;  1cc*D  (filler)
  ldx #7998      ;  2cc*D
  dbne X, *      ;  3cc*X*D
  dbne D, delay  ;  3cc*D
  rts
; TotalCC ~= 6D + 3XD             note: ignore rts and branching
; D = 1 | TotalCC = 24,000 (1ms)        in as they happen once
; 24,000 = 6 + 3X                       and therefore don't scale
; X = 7998
; Hence the delay will be Dms with an error ~10cc or 0.42us
