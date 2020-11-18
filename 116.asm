; Huan Tran & Duc Huy Nguyen       Wed Nov 18 03:41:13 2020
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"

SECTION .text                           
global  _start                          ; program stage1no116

_start:                                 
        mov     eax,[TRUE]              ; AReg = true
        mov     [B3],eax                ; b = AReg
        mov     eax,[B0]                ; AReg = yes
        and     eax,[TRUE]              ; AReg = yes and true
        mov     [T0],eax                ; deassign AReg
        mov     eax,[B1]                ; AReg = no
        or      eax,[FALSE]             ; AReg = no or false
        and     eax,[T0]                ; AReg = T1 and T0
        mov     [B7],eax                ; d = AReg
        mov     eax,[B3]                ; AReg = b
        not     eax                     ; AReg = !AReg
        and     eax,[B0]                ; AReg = T0 and yes
        mov     [B5],eax                ; w = AReg
        mov     eax,[B7]                ; load d in eax
        cmp     eax,0                   ; compare to 0
        je      .L0                     ; jump if equal to print FALSE
        mov     edx,TRUELIT             ; load address of TRUE literal in edx
        jmp     .L1                     ; unconditionally jump to .L1
.L0:                                    
        mov     edx,FALSLIT             ; load address of FALSE literal in edx
.L1:                                    
        call    WriteString             ; write string to standard out

SECTION .data                           
TRUELIT db      'TRUE',0                ; literal string TRUE
FALSLIT db      'FALSE',0               ; literal string FALSE

SECTION .text                           
        call    Crlf                    ; write \r\n to standard out
        mov     eax,[B5]                ; load w in eax
        cmp     eax,0                   ; compare to 0
        je      .L2                     ; jump if equal to print FALSE
        mov     edx,TRUELIT             ; load address of TRUE literal in edx
        jmp     .L3                     ; unconditionally jump to .L3
.L2:                                    
        mov     edx,FALSLIT             ; load address of FALSE literal in edx
.L3:                                    
        call    WriteString             ; write string to standard out
        call    Crlf                    ; write \r\n to standard out
        Exit    {0}                     

SECTION .data                           
FALSE   dd      0                       ; false
B1      dd      0                       ; no
TRUE    dd      -1                      ; true
B0      dd      -1                      ; yes

SECTION .bss                            
T0      resd    1                       ; T0
B2      resd    1                       ; a
B3      resd    1                       ; b
B4      resd    1                       ; c
B7      resd    1                       ; d
B8      resd    1                       ; e
B5      resd    1                       ; w
B6      resd    1                       ; z
