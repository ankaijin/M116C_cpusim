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
    unsigned int opcode    : 7;
    unsigned int rd        : 5;
    unsigned int funct3    : 3;
    unsigned int rs1       : 5;
    unsigned int rs2       : 5;
    unsigned int funct7    : 7;
};

struct instr_i {
    unsigned int opcode    : 7;
    unsigned int rd        : 5;
    unsigned int funct3    : 3;
    unsigned int rs1       : 5;
    unsigned int imm       : 12;
};

struct instr_s {
    unsigned int opcode    : 7;
    unsigned int imm4_0    : 5;
    unsigned int funct3    : 3;
    unsigned int rs1       : 5;
    unsigned int rs2       : 5;
    unsigned int imm11_5   : 7;
};

struct instr_b {
    unsigned int opcode    : 7;
    unsigned int imm11     : 1;
    unsigned int imm4_1    : 4;
    unsigned int funct3    : 3;
    unsigned int rs1       : 5;
    unsigned int rs2       : 5;
    unsigned int imm10_5   : 6;
    unsigned int imm12     : 1;
};

struct instr_u {
    unsigned int opcode    : 7;
    unsigned int rd        : 5;
    unsigned int imm31_12  : 20;
};

struct instr_j {
    unsigned int opcode    : 7;
    unsigned int rd        : 5;
    unsigned int imm19_12  : 8;
    unsigned int imm11     : 1;
    unsigned int imm10_1   : 10;
    unsigned int imm20     : 1;
};

// instruction union
union instruction {
    struct instr_r r;
    struct instr_i i;
    struct instr_s s;
    struct instr_b b;
    struct instr_u u;
    struct instr_j j;

    unsigned int opcode    : 7;
    uint32_t mcode;
};

class CPU; // forward declaration

// Controller class
class Controller {
    private:    
        friend class CPU;
        Controller();
    public:
        CPU* cpu;
        int branch;
        int memRead;
        int memToReg;
        int aluOp;  // 3-bit control signal
        int memWrite;
        int aluSrc;
        int regWrite;
        // int pcSrc;

        string getInstrType(instruction instr);
        void setControlSignals(string instrType); // set control signals based on opcode
};

// ALU Controller class
// should send 4-bit ALU control signal
// gets passed in funct3, funct7, and aluOp from main controller
class ALUController {
    private:
        friend class CPU;
        ALUController();
    public:
        CPU* cpu;
        int ALUCtrlSig;
        int setALUCtrlSig(int funct3, int funct7, int aluOp);
};

class CPU {
    private:
        // DATAPATH COMPONENTS
        int dMemory[4096]; // data memory byte addressable in little endian fashion;
        vector<uint32_t> iMemory; // instruction memory in binary
        int registerFile[32]; // 32 registers, 32 bits each, don't write to 0th element
        unsigned long PC; // pc

    public:
        // Constructor that initializes instruction memory and PC
        CPU(vector<uint32_t> iMemory);
        // Constructor for controller
        Controller controller;
        ALUController aluController;

        // CPU FUNCTIONS
        unsigned long readPC();
        void incPC();
        // fetch instruction (from iMemory)
        uint32_t fetchInstruction();
        // Immediate Generator function (takes 32-bit instruction as input)
        int32_t immGen(instruction instr);
        // read data from rs1 and rs2
        int rs1data(unsigned int rs1);
        int rs2data(unsigned int rs2);
        // aluSrcMux (selects between register value and immediate for ALU input)
        int aluSrcMux(int rs2, int immValue);   // store in main as ALU input 2
        // ALU operation (takes in two operands and ALU control signal)
        int ALUOperation(int operand1, int operand2, int aluControl);
        // memory access (determined by MemWrite signal)
        // memToRegMux (selects between ALU result and memory data for write back)
        // branchMux (selects next PC based on branch decision)
};

// add other functions and objects here
