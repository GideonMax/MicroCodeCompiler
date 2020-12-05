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
		char padding[209];
	};
	Runtime Compile(char* FilePath);
	void Write(Runtime& r, char* FilePath);
}
