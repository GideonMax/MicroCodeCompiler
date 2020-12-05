#pragma once
#include <string>
#include <algorithm>
#include <sstream>
#include <map>
#include <vector>
#include "Utils.h"
#include "MicroCodeDescriptor.h"
#include "CodeTreePart.h"
#include "Instruction.h"

/// <summary>
/// this class represents a microcode function, of course.
/// I still have to make changes to make it work with multiple eeproms and also make changes to make it work at all.
/// </summary>
class Function {
public:
	bool isFetch = false;
	uint64_t Alias = -1;//this is the value of the function address part
	Function() {}
	Function(std::string Content, MicroCodeDescriptor& Descriptor) {

		//get the body and parse it
		size_t a = Content.find('{');
		size_t b = Content.find_last_of('}');
		std::string body = Content.substr(a + 1ULL, b - a - 1ULL);
		ParseBody(body, Descriptor, Instructions);

		//get the Alias of the function
		size_t indexOfColon = Content.find(':');
		if (indexOfColon == -1) {//if there is no colon, it is fetch
			isFetch = true;
		}
		else {
			std::string Value = Content.substr(indexOfColon + 1ULL, a - indexOfColon - 1);
			Alias = Utils::ParseConstantExpression(Value);
		}
	}
	std::vector<Instruction> Instructions;
private:
	void ParseBody(std::string body, MicroCodeDescriptor& Descriptor, std::vector<Instruction>& Out) {
		CodeTreePart Part(body, CodeTreePart::PartType::PlainCode, "");
		ParseBody(Part, Descriptor, Out);
	}
	void ParseBody(CodeTreePart& Code, MicroCodeDescriptor& Descriptor, std::vector<Instruction>& Out) {
		if (Code.MultiPart) {
			std::vector<CodeTreePart>& Parts = Code.Parts;
			for (size_t i = 0; i < Parts.size(); i++) {
				switch (Parts[i].Type)
				{
				case CodeTreePart::PartType::PlainCode:
					ParseBody(Parts[i], Descriptor, Out);
					break;
#ifdef _DEBUG
				//we shouldn't see any else statement because if we encounter an if, it will skip the else statement after it (if it exists)
				case CodeTreePart::PartType::Else:
					Utils::Error("unexpected else statment");
					break;
#endif // _DEBUG
				case CodeTreePart::PartType::If:
					std::vector<Instruction> ifBody;
					ParseBody(Parts[i], Descriptor, ifBody);
					std::string& condition = Parts[i].IfCondition;

					std::vector<Instruction> elseBody;
					if (i + 1ULL < Parts.size()) {
						if (Parts[i + 1ULL].Type == CodeTreePart::PartType::Else) {
							i++;//make sure to skip the else so on the next iteration there won't be a lone else statement
							ParseBody(Parts[i], Descriptor, elseBody);
						}
					}
					Instruction a(condition,Descriptor,ifBody,elseBody);
					Out.push_back(a);
					break;
				}
			}
			return;
		}
		std::stringstream stream(Code.Code);
		std::string line;
		while (std::getline(stream, line)) {
			if (line.size() == 0)continue;
			bool Next = line[line.size() - 1] == ';';
			//remove the semicolon at the end of the line, since we are resizing to a smaller string, this has complexity O(1)
			if (Utils::AllNewLines(line))continue;
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
};