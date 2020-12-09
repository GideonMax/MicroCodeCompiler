#pragma once
#include <string>
#include <sstream>
#include <vector>
#include "MicroCodeDescriptor.h"
#include "CodeTreePart.h"
#include "Instruction.h"

/// <summary>
/// this class parses a function
/// </summary>
class Function {
public:
	bool isFetch = false;
	uint64_t Alias = -1;//this is the value of the function address part
	Function();
	Function(std::string& Content, MicroCodeDescriptor& Descriptor);
	std::vector<Instruction> Instructions;
private:
	void ParseBody(std::string& body, MicroCodeDescriptor& Descriptor, std::vector<Instruction>& Out);
	static void ParseBody(CodeTreePart& Code, MicroCodeDescriptor& Descriptor, std::vector<Instruction>& Out);
};