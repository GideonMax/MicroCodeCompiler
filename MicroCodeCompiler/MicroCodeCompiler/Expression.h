#pragma once
#include <functional>
#include <string>
#include "MicroCodeDescriptor.h"

/// <summary>
/// this class is for parsing expressions
/// by parsing, it sets its base class (std::function) to a function the evaluates the expression given the address
/// </summary>
class Expression : public std::function<uint64_t(uint64_t)> {
public:
	//if the expression refers to a control word part, this is the eeprom it belongs to
	uint64_t EEprom;
private:
	/// <summary>
	/// since the function has to be set in the initializer list of the constructor,
	/// I made this as doing all the logic in the initializer list is impossible
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