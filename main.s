section .rodata
  L1 db "Hello world",0x0A,0 
  E1 equ $ - L1

section .text
  global _start

_start:

  xor rbp,rbp
  and rsp,0xFFFFFFFFFFFFFFFF
  push rbp
  mov rbp,rsp
  sub rsp,32

  mov rax,1
  mov rdi,1
  lea rsi,[rel L1]
  mov rdx,E1
  syscall

  leave

  mov rax,60
  xor rdi,rdi
  syscall
