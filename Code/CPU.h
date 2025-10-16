#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;


class Instruction { // optional
public:
	bitset<32> instr; // instruction
	Instruction(bitset <32> fetch = bitset<32>(0)); // constructor to initialize the instruction
};

class CPU {
private:
	int dmemory[4096]; // data memory byte addressable in little endian fashion;
	const static int x0 = 0; // $0 is hardwired to 0
	int registerFile[32]; // 31 registers, 32 bits each, don't use 0th element
	// register module
	unsigned int rs1;
	unsigned int rs2;
	unsigned int rd;
	unsigned int imm;
	unsigned long PC; // pc 

public:
	CPU();
	unsigned long readPC();
	void incPC();
	// fetch instruction
	// decode instruction
	// execute instruction
	// memory access
	// write back
	// update PC
};

// add other functions and objects here
