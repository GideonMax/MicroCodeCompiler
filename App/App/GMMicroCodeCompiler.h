#pragma once

#include <stdint.h>



namespace MicroCodeCompiler {

	class Runtime {
	public:
		uint64_t EepromCount;
		/// <summary>
		/// executes all the code for a given eeprom.
		/// </summary>
		size_t Run(int Eeprom, uint64_t*& Out);
	private:
		/// <summary>
		/// padding for all the members that this header doesn't show
		/// PROBLEMATIC: I have to change the size every time I change the class
		/// </summary>
		char padding[209];
	};
	Runtime Compile(char* FilePath);
	void Write(Runtime& r, char* FilePath);
	size_t Size();
}
