
.global spinunlock, spinlock

spinlock:
	push   %ebp
	mov    %esp, %ebp
	mov    0x8(%ebp),%ecx
	movl  $0x1, %eax
	xchgl %eax, (%ecx)
.loop1:
	mov   0x8(%ebp),%eax
	mov   (%eax), %eax
	test   %eax, %eax
	jne    .loop1

	pop    %ebp
	ret

spinunlock:
	push   %ebp
	mov    %esp, %ebp
	mov    0x8(%ebp), %ecx
	mov    $0x0, %eax
	xchgl  %eax, (%ecx)
	pop    %ebp
	ret    
