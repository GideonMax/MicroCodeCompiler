#include "Expression.h"
#include "Utils.h"

std::function<uint64_t(uint64_t)> Expression::hiddenConstructor(std::string& Expr, MicroCodeDescriptor& descriptor) {

	///test for the == operator
	size_t eq = Expr.find_first_of('=');
	if (eq != -1) {
		if (Expr[eq + 1ULL] != '=')Utils::Error("found single = in expression");


		Expression a(*&Expr.substr(0, eq), descriptor);
		Expression b(*&Expr.substr(eq + 2ULL, Expr.size() - eq - 2), descriptor);
		return [=](uint64_t Address) {
			return (uint64_t)(a(Address) == b(Address));
		};

	}

	//if Expr is a constant Expression return a function with a constant value
	if ((Expr[0] <= '9' && Expr[0] >= '0') || Expr[0] == 'b' || Expr[0] == 'x' || Expr[0] == '\'') {
		uint64_t val = Utils::ParseConstantExpression(Expr);
		return [=](uint64_t i) {return val; };
	}

	//look for indexing
	size_t Bracket = Expr.find('[');
	//get the name of the variable
	std::string name;
	if (Bracket == -1) {
		name = Expr;
	}
	else {
		name = Expr.substr(0, Bracket);
	}

	//gets whether the part is an address part
	bool isAddressPart = descriptor.Address.count(name)==1;

	if (!isAddressPart && descriptor.ControlLine.count(name) == 0) {
		Utils::Error("Given Part \"" + name + "\" is not a valid part (it is not mentioned in the description file)");
	}

	if (Bracket == -1) {
		if (isAddressPart) {
			MicroCodeDescriptor::AddressWordPart part = descriptor.Address[name];

			//the function shifts the Address right so the given part will be shifted to index 0
			//and then ands it with a mask to zero out the other variables
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
	std::string ValueInBrackets = Expr.substr(Bracket+1, Expr.size() - Bracket - 2);

	//test for a comma, set first and second accordingly. (if there is no comma, first is just the value)
	size_t Comma = ValueInBrackets.find(',');
	std::string first;
	std::string second;
	if (Comma != -1) {
		first = ValueInBrackets.substr(0, Comma);
		second = ValueInBrackets.substr(Comma + 1ULL, ValueInBrackets.size() - Comma - 1);
	}
	else {
		first = ValueInBrackets;
	}

	//if it is a control word part, the second value can be ignored as it does not change the index.
	if (!isAddressPart) {
		MicroCodeDescriptor::ControlWordPart part = descriptor.ControlLine[name];
		Expression expression(first, descriptor);
		EEprom = part.Eeprom;
		return [=](uint64_t Address) {
			return part.Index + expression(Address);
		};
	}
	else {
		// these functions do pretty much the same thing, they shift the address and then and it with a mask.
		// the difference is, in the first case, the mask is just 1.
		// in the second case, its length is based on the difference of the 2 indexing expressions
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
				return (Address >> (part.Index + a)) & ((1ULL << (b - a + 1)) - 1);
			};

		}
	}

}

Expression::Expression(std::string& Expr, MicroCodeDescriptor& descriptor) : std::function<uint64_t(uint64_t)>(hiddenConstructor(Expr, descriptor)) { }

Expression::Expression() : std::function<uint64_t(uint64_t)>([](uint64_t Address) {return 1; }), EEprom(-1) {}
