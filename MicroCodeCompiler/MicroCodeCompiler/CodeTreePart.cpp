#include "CodeTreePart.h"
#include "Utils.h"

CodeTreePart::CodeTreePart(std::string content, PartType type = PartType::PlainCode, std::string condition = "") {
	Type = type;
	if (Type == PartType::If) {
		IfCondition = condition;
	}
	//look for any if statements, if there are none, this must be a non-multi-part part
	int index = content.find("if(");
	if (index == -1) {
		Code = content;
		MultiPart = false;
		return;
	}
	MultiPart = true;

	//this part goes over the content, splits it into multiple parts, decides the type of each part and gets it's content and sends that down to another CodeTreePart to parse
	int level = 0;//level goes up and down according to the brackets, this is so we could capture matching brackets
	PartType currentType = PartType::PlainCode;
	std::string currentPart = "";//the content of the part we are currently on
	std::string currentCondition = "";//the condition for an if statement
	bool inCondition = false;//are we currently between the () of an if statement
	for (size_t i = 0; i < content.size(); i++) {
		if (content[i] == '}') {
			level--;
			//if the level is now 0 that means thatwe just finished a part (the inside of an if or else statement
			if (level == 0) {
				CodeTreePart p(currentPart, currentType, currentCondition);
				if (!(!p.MultiPart && Utils::AllNewLines(p.Code)))//checks if the part is not multipart and has only new lines, in that case, it is empty
					Parts.push_back(p);
				currentType = PartType::PlainCode;
				currentCondition = "";
				currentPart = "";
			}
			else {
				currentPart += '}';
			}
			continue;
		}

		if (content[i] == '{') {
			level++;
			if (level == 1)continue;//if the level is one it means that the level was just 0 so this is the start of a part and we should not record the {
		}

		if (level > 0) {
			currentPart += content[i];
			continue;
		}

		if (content[i] == 'i' && content[i + 1ULL] == 'f') {
			CodeTreePart p(currentPart, currentType, currentCondition);
			if (!(!p.MultiPart && Utils::AllNewLines(p.Code)))
				Parts.push_back(p);
			currentType = PartType::If;
			currentCondition = "";
			currentPart = "";
			i++;
			continue;
		}
		if (content[i] == '(') {
			inCondition = true;
			continue;
		}
		if (content[i] == ')') {
			inCondition = false;
			continue;
		}
		if (inCondition) {
			currentCondition += content[i];
			continue;
		}

		if (content[i] == 'e' && content[i + 1ULL] == 'l' && content[i + 2ULL] == 's' && content[i + 3ULL] == 'e') {
			CodeTreePart p(currentPart, currentType, currentCondition);
			if (!(!p.MultiPart && Utils::AllNewLines(p.Code)))
				Parts.push_back(p);
			currentType = PartType::Else;
			currentCondition = "";
			currentPart = "";
			i += 3;
			continue;
		}
		currentPart += content[i];
	}
	if (currentPart.size() > 0) {
		CodeTreePart p(currentPart, currentType, currentCondition);
		if (!(!p.MultiPart && Utils::AllNewLines(p.Code)))
			Parts.push_back(p);
		currentType = PartType::Else;
		currentCondition = "";
		currentPart = "";
	}
	return;
}