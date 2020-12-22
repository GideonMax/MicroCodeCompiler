#pragma once
#include <string>
#include <sstream>
#include <vector>
#include "MicroCodeDescriptor.h"
#include "CodeTreeNode.h"
#include "Instruction.h"

/// <summary>
/// this class parses a function
/// </summary>
class Function {
public:

	bool isFetch = false;
	//this is the opcode of the instruction (Address part: "instruction")
	uint64_t Opcode = -1;
	// default CTOR, does nothing
	Function();
	/// <summary>
	/// parses the function
	/// </summary>
	/// <param name="Content">the code that is the function</param>
	Function(std::string& Content, MicroCodeDescriptor& Descriptor);
	std::vector<Instruction> Instructions;
private:
	//functions the class uses internally to parse the body
	void ParseBody(std::string& body, MicroCodeDescriptor& Descriptor, std::vector<Instruction>& Out);
	static void ParseBody(CodeTreeNode& Code, MicroCodeDescriptor& Descriptor, std::vector<Instruction>& Out);
};