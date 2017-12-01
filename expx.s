; Author: Chinmoy Mohapatra
;	program to find exp(x) in M4-FPU 
; Output avaialble in S3 register
	
X 	EQU 0x3F800000 ;ieee 754 for 1.0  (x in e^x)
TEMP 	EQU	0x3F800000 ;ieee 754 for 1.0
;PREC	EQU 0x3A83126F ;precision till decimal  (WIP unable to put into work properly)
	
	PRESERVE8
	THUMB
	AREA expx,CODE,READONLY
	EXPORT __main
	ENTRY
	
__main FUNCTION
	
	MOV R0, #X
	MOV R1, #TEMP
;	MOV R2, #PREC
	MOV R3, #0 ;start from 1 keep loop count
	
	VMOV.F32 S0, R0
	VMOV.F32 S1, R1
	VMOV.F32 S3, R1
	VMOV.F32 S4, R1
	VMOV.F32 S5, R3
	VMOV.F32 S6, R1 ; increase the counter
	
loop
	ADD R3, R3, #1
	VADD.F32 S5, S5, S6 ; contains i start i from 1
	VMUL.F32 S1, S1, S0 ; find x^ (1*x) -- (1*x*x) and so on
	VMUL.F32 S4, S4, S5 ; fact(i)
	VDIV.F32 S2, S1, S4 ; do x^/fact(i)
	VADD.F32 S3, S3, S2 ; cummulative sum
	
	CMP R3, #10
	BNE loop
	
stop
	B stop
	ENDFUNC
	END
