
.global _spinunlock, _spinlock, _dumpasm

_spinlock:
	push   %ebp
	mov    %esp, %ebp
	mov    0x8(%ebp),%eax
	movl  $0x1, (%eax)
.loop1:
	mov   0x8(%ebp),%eax
	mov   (%eax), %eax
	test   %eax, %eax
	jne    .loop1

	pop    %ebp
	ret

_spinunlock:
	push   %ebp
	mov    %esp, %ebp
	mov    0x8(%ebp), %eax
	movl   $0x0,(%eax)
	pop    %ebp
	ret    

_dumyasm:
	ret
