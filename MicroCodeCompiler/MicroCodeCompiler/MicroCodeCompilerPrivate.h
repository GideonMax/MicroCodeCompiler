#pragma once

#include <functional>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>
#include "Function.h"
#include "MicroCodeDescriptor.h"



namespace MicroCodeCompiler {

	class Runtime {
	public:
		uint64_t EepromCount;
		Runtime(std::map<uint64_t, Function> code, MicroCodeDescriptor descriptor);
		/// <summary>
		/// executes all the code for a given eeprom.
		/// </summary>
		size_t Run(int Eeprom, uint64_t*& Out);
	private:
		/// <summary>
		/// runs a single vector of instructions (function, if body or else body)
		/// </summary>
		/// <param name="Eeprom">- The EEprom Currently being computed</param>
		/// <param name="Out">- The output array which the function modifies</param>
		/// <param name="code">- The code to execute</param>
		/// <param name="Address">- The current address, as there might be "Next" instructions in a conditional body which affect the addresses after said body, Address needs to be a reference</param>
		/// <param name="increment">- The amount to increment by at each step</param>
		static void Run(int Eeprom, uint64_t* Out, std::vector<Instruction>& code, uint64_t& Address, uint64_t increment);
		uint64_t MakeRoomForStep(uint64_t Address);
		MicroCodeDescriptor Descriptor;
		Function& Fetch;
		MicroCodeDescriptor::AddressWordPart& instruction;
		MicroCodeDescriptor::AddressWordPart& Step;
		std::map<uint64_t, Function> Code;
	};
	Runtime Compile(char* FilePath);
	void Write(Runtime& r, char* FilePath);
}
