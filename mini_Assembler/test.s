	push	{r7, lr}
	sub	sp, sp, #32
	add	r7, sp, #0
	movs	r3, #6
	str	r3, [r7]
	movs	r3, #5
	str	r3, [r7, #4]
	movs	r3, #2
	str	r3, [r7, #8]
	ldr	r2, [r7]
	ldr	r3, [r7, #4]
	add	r3, r3, r2
	str	r3, [r7, #12]
	ldr	r2, [r7]
	ldr	r3, [r7, #4]
	subs	r3, r2, r3
	str	r3, [r7, #16]
	ldr	r3, [r7]
	ldr	r2, [r7, #4]
	mul	r3, r2, r3
	str	r3, [r7, #20]
	ldr	r0, [r7]
	ldr	r1, [r7, #4]
	bl	__aeabi_idiv
	mov	r3, r0
	str	r3, [r7, #24]
	movs	r3, #2
	str	r3, [r7, #28]
	movs	r3, #0
	mov	r0, r3
	adds	r7, r7, #32
	mov	sp, r7
	pop	{r7, pc} 

