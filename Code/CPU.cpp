#include "CPU.h"

Controller::Controller() : cpu(nullptr), branch(0), memRead(0), memToReg(0),
        aluOp(0), memWrite(0), aluSrc(0), regWrite(0) {}

ALUController::ALUController() : cpu(nullptr), ALUCtrlSig(-1) {}

string Controller::getInstrType(instruction instr) {
	switch (instr.opcode) {
		case 0b0110011: return "R"; // R-type
		case 0b0010011: return "I"; // I-type
		case 0b0000011: return "Load"; // Load
		case 0b0100011: return "S"; // S-type
		case 0b1100011: return "B"; // B-type
		case 0b0110111: return "U"; // U-type
		case 0b1101111: return "J"; // J-type
		default: return "Invalid"; // Invalid opcode
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
		aluOp = 3; // ALU operation determined by funct3 ??
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
	else if (instrType == "U") {	// LUI only
		aluSrc = 1;
		regWrite = 1;
		memRead = 0;
		memWrite = 0;
		memToReg = 0;
		branch = 0;
		aluOp = 4; // ALU does left shift for address calculation
	}
	// do nothing for invalid instruction type
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

int32_t CPU::immGen(instruction instr) {
	uint32_t immediate = 0;
	switch (instr.opcode) {
		case 0b0010011: // I-type
		case 0b0000011: // Load
		case 0b1100111: // JALR
			immediate = (instr.i.imm);
			// Sign-extend
			if (immediate & 0x800) immediate |= 0xFFFFF000;
			break;
		case 0b0100011: // S-type
			immediate = (instr.s.imm11_5 << 5) | (instr.s.imm4_0);
			// Sign-extend
			if (immediate & 0x800) immediate |= 0xFFFFF000;
			break;
		case 0b1100011: // B-type
			immediate = (instr.b.imm12 << 12) | (instr.b.imm10_5 << 5) |
				  (instr.b.imm4_1 << 1) | (instr.b.imm11 << 11);	// seems legit
			// Sign-extend
			if (immediate & 0x1000) immediate |= 0xFFFFE000;
			break;
		case 0b0110111: // U-type
			immediate = (instr.u.imm31_12 << 12);
			break;
		default:
			break;
	}
	return static_cast<int32_t>(immediate);
}

int CPU::rs1data(unsigned int rs1) {
	return registerFile[rs1];
}

int CPU::rs2data(unsigned int rs2) {
	return registerFile[rs2];
}

int CPU::aluSrcMux(int rs2, int immValue) {
	if (controller.aluSrc == 0) return rs2data(rs2);
	else return immValue;
}