# A Quick and Dirty RISC-V Assembler

This is a RISC-V Assembler written to be RV64I Capable for a college project (CS2323 my beloved).

# How To use
1) Run `make build` to compile the project, binary is produced in `/bin` called `riscv_asm`
2) Copy the input assembly code to a file called input.s in the same directory as the binary
3) Run the binary. A temporary file called `clean.s` will be created and the machine code output shall be in `output.hex`. Both in the same directory

A more detailed report on the design and features of this assembler is present in `report.pdf` at the root of this project