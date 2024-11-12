# CS2323 Lab 3 - RISC-V Assembler

## How To use
1) Run `make build` to compile the project, binary is produced in `/bin` called `riscv_asm`
2) Run the binary. A temporary file called `cleaned.s` will be created and the machine code output shall be in `output.hex` in the same directory by default. 
See the next section for how to specify input/output files

## Command line switches
`-d` 
`--debug`
Runs the assembler in debug mode, providing more verbose output as it runs

`-i <filename>`
`--input <filename>`
Specifies a file for input, defaults to input.s if not specified

`-o <filename>`
`--output <filename>`
Specifies a file for output, defaults to output.hex if not specified

`-b`
`--binary`
Indicates that the output should be binary, by default output is machine readable hexcodes

A more detailed report on the design and features of this assembler is present in `report.pdf` at the root of this project