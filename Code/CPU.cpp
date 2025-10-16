#include "CPU.h"

Instruction::Instruction(bitset <32> fetch)
{
	instr = fetch; // initialize the instruction
}

CPU::CPU()
{
	PC = 0; //set PC to 0
	for (int i = 0; i < 4096; i++) // copy instrMEM
	{
		dmemory[i] = (0);
	}
	for (int i = 0; i < 32; i++) // initialize registers to 0
	{
		registerFile[i] = 0;
	}
	// initialize all values to an invalid value
	rs1 = -1;
	rs2 = -1;
	rd = -1;
	imm = -1;
}

unsigned long CPU::readPC()
{
	return PC;
}

void CPU::incPC()
{
	PC++;
}

// Add other functions here ... 