;Author: Chinmoy Mohapatra
;	Program: to find the nth fibonacci number 
;Output
;	R1 = Fn and R0 = Fn-1
;

	PRESERVE8
	AREA fibonnacci, CODE, READONLY
	EXPORT __main
	ENTRY
	
N EQU 5
	
__main FUNCTION
	MOV R0, #0
	MOV R1, #1
	MOV R5, #2
	MOV R6, #0
	MOV R7, #N
	SUB R7, R7, #1
fib
	CMP R5, R7
	
	ADD R6, R1, R0
	MOV R0, R1
	MOV R1, R6
	
	ADD R5, #1
	BLT fib
	
stop
	B stop
	ENDFUNC
	END