#pragma once
#include <functional>
#include <string>
#include "MicroCodeDescriptor.h"

/// <summary>
/// given an expression, this parses it and becomes a function which given the current address evaluates the expression.
/// if the expression is a constant value or an Address part, the function will return the value itself.
/// if it is a Control word part, the function will return the index.
/// </summary>
class Expression : public std::function<uint64_t(uint64_t)> {
public:
	int EEprom;//if the expression refers to a control word part, 
private:
	/// <summary>
	/// since the function has to be set in the initializer list of the constructor,
	///I made this as doing all the logic in the initializer list is impossible
	/// </summary>
	/// <param name="Expr">the code for the expression</param>
	/// <param name="descriptor">the descriptor</param>
	/// <returns></returns>
	std::function<uint64_t(uint64_t)> hiddenConstructor(std::string& Expr, MicroCodeDescriptor& descriptor);
public:
	Expression(std::string& Expr, MicroCodeDescriptor& descriptor);
	
	//the Expression is a constant 1
	Expression();
};