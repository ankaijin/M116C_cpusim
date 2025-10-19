#include "CPU.h"

#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

/*
Add all the required standard and developed libraries here
*/
#include <vector>
#include <bitset>

/*
Put/Define any helper function/definitions you need here
*/
int main(int argc, char* argv[])
{
	/* This is the front end of your project.
	You need to first read the instructions that are stored in a file and load them into an instruction memory.
	*/

	/* Each cell should store 1 byte. You can define the memory either dynamically, or define it as a fixed size with size 4KB (i.e., 4096 lines). Each instruction is 32 bits (i.e., 4 lines, saved in little-endian mode).
	Each line in the input file is stored as an hex and is 1 byte (each four lines are one instruction). You need to read the file line by line and store it into the memory. You may need a mechanism to convert these values to bits so that you can read opcodes, operands, etc.
	*/

	vector<string> instMem; // instruction memory in hex string
	vector<uint32_t> instMemBin; // instruction memory in binary

	if (argc < 2) {
		//cout << "No file name entered. Exiting...";
		return -1;
	}

	ifstream infile(argv[1]); //open the file
	if (!(infile.is_open() && infile.good())) {
		cout<<"error opening file\n";
		return 0; 
	}

	string line;
	int i = 0;
	while (infile) {
		// read file instructions are in little endian format
		// reverse little endian to big endian
		while (true) {
			string b0, b1, b2, b3;        // b0 = least significant byte on disk
			if (!(infile >> b0 >> b1 >> b2 >> b3)) break;  // stop cleanly at EOF/short read
			instMem.push_back(b3 + b2 + b1 + b0);          // big-endian string
			cout << instMem.back() << '\n';
			i += 1;
		}
	}
	int maxPC = i;
	cout << endl;

	for (int i = 0; i < maxPC; i++) {
		stringstream ss;
		uint32_t binary;
		
		// convert hex to binary
		ss << hex << instMem[i];
		ss >> binary;
		bitset<32> tempBin(binary);

		binary = static_cast<uint32_t>(tempBin.to_ulong());
		// save binary back to instMem
		instMemBin.push_back(binary);
	}

	/* Instantiate your CPU object here.  CPU class is the main class in this project that defines different components of the processor.
	CPU class also has different functions for each stage (e.g., fetching an instruction, decoding, etc.).
	*/

	CPU myCPU(instMemBin);  // call the approriate constructor here to initialize the processor... 
	Controller controller;
	ALUController aluController; 
	controller.cpu = &myCPU;
	aluController.cpu = &myCPU;

	// make sure to create a variable for PC and resets it to zero (e.g., unsigned int PC = 0);

	/* OPTIONAL: Instantiate your Instruction object here. */
	instruction currInst;
	
	bool done = true;
	while (done == true) // processor's main loop. Each iteration is equal to one clock cycle.  
	{
		// fetch
		currInst.mcode = myCPU.fetchInstruction();
		if (currInst.mcode == 0) {
			break; // halt on all zeros instruction
		}

		// decode
		cout << "==========================" << endl;
		cout << "PC: " << myCPU.readPC() << " Instruction: " << bitset<32>(currInst.mcode) << endl;
		controller.setControlSignals(controller.getInstrType(currInst));

		// execute
		cout << "Immediate: " << myCPU.immGen(currInst) << endl;
		int32_t aluInput1 = myCPU.rs1data(currInst.r.rs1);
		int32_t aluInput2 = controller.aluSrcMux(currInst.r.rs2, myCPU.immGen(currInst));
		cout << "Rs1 num: " << currInst.r.rs1 << endl;
		cout << "ALU Input 1: " << aluInput1 << endl;
		cout << "Rs2 num: " << currInst.r.rs2 << endl;
		cout << "ALU Input 2: " << aluInput2 << endl;

		unsigned int aluCtrl5bit = aluController.setALUCtrlSig(currInst.r.funct3, controller.aluOp);
		cout << "ALU Control Signal: " << bitset<5>(aluCtrl5bit) << endl;

		int32_t aluResult = myCPU.ALUOperation(aluInput1, aluInput2, aluCtrl5bit);
		cout << "ALU Result: " << aluResult << endl;

		// memory access (Load and Store) NEED TO TEST
		int32_t memData = 0;
		if (controller.memRead == 1) {
			memData = myCPU.readDataMem(static_cast<uint32_t>(aluResult), currInst.r.funct3);
			cout << "Memory Read Data: " << memData << endl;
		}
		if (controller.memWrite == 1) {
			myCPU.writeDataMem(static_cast<uint32_t>(aluResult), myCPU.rs2data(currInst.r.rs2), currInst.r.funct3);
			cout << "Memory Write at Address: " << aluResult << " Data: " << myCPU.rs2data(currInst.r.rs2) << endl;
		}

		// write back (if mem not read, aluResult is selected anyway)
		int32_t writeData = controller.memToRegMux(aluResult, memData);
		if (controller.regWrite == 1) {
			int written = myCPU.writeBackToReg(currInst.r.rd, writeData);
			cout << "Write Back to Register: x" << currInst.r.rd << " Data: " << writeData << endl;
		}

		// default PC += 4
		myCPU.incPC();
		if (myCPU.readPC() >= maxPC)	// changed from > to >=
			break;
	}
	int a0 = myCPU.a0val();
	int a1 = myCPU.a1val();
	// print the results (you should replace a0 and a1 with your own variables that point to a0 and a1)
	cout << "(" << a0 << "," << a1 << ")" << endl;
	
	return 0;

}