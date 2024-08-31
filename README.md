# A Quick and Dirty RISC-V Assembler

This is a RISC-V Assembler written to be RV64I Capable for a college project (CS2323 my beloved).

# How To use
1) Run `make build` to compile the project, binary is produced in `/bin` called `riscv_asm`
2) Run the binary. A temporary file called `cleaned.s` will be created and the machine code output shall be in `output.hex` in the same directory by default. 
See the next section for how to specify input/output files

# Command line switches
`-d` 
`--debug`
Runs the assembler in debug mode, providing more verbose output as it runs

`-i <filename>`
`--input <filename>`
Specifies a file for input, defaults to input.s if not specified

`-o <filename>`
`--output <filename>`
Specifies a file for output, defaults to output.hex if not specified

A more detailed report on the design and features of this assembler is present in `report.pdf` at the root of this project

# File structure 

```
.
+-- root
    +-- bin/                    (Contains output binary when built)
    +-- build/                  (Object files are placed here)
    +-- report/
    |   +-- AI24BTECH11030.tex  (The latex file for project report)
    +-- src/
    |   +-- index.c             (Defines label_index struct)
    |   +-- index.h
    |   +-- main.c              (Main file)
    |   +-- translator.c        (Contains most functions and hard-coded data 
    |   |                        regarding instruction parsing)
    |   +-- translator.h
    |   +-- vec.c               (Defines the vec struct)
    |   +--  vec.h
    +-- tests/
    |   +-- cases/              (The actual tests cases are stored here)
    |   |   +-- (...)
    |   +-- outputs/            (Program output from running tests goes here)
    |   +-- test.bash           (Basic Test harness used for running tests)
    +-- .gitignore
    +-- LICENSE
    +-- README.md
    +-- report.pdf              (Project report)
    +-- Makefile
```