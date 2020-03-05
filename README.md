# Brycore-8bit
>an 8-bit micro processor simulator built in c++.

## General features
- 4 General purpose registers. (GPR)
- 256 Bytes of random accessible memory. (RAM)
- Supports a stack and base pointer register.
- Includes the python BRASM 8-bit Assembler.
- 60 Opcodes including the debugging of registers and flags.
- Supports sub-routines using the call and ret operations.
- Intel x86 architecture.
## Instruction and Label syntax
in BRASM the instruction syntax isn't all that complicated, it goes something like **Instruction,  Destination, Source.**

Although if you're coding Brycore 8-bit in binary syntax, you would type the opcode associated with the task you're trying to complete.

### BRASM Labels.
Labels are the same as constant addresses, the only difference is that they can help you out with offsets so you don't have to type them new every time.
You also might use labels to save an address to an virtual memory location.
to initialize a label simply type **LABELNAME:**

Note: " you **must** initialize a label with the semicolon at the end. "

Labels can be used in any place a constant can be used, in order to reference a label you have to put a percent symbol at the beginning of the name.
for example: **%LABELNAME** will be replaced with the address LABELNAME is pointing to.

Note: " Labels **must** start with a letter or an underscore. "
## Defining raw bytes
> There are a couple ways to define bytes in BRASM. whether is defining a str of bytes, an array of bytes or a bunch of zero bytes.

To define ten zero bytes:

    DZ 10
To define a series of bytes:

    DB 10 0 25 32
To define a string of bytes:

    .str "Hello, world!"


## Instruction set

### Operand types
>This is a list of all the operand types that Brycore 8-bit and BRASM supports when using instruction argumentation.

| ID | Type | Description | Example |
|----|------|-------------|---------|
| 0 | reg | an 8-bit register. | A |
| 1 | regAddr | returns the addr of the register. | [A] |
| 2 | reg \w offset addr | returns addr of register with offset | [A+15]|
| 3 | constAddr | returns addr of const | [234] |
| 4 | const | 8-bit char with range '0-255' | 145 |

Note: " the offset can range from 0-15 and works for addition and subtraction. "

### Available Instructions
> a list of all available Instructions/opcodes associated with Brycore 8-bit.

