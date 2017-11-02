;Author: Chinmoy Mohapatra
;	program to find the greatest of three numbers
;Output in register R0
;

	PRESERVE8
	AREA	go3, CODE, READONLY
	EXPORT __main
	ENTRY

N1 EQU 8
N2 EQU 6
N3 EQU 7

x RN R1
y RN R2
z RN R3

__main FUNCTION
	MOV x, #N1
	MOV y, #N2
	MOV z, #N3
	
	CMP x, y
	BGT b1
	CMP y, z
	MOVGT R0, y
	MOVLE R0, z
	B stop
b1
	CMP x, z
	MOVGT R0, x
	MOVLE R0, z
stop
	B stop
	
	ENDFUNC
	END