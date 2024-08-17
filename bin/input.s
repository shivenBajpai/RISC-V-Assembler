.data
    .dword 5, 12, 0, 0, 0, 312, 1346, 12, -12, 84, 1
    # Expected answers: 0, 0, 2, 0, 1
    
.text
    # The following line initializes register x3 with 0x10000000 
    # so that you can use x3 for referencing various memory locations. 
    lui x3, 0x10000
    #your code starts here
    
    # Using x1 for storing jump addresses
    # Using x3 and x4 for the addresses of input and output in memory
    # a0 for counting cases
    # a1 and a2 for input numbers
    # a3 for storing output temporarily
    # a4-a7 for variables

    jal x0, Start

# Function for calculating remainder by repeated subtraction
# uses a1 and a2 as input and results in
# a2 gets the value a1%a2
# a1 gets the original value of a2
Bullshit:
Remainder:
    R_loop:
        blt a1, a2       R_exit
		sub a1,   a1, a2
        jal x0, 			R_loop
    R_exit:
        add a1, a1, a2
        sub a2, a1, a2
        sub a1, a1, a2
        jalr x0, x1, 0

Start:
    addi x4, x3, 0x200
    ld a0, 0(x3)
    addi x3, x3, 8
    
Outer_Loop:
    # If there are no more cases to process, exit
    beq a0,x0, Exit
    
    ld a1, 0(x3)
    ld a2, 8(x3)
    add a3, x0, x0
    
    # Check if either is 0 or negative
    # if so then skip to the end because a3 is already set to 0
    bge x0, a1, End_Segment
    bge x0, a2, End_Segment    
    
    # Order the inputs so that a1 > a2
    bge a1, a2, Inner_Loop
    add a1, a1, a2
    sub a2, a1, a2
    sub a1, a1, a2
    
    # Solve using Euclidean Algorithm
    Inner_Loop:
        jal Remainder
        bne a2, x0, Inner_Loop
    
    # Final answer ends up in a1. 
    # We copy it to a3 because handling 
    # the 0 case is simpler this way
    add a3, a1, x0
    
End_Segment:
    sd a3, 0(x4)
    addi a0, a0, -1
    addi x4, x4, 8
    addi x3, x3, 16
    jal x0, Outer_Loop
        
Exit:
    beq x0,x0 Exit
     
    #The final result should be in memory starting from address 0x10000200
    #The first dword location at 0x10000200 contains gcd of input11, input12
    #The second dword location from 0x10000200 contains gcd of input21, input22, and so on.
