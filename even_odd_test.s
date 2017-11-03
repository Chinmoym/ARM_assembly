; even or odd
; z =1 even z=0 odd
; R1 = all FF for odd and R1 = all 0 for even
	AREA	ce,CODE,READONLY
	EXPORT __main
	ENTRY
N EQU 25	

__main FUNCTION
	
	MOV R0, #N
	TST R0, #1
	ITE EQ
	MOVEQ R1, #00
	MOVNE R1, #-1
stop 
	B stop
		
	ENDFUNC
	END
