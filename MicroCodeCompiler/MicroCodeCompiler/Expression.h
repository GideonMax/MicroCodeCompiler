#pragma once
#include <functional>
#include <string>
#include "MicroCodeDescriptor.h"
#include "Utils.h"

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
	std::function<uint64_t(uint64_t)> hiddenConstructor(std::string& Expr, MicroCodeDescriptor& descriptor) {

		///test for the == operator
		size_t eq = Expr.find_first_of('=');
		if (eq != -1) {
#ifdef _DEBUG
			if (Expr[eq + 1ULL] != '=')Utils::Error("found single = in expression");
#endif 


			Expression a(*& Expr.substr(0, eq), descriptor);
			Expression b(*& Expr.substr(eq + 2ULL, Expr.size() - eq - 2), descriptor);
			return [=](uint64_t Address) {
				return (uint64_t)(a(Address) == b(Address));
			};
			
		}

		//if Expr is a constant Expression return a function with a constant value
		if (Expr[0] <= '9' && Expr[0] >= '0'|| Expr[0]=='\'') {
			uint64_t val = Utils::ParseConstantExpression(Expr);
			return [=](uint64_t i) {return val; };
		}


		int Bracket = Expr.find('[');
		//get the name of the variable
		std::string name;
		if (Bracket == -1) {
			name = Expr;
		}
		else {
			name = Expr.substr(0, Bracket);
		}

		//gets whether the part is an address part
		bool AddressPart = Utils::MapContainsKey(descriptor.Address, name);

#ifdef _DEBUG
		if (!AddressPart && !Utils::MapContainsKey(descriptor.ControlLine, name)) {
			Utils::Error("Given Part \"" + name + "\" is not a valid part (it is not mentioned in the description file)");
		}
#endif 

		if (Bracket == -1) {
			if (AddressPart) {
				MicroCodeDescriptor::AddressWordPart part = descriptor.Address[name];
				//the function shifts the Address right so the given part starts at index 0 and then ands it with a mask to zero out the other variables
				return [=](uint64_t Address) {
					return (Address >> part.Index) & ((1ULL << part.Length) - 1);
				};
			}
			else {
				MicroCodeDescriptor::ControlWordPart part = descriptor.ControlLine[name];
				EEprom = part.Eeprom;
				return [=](uint64_t Address) {return part.Index; };//just returns the index
			}
		}

		//get the value in the square brackets
		std::string ValueInBrackets = Expr.substr(Bracket, Expr.size() - Bracket - 1);

		//test for a comma, set first and second accordingly. (if there is no comma, first is just the value)
		size_t Comma = ValueInBrackets.find(',');
		std::string first;
		std::string second;
		if (Comma != -1) {
			first = ValueInBrackets.substr(0, Comma);
			second = ValueInBrackets.substr(Comma+1ULL, ValueInBrackets.size() - Comma-1);
		}
		else {
			first = ValueInBrackets;
		}
		if (!AddressPart) {//if it is a control word part, the second value can be ignored as it does not change the index.
			MicroCodeDescriptor::ControlWordPart part = descriptor.ControlLine[name];
			Expression expression(first, descriptor);
			EEprom = part.Eeprom;
			return [=](uint64_t Address) {
				return part.Index + expression(Address);
			};
		}
		else {
			//these function do pretty much the same thing, they shift the address and then and it with a mask.
			if (Comma == -1) {
				Expression expression(first, descriptor);
				MicroCodeDescriptor::AddressWordPart part = descriptor.Address[name];
				return [=](uint64_t Address) {
					return (Address >> (part.Index + expression(Address))) & 1;
				};
			}
			else {
				Expression firstExpression(first, descriptor);
				Expression secondExpression(second, descriptor);
				MicroCodeDescriptor::AddressWordPart part = descriptor.Address[name];
				return [=](uint64_t Address) {
					uint64_t a = firstExpression(Address);
					uint64_t b = secondExpression(Address);
					return (Address >> (part.Index + a)) & ((1ULL<<(b-a+1))-1);
				};

			}
		}

	}
public:
	Expression(std::string& Expr, MicroCodeDescriptor& descriptor): std::function<uint64_t(uint64_t)>(hiddenConstructor(Expr, descriptor)) { }
	
	//the Expression is a constant 1
	Expression() :std::function<uint64_t(uint64_t)>([](uint64_t Address) {return 1; }) {};
};