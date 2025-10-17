#include "CPU.h"

Controller::Controller() : cpu(nullptr), branch(0), memRead(0), memToReg(0),
        aluOp(0), memWrite(0), aluSrc(0), regWrite(0) {}

string Controller::getInstrType(instruction instr) {
	switch (instr.opcode) {
		case 0b0110011: return "R"; // R-type
		case 0b0010011: return "I"; // I-type
		case 0b0000011: return "Load"; // Load
		case 0b0100011: return "S"; // S-type
		case 0b1100011: return "B"; // B-type
		case 0b0110111: return "U"; // U-type
		case 0b1101111: return "J"; // J-type
		default: return "X"; // Unknown
	}
}

void Controller::setControlSignals(string instrType) {
	if (instrType == "R") {
		aluSrc = 0;
		regWrite = 1;
		memRead = 0;
		memWrite = 0;
		memToReg = 0;
		branch = 0;
		aluOp = 2; // ALU operation determined by funct3 and funct7 fields
	}
	else if (instrType == "I") {
		aluSrc = 1;
		regWrite = 1;
		memRead = 0;
		memWrite = 0;
		memToReg = 0;
		branch = 0;
		aluOp = 0; // ALU operation determined by funct3
	}
	else if (instrType == "Load") {
		aluSrc = 1;
		regWrite = 1;
		memRead = 1;
		memWrite = 0;
		memToReg = 1;
		branch = 0;
		aluOp = 0; // ALU does addition for address calculation
	}
	else if (instrType == "S") {
		aluSrc = 1;
		regWrite = 0;
		memRead = 0;
		memWrite = 1;
		memToReg = 0; // don't care
		branch = 0;
		aluOp = 0; // ALU does addition for address calculation
	}
	else if (instrType == "B") {
		aluSrc = 0;
		regWrite = 0;
		memRead = 0;
		memWrite = 0;
		memToReg = 0; // don't care
		branch = 1;
		aluOp = 1; // ALU does subtraction for comparison
	}
	else {
		// For U-type and J-type, control signals can be set as needed
	}
}

CPU::CPU(vector<uint32_t> iMem) : iMemory(iMem), PC(0)
{
	controller = Controller();
	controller.cpu = this;
	for (int i = 0; i < 4096; i++) // copy instrMEM
	{
		dMemory[i] = (0);
	}
	for (int i = 0; i < 32; i++) // initialize registers to 0
	{
		registerFile[i] = 0;
	}
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
uint32_t CPU::fetchInstruction()
{
	if (PC < iMemory.size()) return iMemory[PC];
	else return -1;
}