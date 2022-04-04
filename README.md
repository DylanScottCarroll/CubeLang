### Overview

**Programs**

Every program consists of a NxNxN cube of 16 bit values, one 16 bit register, and two spatial pointers.

You define a program by writing out that array as a list of N^3 values written in hex separated by spaces. The interpreter otherwise ignores whitespace, so these values can be arranged in any way. However, it is recommended that the rows of values be separated by newlines and layers be separated by multiple newlines. The dimensions of the cube must be equal otherwise the program will not run.

The first value in every program is the value of N; the size of the cube in hex

**Pointers**
In that space exist two pointers: the instruction pointer and the data pointer. The instruction pointer reads data from the cube and executes the values as instructions and the data pointer is able to save and load values from the cube to the register and preform operations with the register.

The data pointer interprets the values in the cube as signed for the purposes of operations where that makes a difference.

For movement of the pointers, the axes are ordered x, y, z. If formatted in the standard way, this means that +x = left, -x = right, +y down, -y=up, +z = out (down a layer), -z in (up a layer).

At the beginning of the program the instruction pointer is at the top, right, back corner (the lowest of all three dimensions) and the data pointer is at the top, right, front corner (minimum in x and y but maximum of z).

### Instructions

**Format**


|15|14|13|12|11|10| 9| 8| 7| 6| 5| 4| 3| 2| 1| 0|
|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|
| U| U| C| C| C| O| O| O| O| O| D| D| D| I| I| I|

U - Unused
C - Conditional Moves
O - Op-code
D - Data pointer direction
I - Instruction  pointer direction.

The lowest 6 bits of every instruction indicate the movement of the instruction and data pointers. The lowest three bits indicate the movement of the instruction pointer and the next three indicate the movement of the data. A move off the edge of the cube wraps to the other side.

| Value | Direction |
|-------|-----------|
|   0   |    Stay   |
|   1   |    +x     | 
|   2   |    -x     |
|   3   |    +y     |
|   4   |    -y     |
|   5   |    +z     |
|   6   |    -z     |

The next 5 bits represent the operation code and represents the operation that is preformed

The next 3 bits are only used for conditional jumps. They represent the direction that the instruction pointer will move in the case that the condition is satisfied. If the condition is not satisfied, the pointer will move according to the lowest three bits as normal.

**Instruction List**
All instructions operate on the values in the register and the value currently pointed at by the data pointer.

For the descriptions R represents the value in the register and V represents the value at the data pointer, and the results of operations are stored in R unless otherwise specified

| Save/Load  |                |
|------------|----------------|
|     SAV    | Saves R to V   |
|     LOD    | Loads V to R   |


| Arithmetic |                                 |
|------------|---------------------------------|
|     ADD    | Adds V to R                     |
|     SUB    | Subtracts V from R              |
|     INC    | Increments R by 1               |
|     DEC    | Decrements R by 1               |
|     MUL    | Multiply R by V                 |
|     DIV    | Divides R by V                  |
|     MOD    | Stores the mod of R and V in R  |

| Bitwise |                                                                      |
|---------|----------------------------------------------------------------------|
|   AND   | ANDs the bits of V and R                                             |
|   ORR   | OR s the bits of V and R                                             |
|   NOT   | NOTs the bits of R                                                   |
|   XOR   | XORs the bits of V and R                                             |
|   SFT   | Shifts the the bits of R left by V. Negative values of V shift right |

| Conditional |                                                                                               |
|-------------|-----------------------------------------------------------------------------------------------|
|     MPS     | Move positive - The instruction pointer will move in the specified direction if R is positive |
|     MNG     | Move Negative - The instruction pointer will move in the specified direction if R is negative |
|     MZR     | Move Zero - The instruction pointer will move in the specified direction if R is zero         |

| I/O |                                         |
|-----|-----------------------------------------|
| RED | Reads one byte from standard in to R    | 
| WRT | Writes one byte from R to standard out  |
| END | Terminates execution of the program     |


**Instruction Values**
| Instr | Dec  | Hex  | 
|-------------|------|------|
|     SAV     |  00  |  00  |
|     LOD     |  01  |  01  |
|     ADD     |  02  |  02  |
|     SUB     |  03  |  03  |
|     INC     |  04  |  04  |
|     DEC     |  05  |  05  |
|     MUL     |  06  |  06  |
|     DIV     |  07  |  07  |
|     MOD     |  08  |  08  |
|     AND     |   9  |  09  |
|     ORR     |  10  |  0A  |
|     NOT     |  11  |  0B  |
|     XOR     |  12  |  0C  |
|     SFT     |  13  |  0D  |
|     MPS     |  14  |  0E  |
|     MNG     |  15  |  0F  |
|     MZR     |  16  |  10  |
|     RED     |  17  |  11  | 
|     WRT     |  18  |  12  |
|     END     |  19  |  13  |

**Assembler Format**

Values can also be written in a more readable format that will be interpreted at runtime. This format consists of 5 or 6 characters in the  format:
| Conditional Move (optional) | Instruction | Data Move | Instruction Move |

Instructions are written as their three-character representation as shown in the table above.

Move directions are represented by the characters x, X, y, Y, z, Z, and _. These characters represent the axis of motion and the case represents the direction of motion. '_' represents no movement. Uppercase is positive.


