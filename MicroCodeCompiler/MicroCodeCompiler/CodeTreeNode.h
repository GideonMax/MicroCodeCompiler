#pragma once
#include <string>
#include <vector>

/// <summary>
/// this struct takes code and builds a tree from it:
/// the Type of the node describes whether it is the body of an if/else statement or anything else.
/// MultiPart signifies whether the node has multiple child nodes, the vector Nodes will contain the child nodes.
/// </summary>
struct CodeTreeNode {
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
	std::vector<CodeTreeNode>  Nodes;

	/// <summary>
	/// this function constructs the tree
	/// if the current node is an if/else statement, content is the body and type will be If/Else
	/// </summary>
	/// <param name="content">the code</param>
	/// <param name="type">the type of node which the code is in</param>
	/// <param name="condition">the condition for an if statement</param>
	CodeTreeNode(std::string content, PartType type = PartType::PlainCode, std::string condition = "");
};
