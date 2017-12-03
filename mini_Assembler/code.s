	push	{r7}
	sub	sp, sp, #20
	add	r7, sp, #0
	movs	r3, #10
	str	r3, [r7, #4]
	movs	r3, #5
	str	r3, [r7, #8]
	ldr	r2, [r7, #4]
	ldr	r3, [r7, #8]
	add	r3, r3, r2
	str	r3, [r7, #12]
	movs	r3, #0
	mov	r0, r3
	adds	r7, r7, #20
	mov	sp, r7
	ldr	r7, [sp], #4
	bx	lr
