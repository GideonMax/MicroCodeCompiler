#include "Function.h"
#include "Utils.h"
#include <algorithm>


Function::Function(){}


Function::Function(std::string& Content, MicroCodeDescriptor& Descriptor) {

	//get the body and parse it
	size_t a = Content.find('{');
	size_t b = Content.find_last_of('}');
	std::string body = Content.substr(a + 1ULL, b - a - 1ULL);
	ParseBody(body, Descriptor, Instructions);

	size_t indexOfColon = Content.find(':');
	if (indexOfColon == -1) {//if there is no colon, it is fetch
		isFetch = true;
	}
	else {
		//get the Opcode of the function
		std::string Value = Content.substr(indexOfColon + 1ULL, a - indexOfColon - 1);
		Opcode = Utils::ParseConstantExpression(Value);
	}
}


void Function::ParseBody(std::string& body, MicroCodeDescriptor& Descriptor, std::vector<Instruction>& Out) {
	CodeTreeNode Node(body, CodeTreeNode::PartType::PlainCode, "");
	ParseBody(Node, Descriptor, Out);
}


void Function::ParseBody(CodeTreeNode& Code, MicroCodeDescriptor& Descriptor, std::vector<Instruction>& Out) {
	if (Code.MultiPart) {
		//go over every node and parse it
		std::vector<CodeTreeNode>& Nodes = Code.Nodes;
		for (size_t i = 0; i < Nodes.size(); i++) {
			switch (Nodes[i].Type)
			{
			case CodeTreeNode::PartType::PlainCode:
				ParseBody(Nodes[i], Descriptor, Out);
				break;
			//we shouldn't see any else statement because if we encounter an if,
			//it will skip the else statement after it (if it exists)
			case CodeTreeNode::PartType::Else:
				Utils::Error("unexpected else statment");
				break;
			case CodeTreeNode::PartType::If:
				std::vector<Instruction> ifBody;
				ParseBody(Nodes[i], Descriptor, ifBody);
				std::string& condition = Nodes[i].IfCondition;

				std::vector<Instruction> elseBody;
				if (i + 1ULL < Nodes.size()) {
					if (Nodes[i + 1ULL].Type == CodeTreeNode::PartType::Else) {
						i++;//make sure to skip the else so on the next iteration there won't be a lone else statement
						ParseBody(Nodes[i], Descriptor, elseBody);
					}
				}
				Instruction a(condition, Descriptor, ifBody, elseBody);
				Out.push_back(a);
				break;
			}
		}
		return;
	}
	//go over every line and parse it
	std::stringstream stream(Code.Code);
	std::string line;
	while (std::getline(stream, line)) {
		if (line.size() == 0)continue;
		if (Utils::AllNewLines(line))continue;

		// semicolons only appear at the end of a line
		bool Next = line[line.size() - 1] == ';';
		// remove the semicolon at the end of the line, since we are resizing to a smaller string, this has complexity O(1)
		if (Next)line.resize(line.size() - 1);

		std::vector<std::string> lineInstructions = Utils::Split(line, '|');
		for (std::string instr : lineInstructions) Out.push_back(Instruction(instr, Descriptor));

		
		if (Next) {
			Instruction a;
			a.type = Instruction::InstructionType::Next;
			Out.push_back(a);
		}
	}
	return;
}