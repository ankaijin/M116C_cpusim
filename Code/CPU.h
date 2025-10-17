#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <cstdint>
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

// instruction union
union instruction {
    struct instr_r r;
    struct instr_i i;
    struct instr_s s;
    struct instr_b b;
    struct instr_u u;
    struct instr_j j;

    int opcode    : 7;
    uint32_t mcode;
};

class CPU; // forward declaration

// Controller struct
class Controller {
    friend class CPU;
    Controller();
public:
    CPU* cpu;
    int branch;
    int memRead;
    int memToReg;
    int aluOp;
    int memWrite;
    int aluSrc;
    int regWrite;
    // int pcSrc;

    string getInstrType(instruction instr);
    void setControlSignals(string instrType); // set control signals based on opcode
};

class CPU {
private:
    // DATAPATH COMPONENTS
	int dMemory[4096]; // data memory byte addressable in little endian fashion;
    vector<uint32_t> iMemory; // instruction memory in binary
	const static int x0 = 0; // $0 is hardwired to 0
	int registerFile[32]; // 31 registers, 32 bits each, don't use 0th element
	unsigned long PC; // pc

    // ALU CONTROLLER COMPONENTS
public:
    // Constructor that initializes instruction memory and PC
    CPU(vector<uint32_t> iMemory);
    Controller controller;
	unsigned long readPC();
	void incPC();
	// fetch instruction
    uint32_t fetchInstruction();
	// decode instruction (Controller)

	// execute instruction
	// memory access
	// write back
	// update PC
};

// add other functions and objects here
