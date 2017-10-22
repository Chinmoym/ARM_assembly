; Circular queue
; Q_HEAD and Q_TAIL represent the head and tail of the Q, all FF is the default value
; check Q_FULL and Q_EMPTY register for full or empty
;		all FF - full/empty
;		all 00 - not full / not empty
; 
;

Q_START_ADDR 	EQU 0x20000000
Q_SIZE			EQU 15 ;no of elements the Q can store(each 32-bits)- 40bytes
Q_SIZE_BYTES	EQU 60
Q_END_ADDR		EQU 0x2000000A
Q_HEAD			RN  R2
Q_TAIL			RN  R3
Q_DATA_IN		RN  R0
Q_DATA_OUT		RN  R1
Q_FULL			RN  R4
Q_EMPTY			RN	R5
TMP1			RN  R8
TMP2			RN  R9
TMP3			RN	R10
TMP4			RN 	R11

	
	PRESERVE8
    THUMB
    AREA circular_queue, CODE, READONLY
    EXPORT __main
	ENTRY 
	
	
__main FUNCTION

	MOV R7, #Q_START_ADDR	
	MOV R6, R7				;R7 and R6 holds the starting address of queue
	
	;MOV R5, #Q_SIZE
	;MOV R4, R5				;R5 and R4 holds the Q size (10 - elements)
	
	BL Initialize			;initialize the Q to all FF (easy to watch ih memory window)
	MOV R6, R7				;get the address backup
	
	
	;insert 10 numbers
	MOV TMP1, #10
	MOV TMP2, #0
in_loop1

	EOR Q_DATA_IN, Q_DATA_IN, #0x36000000
	SUB Q_DATA_IN, Q_DATA_IN, #15
	
	BL q_insert

	ADD TMP2, TMP2, #1
	CMP TMP1, TMP2
	BNE in_loop1
	
	;remove 3 number of times
	MOV TMP1, #3
	MOV TMP2, #0
in_loop2

	BL q_remove

	ADD TMP2, TMP2, #1
	CMP TMP1, TMP2
	BNE in_loop2
	
	;insert 7 number of times
	MOV TMP1, #7
	MOV TMP2, #0
in_loop3

	EOR Q_DATA_IN, Q_DATA_IN, #0x36000000
	SUB Q_DATA_IN, Q_DATA_IN, #15
	
	BL q_insert

	ADD TMP2, TMP2, #1
	CMP TMP1, TMP2
	BNE in_loop3
	
	;remove 13 number of times
	MOV TMP1, #13
	MOV TMP2, #0
in_loop4

	BL q_remove

	ADD TMP2, TMP2, #1
	CMP TMP1, TMP2
	BNE in_loop4	
	
	
stop B stop
	
; dequeue operation
q_remove
	PUSH {LR}
	CMP Q_TAIL, Q_HEAD ; check for empty
	;BGT rem_back
	MOV TMP3, #Q_START_ADDR
	ADD TMP3, TMP3, #Q_SIZE_BYTES
	MOV TMP4, Q_HEAD
	LDR Q_DATA_OUT, [Q_HEAD], #4
	STR Q_EMPTY, [TMP4]
	CMP TMP3, Q_HEAD
	BGT remove1
	;head is at the end
	MOV Q_HEAD, #Q_START_ADDR
	POP {PC}
remove1
	POP {PC}
;rem_back
	;POP {PC}
	
q_insert
	PUSH {LR}	; store the return address
	;CMP Q_HEAD, Q_TAIL ; check for empty slots
	;BGT ins_back
	MOV TMP3, #Q_START_ADDR
	ADD TMP3, TMP3, #Q_SIZE_BYTES
	STR Q_DATA_IN, [Q_TAIL], #4
	CMP TMP3, Q_TAIL
	BGT insert1
	;tail is at end of queue	
	MOV Q_TAIL, #Q_START_ADDR
	POP{PC}
insert1 ;tail is not at end of queue
	POP {PC} ; return to main
;ins_back
	;POP {PC}
	
; to initialize the memory location
Initialize
	PUSH {LR}			;store the return address
	
	MOV Q_FULL, #0
	MOV Q_EMPTY, #0xFFFFFFFF
	MOV TMP1, #0
	MOV TMP2, #0xFFFFFFFF
	MOV Q_HEAD, #Q_START_ADDR
	MOV Q_TAIL, #Q_START_ADDR
	
init_loop
	
	STR TMP2, [R6], #4	;offset of 4 to have 32 bit boundary
	
	ADD TMP1, TMP1, #1
	CMP TMP1, #Q_SIZE
	BNE init_loop
	
	POP {PC}			; return to main
	

	ENDFUNC
	END

	
	