| Opcode | Name | Syntax | Description |
|--------|------|--------|-------------|
| 0x00 | HLT | Null | Halts the processor.
| 0x01 | MOV_04 | reg, const | Moves constant into register.|
| 0x02 | MOV_01 | reg, regAddr | Moves a register address into a register. |
| 0x03 | MOV_00 | reg, reg | Moves register into register. |
| 0x04 | MOV_14 | regAddr, const | Moves constant into register address.
| 0x05 | MOV_03 | reg, constAddr | Moves constant address into register.
| 0x06 | MOV_10 | regAddr, reg | Moves register into regAddr. |
| 0x07 | MOV_24 | reg \w offset Addr, const | Moves constant into address of reg +/- an offset. |
| 0x08 | MOV_02 | reg, reg \w offset Addr | Moves address of reg +/- an offset into register. |
| 0x09 | MOV_20 | reg \w offset Addr, reg | Moves register into address of reg +/- an offset. |
| 0x0A | MOV_30 | constAddr, reg | Moves register into constant address. |
| 0x0B | MOV_34 | constAddr, const| Moves constant into constant address. |
| 0x0C | JMP_0 | reg | Jumps to a register value. |
| 0x0D | JC_0 | reg | Jumps if carry flag is set '<'. |
| 0x0E | JZ_0 | reg | Jumps if zero flag is set '='. |
| 0x0F | JBE_0 | reg | Jumps if zero and carry '<='. |
| 0x10 | JNC_0 | reg | Jumps if not carry '>='. |
| 0x11 | JNZ_0 | reg | Jumps if not zero '!='. |
| 0x12 | JNBE_0 | reg | Jumps if not zero and carry '>'. |
| 0x13 | JMP_4 | const | Jumps to a register value. |
| 0x14 | JC_4 | const | Jumps if carry flag is set '<'. |
| 0x15 | JZ_4 | const | Jumps if zero flag is set '='. |
| 0x16 | JBE_4 | const | Jumps if zero and carry '<='. |
| 0x17 | JNC_4 | const | Jumps if not carry '>='. |
| 0x18 | JNZ_4 | const | Jumps if not zero '!='. |
| 0x19 | JNBE_4 | const | Jumps if not zero and carry '>'. |
| 0x1A | PUSH_0 | reg | Pushes register. |
| 0x1B | PUSH_4 | const | Pushes constant. |
| 0x1C | POP_0 | reg | Pops a value off of the stack and stores it in a register. |
| 0x1D | ADD_00 | reg, reg | Adds a register by a register. |
| 0x1E | ADD_04 | reg, const | Adds a register by a constant. |
| 0x1F | SUB_00 | reg, reg | Subtracts a register by a register. |
| 0x20 | SUB_04 | reg, const | Subtracts a register by a constant. |
| 0x21 | MUL_4 | reg | Multiplies the accumulator by a register. |
| 0x22 | MUL_0 | const | Multiplies the accumulator by a constant. |
| 0x23 | DIV_4 | reg | Divides the accumulator by a register. |
| 0x24 | DIV_0 | const | Divides the accumulator by a constant. |
| 0x25 | INC_0 | reg | Increments a register. |
| 0x26 | DEC_0 | reg | Decrements a register. |
| 0x27 | CMP_00 | reg, reg | Compares a register by a register. |
| 0x28 | CMP_03 | reg, constAddr | Compares a register by a const address. |
| 0x29 | CMP_02 | reg, reg \w offset Addr | Compares a register by a address of reg +/- an offset. |
| 0x2A | CMP_04 | reg, const | Compares a register by a constant. |
| 0x2B | CMP_01 | reg, regAddr | Compares a register by a register const. |
| 0x2C | SHL_4 | const | Shifts the accumulator to the left by const value. |
| 0x2D | SHL_0 | reg | Shifts the accumulator to the left by register value. |
| 0x2E | SHR_4 | const | Shifts the accumulator to the right by const value. |
| 0x2F | SHR_0 | reg | Shifts the accumulator to the right by register value. |
| 0x30 | AND_04 | const | Bitwise AND operation. |
| 0x31 | AND_00 | reg | Bitwise AND operation. |
| 0x32 | OR_04 | const | Bitwise OR operation. |
| 0x33 | OR_00 | reg | Bitwise OR operation. |
| 0x34 | XOR_04 | const | Bitwise XOR operation. |
| 0x35 | XOR_00 | reg | Bitwise XOR operation. |
| 0x36 | NOT_0 | reg | Bitwise NOT operation. |
| 0x37 | CALL_0 | reg | Jumps into a sub-routine and pushes the return address. |
| 0x38 | CALL_4 | const | Jumps into a sub-routine and pushes the return address. |
| 0x39 | RET | Null | Pops return address into the Instruction pointer. |
| 0x3A | LEA_02 | reg, reg \w offset Addr | Moves the result of the Source operand into a register. |
| 0x3B | INT_4 | reg | Makes calls to the system interrupt vector.

## interrupts
> a list of all the available interrupts currently.

| ID | Description |
|----|-------------|
| 0 | read char into constAddr of register A |
| 1 | prints char from constAddr of register A |

## Building the application

### Requirements
> these are the requirements for building/running the application.
- an early/late build of the ncurses library.
- a basic compiler for the c++ source code.
- Python interpreter version 3 or greater.
### building
Building Brycore can be done with the built-in make file.

    ~/Brycore-8bit/src$
    make all
if you want to built it using the command you have to make sure you include the ncurses library.

    ~/Brycore-8bit/src$
    g++ main.cpp lib/Brycore.cpp -l ncurses -o brycore
    
## Running the application
Brycore requires one argument when running the program, the argument being the file of a brycore 8-bit binary. to create a brycore 8-bit binary you can use BRASM, which also takes one argument which is the assembly file you wish to assemble. For example:

Creating an example assembly file.

    ~/Brycore-8bit/src/BRASM$
    vim test.asm
    
    mov a, 34
then you need to assemble it using BRASM.

    ~/Brycore-8bit/src/BRASM$
    python3 main.py
    File name: test.asm
    ...
once assembled and turned into bytes you'll need to run the program to make sure its working the way you want.

    ~/Brycore-8bit/src$
    ./brycore BRASM/test.out

## Todo list
- add more IRQ's for the system call vector.
- make stronger exception handling for BRASM.
- make a memory monitor/dump for debugging
- make a step option for debugging
- change the clock speed.
## Disclaimer
Warning this is a beta version of the software. while using this software you may experience bugs or crashes.  if you have an problem loading/running this program please report the issue. Thank you!