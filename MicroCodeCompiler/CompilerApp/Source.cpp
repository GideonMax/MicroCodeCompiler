#include <iostream>
#include "MicroCodeCompiler.h"

int main(int ArgumentCount, char** Arguments) {
	std::cout << sizeof(MicroCodeCompiler::Runtime) << std::endl;
	if (ArgumentCount != 2) {
		std::cout << "Missing command line arguments" << std::endl;
		return 0;
	}
	try {
		MicroCodeCompiler::Runtime r = MicroCodeCompiler::Compile(Arguments[1]);
		uint64_t* a;
		//r.Run(0, a);
		MicroCodeCompiler::Write(r, Arguments[1]);
	}
	catch (std::runtime_error err) {
		std::cout << err.what() << std::endl;
	}
}