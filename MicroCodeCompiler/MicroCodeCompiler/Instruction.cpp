#include "Instruction.h"
#include <algorithm>

Instruction::Instruction(){}
Instruction::Instruction(std::string& condition, MicroCodeDescriptor& descriptor, std::vector<Instruction> ifBody, std::vector<Instruction> elseBody)
	:Condition(Expression(condition, descriptor)),
	type(InstructionType::Conditional),
	IfBody(ifBody),
	ElseBody(elseBody),
	Eeprom(-1) {}

Instruction::Instruction(std::string instruction, MicroCodeDescriptor& descriptor) {
	type = InstructionType::Set;
	size_t eq = instruction.find('=');
	if (eq == -1) {
		Set_A = Expression(instruction, descriptor);
	}
	else {
		std::string first = instruction.substr(0, eq);
		std::string second = instruction.substr(eq + 1ULL, instruction.size() - eq - 1);
		Set_A = Expression(first, descriptor);
		Set_B = Expression(second, descriptor);
	}
	Eeprom = Set_A.EEprom;
	return;
}