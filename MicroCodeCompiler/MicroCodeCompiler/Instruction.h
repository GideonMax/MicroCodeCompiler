#pragma once
#include <string>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>
#include "Utils.h"
#include "MicroCodeDescriptor.h"
#include "CodeTreePart.h"
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

	Instruction(){}//default CTOR for convenience

	//CTOR for conditionals
	Instruction(std::string& condition, MicroCodeDescriptor& descriptor, std::vector<Instruction> ifBody, std::vector<Instruction> elseBody)
		:Condition( Expression(condition, descriptor)),
		type(InstructionType::Conditional),
		IfBody(ifBody),
		ElseBody(elseBody){}

	Instruction(std::string instruction, MicroCodeDescriptor& descriptor) {
		type = InstructionType::Set;
		size_t eq = instruction.find('=');
		if (eq == -1) {
			Set_A = Expression(instruction, descriptor);
		}
		else {
			std::string first = instruction.substr(0, eq);
			std::string second = instruction.substr(eq+1ULL, instruction.size()-eq-1);
			Set_A = Expression(first, descriptor);
			Set_B = Expression(second, descriptor);
		}
		Eeprom = Set_A.EEprom;
		return;
	}
};
