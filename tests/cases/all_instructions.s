# A bunch of 
# comments

add x0, s6,  a2
sub x1, s7, a3
xor x2, s8, a4
or x3, s9, a5
and x4, s10, a6
label1:sll x5, s11, a7
srl x6, t3, s2
label2:
label3: sra x7, t4, s3
slt x8, t5, s4
sltu x9, t6, s5 ; end of line comments
label4:
addi tp, x27, 125
xori t0, x28, -2048
ori t1, x29, -82
andi t2, x30, 0x724
slli s0, x31, 0x3F
srli fp, zero, 0x11
srai s1, ra, 077
slti a0, sp, 0b100101010
sltiu a1, gp, 2047

lb x16, 125(s5)
lh x17, 2047(s6)
lw x18, -82(s7)
ld x19, 0x724(s8)
lbu x20, 0x7FF(s9)
lhu x21, 0x11(s10)
lwu x22, 077(s11)
sb x23, 0b100101010(t3)
sh x24, 2047(t4)
sw x25, -2048(t5)
sd x26, 0(t6)
label5:
beq x0, x6, label1 
bne x1, x7, label2 
blt x2, x8, label2 
bge x3, x9, label3 
bltu x4, x10, label5 
bgeu x5, x11, label6 

jal x18, label4
jalr x4, 12(x1)

lui t0, 0x10000
label6: auipc a2, 0xFFFF

ecall
ebreak
