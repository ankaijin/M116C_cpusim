#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>
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
    public:
        Controller();
        CPU* cpu;
        int jalr;
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
        // aluSrcMux (selects between register value and immediate for ALU input)
        int aluSrcMux(int rs2, int immValue);
        // memToRegMux (selects between ALU result and memory data for write back)
        int32_t memToRegMux(int32_t aluResult, int32_t memData);
        // branchMux (selects next PC from PC+4 or PC+immediate)
        unsigned long branchMux(int32_t aluResult, int32_t immValue);
        // pcToRegMux (selects between ALU result/memory and PC+4 for write back to rd)
        int32_t pcToRegMux(int32_t dataToWrite, unsigned long pcIncr4);
        // aluToPCMux (selects between ALU result and PC + offset for next PC)
        unsigned long aluToPCMux(int32_t aluResult, unsigned long pcAndOffset);
};

// ALU Controller class
// should send 4-bit ALU control signal
// gets passed in funct3, funct7, and aluOp from main controller
class ALUController {
    private:
        friend class CPU;
    public:
        ALUController();
        CPU* cpu;
        unsigned int ALUCtrlSig;
        // note: funct7 not needed for supported instructions
        unsigned int setALUCtrlSig(int funct3, int aluOp);
};

class CPU {
    private:
        // DATAPATH COMPONENTS
        unordered_map<uint32_t, uint8_t> dataMemory; // data memory, map address to byte (little-endian)
        vector<uint32_t> iMemory; // instruction memory in binary
        int32_t registerFile[32]; // 32 registers, 32 bits each, don't write to 0th element
        unsigned long PC; // pc

    public:
        // Constructor that initializes instruction memory and PC
        CPU(vector<uint32_t> iMemory);

        // CPU FUNCTIONS
        unsigned long readPC();
        void setPC(unsigned long programCount);
        // fetch instruction (from iMemory)
        uint32_t fetchInstruction();
        // Immediate Generator function (takes 32-bit instruction as input)
        int32_t immGen(instruction instr);
        // read data from rs1 and rs2
        int32_t rs1data(unsigned int rs1);
        int32_t rs2data(unsigned int rs2);
        // ALU operation (takes in two operands and ALU control signal)
        int32_t ALUOperation(int32_t operand1, int32_t operand2, unsigned int aluControl);
        // memory access (determined by MemWrite and MemRead signal)
        int32_t readDataMem(uint32_t address, int funct3);
        void writeDataMem(uint32_t address, int32_t data, int funct3);
        // write back output of memToRegMux
        int32_t writeBackToReg(unsigned int rd, int32_t write);

        // return values
        int32_t a0val();
        int32_t a1val();
};

// add other functions and objects here
