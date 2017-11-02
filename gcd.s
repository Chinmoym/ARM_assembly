;Author: Chinmoy Mohapatra
;	program to find gcd of two numbers x and y
;Output:
;	result present in both R1 and R2


	PRESERVE8
	AREA gcd,CODE,READONLY
	EXPORT __main
	ENTRY

N1 EQU 1
N2 EQU 25

x 	RN R1
y 	RN R2

__main FUNCTION
	MOV x, #N1
	MOV y, #N2
loop	
	CMP x, y
	SUBGT x, x, y
	SUBLT y, y, x
	BNE loop
	
stop 
	B stop	
	ENDFUNC
	END
	
	
	