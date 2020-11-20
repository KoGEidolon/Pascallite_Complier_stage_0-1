; Huan Tran & Duc Huy Nguyen       Thu Nov 19 18:56:10 2020
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"

SECTION .text                           
global  _start                          ; program stage1no114

_start:                                 
        mov     eax,[I1]                ; AReg = five
        imul    dword [I2]              ; AReg = five * a
        mov     [I6],eax                ; d = AReg
