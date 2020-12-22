#include "CodeTreeNode.h"
#include "Utils.h"

CodeTreeNode::CodeTreeNode(std::string content, PartType type, std::string condition):Type(type) {
	if (Type == PartType::If) {
		IfCondition = condition;
	}
	//look for any if statements, if there are none, this must be a non-multi-part part
	size_t index = content.find("if(");
	if (index == -1) {
		Code = content;//just set the content accordingly and call it a day
		MultiPart = false;
		return;
	}
	MultiPart = true;

	//this part goes over the content, splits it into multiple parts,
	//decides the type of each part, gets its content and then constructs another node and adds it to the Nodes vector
	int level = 0;//level describes the "elevation" in terms of brackets, i.e. it increments upon '{' and decrements upon '}'
	PartType currentType = PartType::PlainCode;
	std::string currentPart = "";//the content of the part we are currently on
	std::string currentCondition = "";//the condition for an if statement
	bool inCondition = false;//are we currently between the () of an if statement
	for (size_t i = 0; i < content.size(); i++) {
		if (content[i] == '}') {
			level--;
			//if the level is now 0 that means that we just finished a part (the inside of an if or else statement)
			if (level == 0) {
				CodeTreeNode p(currentPart, currentType, currentCondition);

				//checks if the code is not empty, pushes it onto the Nodes vector
				if (p.MultiPart) {
					Nodes.push_back(p);
				}
				else if (!Utils::AllNewLines(p.Code)) {
					Nodes.push_back(p);
				}
				
				//reset values in preparation of reading more code
				currentType = PartType::PlainCode;
				currentCondition = "";
				currentPart = "";
			}
			else {
				//if the level is not 0, we want to keep the '}' 
				currentPart += '}';
			}
			continue;
		}

		if (content[i] == '{') {
			level++;
			//if the level is one it means that the level was just 0 so this is the start of a Node and we should not record the {
			//we don't need to push a node onto Nodes just yet because that action would have been triggered by an if/else statement
			if (level == 1)continue;
		}

		//if the level is greater then 0, we are in the middle of the body of a Node, so we should just store it.
		if (level > 0) {
			currentPart += content[i];
			continue;
		}

		if (content[i] == 'i' && content[i + 1ULL] == 'f') {
			//push the previous node onto Nodes
			CodeTreeNode p(currentPart, currentType, currentCondition);
			if (p.MultiPart) {
				Nodes.push_back(p);
			}
			else if (!Utils::AllNewLines(p.Code)) {
				Nodes.push_back(p);
			}

			//reset values in preparation for reading this node
			currentType = PartType::If;//set the type to if because we're about to read an if statement
			currentCondition = "";
			currentPart = "";
			i++;//jump over the 'f' in "if" 
			continue;
		}

		//the next three statement are pretty self explanatory
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
			//push the previous node
			CodeTreeNode p(currentPart, currentType, currentCondition);
			if (p.MultiPart) {
				Nodes.push_back(p);
			}
			else if (!Utils::AllNewLines(p.Code)) {
				Nodes.push_back(p);
			}

			//reset values in preparation for the next node
			currentType = PartType::Else;
			currentCondition = "";
			currentPart = "";
			i += 3;//skip over the "lse" in "else"
			continue;
		}
		currentPart += content[i];
	}
	// after we finished reading, check if there is still a node we have not pushed
	// after every node push we reset the string currentPart, so if it's length is not 0, it contains unpushed code
	if (currentPart.size() > 0) {
		CodeTreeNode p(currentPart, currentType, currentCondition);
		if (p.MultiPart) {
			Nodes.push_back(p);
		}
		else if (!Utils::AllNewLines(p.Code)) {
			Nodes.push_back(p);
		}
		//no need to reset values since there are no other nodes
	}
	return;
}