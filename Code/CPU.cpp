#include "CPU.h"

Controller::Controller() : cpu(nullptr), branch(0), memRead(0), memToReg(0),
        aluOp(0), memWrite(0), aluSrc(0), regWrite(0) {}

string Controller::getInstrType(instruction instr) {
	switch (instr.opcode) {
		case 0b0110011: return "R"; // R-type
		case 0b0010011: return "I"; // I-type
		case 0b0000011: return "Load"; // Load
		case 0b1100111: return "JALR"; // JALR
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
	else if (instrType == "JALR") {
		aluSrc = 1;
		regWrite = 1;
		memRead = 0;
		memWrite = 0;
		memToReg = 0;
		branch = 1;	// branch is 1
		aluOp = 0; // use same aluOp as load and store instrs. (addition)
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
	else aluOp = -1;
}

int Controller::aluSrcMux(int rs2, int immValue) {
	if (aluSrc == 0) return cpu->rs2data(rs2);
	else return immValue;
}

ALUController::ALUController() : cpu(nullptr), ALUCtrlSig(-1) {}

unsigned int ALUController::setALUCtrlSig(int funct3, int aluOp) {
	// R-type instructions
	if (aluOp == 2) {
		switch (funct3) {
			case 0b000: return 0b01000; // ADD
			case 0b111: return 0b01001; // AND
			case 0b101: return 0b01010; // SRA
			default: return -1; // Invalid funct3 ??
		}
	}
	// I-type instructions
	else if (aluOp == 3) {
		switch (funct3) {
			case 0b000: return 0b01100; // ADDI
			case 0b110: return 0b01101; // ORI
			case 0b011: return 0b01110; // SLTIU
			default: return -1; // Invalid funct3 ??
		}
	}
	// Load/Store instructions
	else if (aluOp == 0) {
		return 0b00000; // ADD for address calculation
	}
	// Branch instructions
	else if (aluOp == 1) {
		return 0b00100; // SUB for comparison
	}
	// U-type instructions (LUI)
	else if (aluOp == 4) {
		return 0b10000; // LUI operation
	}
	return -1; // Invalid aluOp
}

CPU::CPU(vector<uint32_t> iMem) : iMemory(iMem), PC(0)
{
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
	uint32_t immediate = 0;	// does int or uint matter?
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

int32_t CPU::ALUOperation(int32_t operand1, int32_t operand2, unsigned int aluControl) {
	// names for each control signal for clarity
	const unsigned int ADD		= 0b00000; // load/store address add
	const unsigned int SUB      = 0b00100; // branch compare
	const unsigned int ADD_OP   = 0b01000; // R-type ADD
	const unsigned int AND_OP   = 0b01001; // R-type AND
	const unsigned int SRA_OP   = 0b01010; // R-type SRA
	const unsigned int ADDI     = 0b01100; // I-type ADDI
	const unsigned int ORI      = 0b01101; // I-type ORI
	const unsigned int SLTIU    = 0b01110; // I-type SLTIU (unsigned compare)
	const unsigned int LUI_OP   = 0b10000; // U-type LUI

	switch (aluControl) {
		case ADD:
		case ADD_OP:
		case ADDI:
			return operand1 + operand2;
		case SUB:
			return operand1 - operand2;
		case AND_OP:
			return operand1 & operand2;
		case ORI:
			return operand1 | operand2;
		case SRA_OP: {
			// Arithmetic right shift by lower 5 bits of operand2 (RISC-V shamt is 5 bits for RV32)
			int32_t a = operand1;
			unsigned int shamt = static_cast<unsigned int>(operand2) & 0x1F;
			return a >> shamt;
		}
		case SLTIU: {
			uint32_t ua = static_cast<uint32_t>(operand1);
			uint32_t ub = static_cast<uint32_t>(operand2);
			return (ua < ub) ? 1 : 0;
		}
		case LUI_OP:
			// LUI writes the immediate (operand2 should already be imm << 12); ignore operand1
			return operand2;
		default:
			// Unknown control; return 0 as a safe default ??
			return 0;
	}
}