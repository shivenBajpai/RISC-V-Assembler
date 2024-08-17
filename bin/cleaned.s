.data
.dword 5, 12, 0, 0, 0, 312, 1346, 12, -12, 84, 1
.text
lui x3, 0x10000
jal x0, Start
blt a1, a2 R_exit
sub a1, a1, a2
jal x0, R_loop
add a1, a1, a2
sub a2, a1, a2
sub a1, a1, a2
jalr x0, x1, 0
addi x4, x3, 0x200
ld a0, 0(x3)
addi x3, x3, 8
beq a0,x0, Exit
ld a1, 0(x3)
ld a2, 8(x3)
add a3, x0, x0
bge x0, a1, End_Segment
bge x0, a2, End_Segment 
bge a1, a2, Inner_Loop
add a1, a1, a2
sub a2, a1, a2
sub a1, a1, a2
jal Remainder
bne a2, x0, Inner_Loop
add a3, a1, x0
sd a3, 0(x4)
addi a0, a0, -1
addi x4, x4, 8
addi x3, x3, 16
jal x0, Outer_Loop
beq x0,x0 Exit
