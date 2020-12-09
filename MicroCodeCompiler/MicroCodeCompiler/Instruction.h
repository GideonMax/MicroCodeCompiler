#pragma once
#include <string>
#include <vector>
#include "MicroCodeDescriptor.h"
#include "Expression.h"

struct Instruction {
	int Eeprom;

	enum class InstructionType {
		Set,
		Conditional,
		Next//End signifies that the function has ended, the rest of the function gets padded with zeros
	};//imprtant, "Next" will only be set by the parent.
	Instruction::InstructionType type;

	//Set instruction
	Expression Set_A =Expression();//the location of the output word at which you should set
	Expression Set_B =Expression();//the value to set

	Expression Condition=Expression();
	std::vector<Instruction> IfBody;
	std::vector<Instruction> ElseBody;

	Instruction();//default CTOR for convenience

	//CTOR for conditionals
	Instruction(std::string& condition, MicroCodeDescriptor& descriptor, std::vector<Instruction> ifBody, std::vector<Instruction> elseBody);

	Instruction(std::string instruction, MicroCodeDescriptor& descriptor);
};
