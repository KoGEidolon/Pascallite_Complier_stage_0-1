; Huan Tran & Duc Huy Nguyen       Wed Nov 18 20:16:29 2020
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"

SECTION .text                           
global  _start                          ; program stage1no135

_start:                                 
        mov     eax,[TRUE]              ; AReg = true
        mov     [B0],eax                ; a = AReg
        not     eax                     ; AReg = !AReg
        mov     [B1],eax                ; b = AReg
        mov     eax,[B0]                ; load a in eax
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
        mov     eax,[B1]                ; load b in eax
        cmp     eax,0                   ; compare to 0
        je      .L2                     ; jump if equal to print FALSE
        mov     edx,TRUELIT             ; load address of TRUE literal in edx
        jmp     .L3                     ; unconditionally jump to .L3
.L2:                                    
        mov     edx,FALSLIT             ; load address of FALSE literal in edx
.L3:                                    
        call    WriteString             ; write string to standard out
        call    Crlf                    ; write \r\n to standard out
        mov     eax,[FALSE]             ; AReg = false
        mov     [B1],eax                ; b = AReg
        cmp     eax,0                   ; compare to 0
        je      .L4                     ; jump if equal to print FALSE
        mov     edx,TRUELIT             ; load address of TRUE literal in edx
        jmp     .L5                     ; unconditionally jump to .L5
.L4:                                    
        mov     edx,FALSLIT             ; load address of FALSE literal in edx
.L5:                                    
        call    WriteString             ; write string to standard out
        call    Crlf                    ; write \r\n to standard out
        mov     eax,[TRUE]              ; AReg = true
        mov     [B1],eax                ; b = AReg
        cmp     eax,0                   ; compare to 0
        je      .L6                     ; jump if equal to print FALSE
        mov     edx,TRUELIT             ; load address of TRUE literal in edx
        jmp     .L7                     ; unconditionally jump to .L7
.L6:                                    
        mov     edx,FALSLIT             ; load address of FALSE literal in edx
.L7:                                    
        call    WriteString             ; write string to standard out
        call    Crlf                    ; write \r\n to standard out
        cmp     eax,[B0]                ; compare b and a
        je      .L8                     ; b = a then jump to set eax to TRUE
        mov     eax,[FALSE]             ; else set eax to FALSE
        jmp     .L9                     ; unconditionally jump
.L8:                                    
        mov     eax,[TRUE]              ; set eax to TRUE
.L9:                                    
        not     eax                     ; AReg = !AReg
        mov     [B1],eax                ; b = AReg
        mov     eax,[B0]                ; load a in eax
        cmp     eax,0                   ; compare to 0
        je      .L10                    ; jump if equal to print FALSE
        mov     edx,TRUELIT             ; load address of TRUE literal in edx
        jmp     .L11                    ; unconditionally jump to .L11
.L10:                                   
        mov     edx,FALSLIT             ; load address of FALSE literal in edx
.L11:                                   
        call    WriteString             ; write string to standard out
        call    Crlf                    ; write \r\n to standard out
        mov     eax,[B1]                ; load b in eax
        cmp     eax,0                   ; compare to 0
        je      .L12                    ; jump if equal to print FALSE
        mov     edx,TRUELIT             ; load address of TRUE literal in edx
        jmp     .L13                    ; unconditionally jump to .L13
.L12:                                   
        mov     edx,FALSLIT             ; load address of FALSE literal in edx
.L13:                                   
        call    WriteString             ; write string to standard out
        call    Crlf                    ; write \r\n to standard out
        Exit    {0}                     

SECTION .data                           
FALSE   dd      0                       ; false
TRUE    dd      -1                      ; true

SECTION .bss                            
B0      resd    1                       ; a
B1      resd    1                       ; b
