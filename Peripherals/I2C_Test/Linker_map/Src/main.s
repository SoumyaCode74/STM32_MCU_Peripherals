	.cpu arm7tdmi
	.arch armv4t
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 6
	.eabi_attribute 34, 0
	.eabi_attribute 18, 4
	.file	"main.c"
	.text
	.global	a
	.bss
	.align	2
	.type	a, %object
	.size	a, 4
a:
	.space	4
	.global	b
	.align	2
	.type	b, %object
	.size	b, 4
b:
	.space	4
	.global	c
	.section	.rodata
	.align	2
	.type	c, %object
	.size	c, 4
c:
	.word	1
	.global	d
	.align	2
	.type	d, %object
	.size	d, 4
d:
	.word	2
	.global	e
	.data
	.align	2
	.type	e, %object
	.size	e, 4
e:
	.word	3
	.global	f
	.bss
	.align	2
	.type	f, %object
	.size	f, 4
f:
	.space	4
	.global	p
	.data
	.align	2
	.type	p, %object
	.size	p, 4
p:
	.word	5
	.text
	.align	2
	.global	main
	.syntax unified
	.arm
	.type	main, %function
main:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	sub	sp, sp, #12
	mov	r3, #2
	str	r3, [fp, #-8]
	sub	r3, fp, #8
	add	r3, r3, #4
	ldr	r2, .L3
	str	r3, [r2]
	mov	r3, #0
	mov	r0, r3
	add	sp, fp, #0
	@ sp needed
	ldr	fp, [sp], #4
	bx	lr
.L4:
	.align	2
.L3:
	.word	e
	.size	main, .-main
	.data
	.align	2
	.type	a.0, %object
	.size	a.0, 4
a.0:
	.word	1
	.ident	"GCC: (GNU Arm Embedded Toolchain 10.3-2021.10) 10.3.1 20210824 (release)"
