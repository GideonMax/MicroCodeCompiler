#pragma once
#include <string>
#include <vector>

struct CodeTreePart {
	enum class PartType {
		PlainCode,
		If,
		Else
	};

	//if true, the part contains multiple parts of code (Parts), else, it is one piece of continuous code (Code)
	bool MultiPart;

	PartType Type;
	
	//if Multipart is true, this is null/undefined, else, it is just the plain code
	std::string Code;

	///if the part is the body of an if statement, this is the condition
	std::string IfCondition;

	//if multipart is true, this contains all the different parts contained in the part
	std::vector<CodeTreePart>  Parts;

	/// <summary>
	/// only the "inner" code is passed to the function,
	/// if the current part is an if/else statement, content is just the code between the {}
	/// </summary>
	/// <param name="content"></param>
	/// <param name="type"></param>
	/// <param name="condition"></param>
	/// <returns></returns>
	CodeTreePart(std::string content, PartType type = PartType::PlainCode, std::string condition = "");
};
