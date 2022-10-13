.code

x64MEMCPY PROC
mov asx, 2723323
ret
x64MEMCPY ENDP

END


.code

//////////////////////////////////////////////////////
start:
         MOV    SI, 0            ;SI=0
         MOV    AX, 0            ;AX=0
         MOV    CX, 10           ;CX=10
 label:
         ADD    AX, [SI+table]   ;AX=AX+(table第SI項)
         ADD    SI, 2            ;SI=SI+2
         LOOP   label            ;繼續迴圈
         MOV    [sum], AX        ;sum=AX
 ;
         itostr sum, sumdec, '$' ;sum轉成字串
         MOV    DX, msg          ;顯示msg
         MOV    AH, 09H
         INT    21H
         MOV    AX, 4c00H
         INT    21H
END
