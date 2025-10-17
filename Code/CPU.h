#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

// Instruction types
struct instr_r {
    int opcode    : 7;
    int rd        : 5;
    int funct3    : 3;
    int rs1       : 5;
    int rs2       : 5;
    int funct7    : 7;
};

struct instr_i {
    int opcode    : 7;
    int rd        : 5;
    int funct3    : 3;
    int rs1       : 5;
    int imm       : 12;
};

struct instr_s {
    int opcode    : 7;
    int imm4_0    : 5;
    int funct3    : 3;
    int rs1       : 5;
    int rs2       : 5;
    int imm11_5   : 7;
};

struct instr_b {
    int opcode    : 7;
    int imm11     : 1;
    int imm4_1    : 4;
    int funct3    : 3;
    int rs1       : 5;
    int rs2       : 5;
    int imm10_5   : 6;
    int imm12     : 1;
};

struct instr_u {
    int opcode    : 7;
    int rd        : 5;
    int imm31_12  : 20;
};

struct instr_j {
    int opcode    : 7;
    int rd        : 5;
    int imm19_12  : 8;
    int imm11     : 1;
    int imm10_1   : 10;
    int imm20     : 1;
};

// Instruction union
union instruction {
    struct instr_r r;
    struct instr_i i;
    struct instr_s s;
    struct instr_b b;
    struct instr_u u;
    struct instr_j j;

    uint32_t mcode;
};

class CPU {
private:
	int dmemory[4096]; // data memory byte addressable in little endian fashion;
	const static int x0 = 0; // $0 is hardwired to 0
	int registerFile[32]; // 31 registers, 32 bits each, don't use 0th element
	unsigned long PC; // pc 

public:
	CPU();
	unsigned long readPC();
	void incPC();
	// fetch instruction
	// decode instruction (Controller)

	// execute instruction
	// memory access
	// write back
	// update PC
};

// add other functions and objects here
