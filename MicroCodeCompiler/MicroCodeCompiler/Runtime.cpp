#include "MicroCodeCompilerPrivate.h"
#include <filesystem>

MicroCodeCompiler::Runtime::Runtime(std::map<uint64_t, Function> code, MicroCodeDescriptor descriptor) :
	Descriptor(descriptor),
	instruction(Descriptor.Address["instruction"]),
	Step(Descriptor.Address["step"]),
	Fetch(code[-1]),
	EepromCount(descriptor.EepromCount) {
	Code = code;
	Fetch = Code[-1];
}
size_t MicroCodeCompiler::Runtime::Run(int Eeprom, uint64_t*& Out) {

	size_t OutputSize = 1ULL << Descriptor.EepromAddressLength;
	uint64_t DefaultValue = Descriptor.DefaultValues[Eeprom];
	Out = new uint64_t[OutputSize];
	for (size_t i = 0; i < OutputSize; i++) {
		Out[i] = DefaultValue;
	}

	uint64_t loopCeiling = 1ULL << (Descriptor.EepromAddressLength - Step.Length);
	for (uint64_t i = 0; i < loopCeiling; i++) {
		uint64_t Address = MakeRoomForStep(i);
		uint64_t increment = 1ULL << Step.Index;
		Run(Eeprom, Out, Fetch.Instructions, Address, increment);

		uint64_t currentInstruction = (Address >> instruction.Index) & ((1ULL << instruction.Length) - 1);
		if (Utils::MapContainsKey(Code, currentInstruction)) {
			Function& func = Code[currentInstruction];
			Run(Eeprom, Out, func.Instructions, Address, increment);
		}

	}
	return OutputSize;
}
void MicroCodeCompiler::Runtime::Run(int Eeprom, uint64_t* Out, std::vector<Instruction>& code, uint64_t& Address, uint64_t increment) {
	for (Instruction& inst : code) {
		switch (inst.type)
		{
		case Instruction::InstructionType::Next:
			Address += increment;
			break;
		case Instruction::InstructionType::Set:
			if (inst.Eeprom == Eeprom) Out[Address] ^= (inst.Set_B(Address) << inst.Set_A(Address));
			break;
		case Instruction::InstructionType::Conditional:
			if (inst.Condition(Address)) Run(Eeprom, Out, inst.IfBody, Address, increment);
			else if (!inst.ElseBody.empty()) Run(Eeprom, Out, inst.ElseBody, Address, increment);
			break;
		default:
			break;
		}
	}
	return;
}

uint64_t MicroCodeCompiler::Runtime::MakeRoomForStep(uint64_t Address) {
	uint64_t firstPart = Address & ((1ULL << Step.Index) - 1);
	uint64_t secondPart = firstPart ^ Address;
	return firstPart | (secondPart << Step.Length);
}

std::string ParseFirstLine(std::string line) {
	size_t i = line.find('"');
	if (i == -1)Utils::Error("Expected descriptor file path");

	line = line.substr(i + 1ULL);
	i = line.find('"');
	if (i == -1)Utils::Error("Error parsing description file path");

	line = line.substr(0, i);
	return line;
}
MicroCodeCompiler::Runtime MicroCodeCompiler::Compile(char* FilePath) {

	char* path;
	Utils::ChangePathFormat(FilePath, path);

	std::string content = Utils::ReadCodeFile(path);

	//since every function starts with *, we can use it to split the file into the first line and the function
	std::vector<std::string> Parts = Utils::Split(content, '*');
	//gets the relative path of the descriptor file
	std::string relativeDescriptorFilePath = ParseFirstLine(Parts[0]);

	//resolves the path of the descriptor file
	std::string descriptorFilePath = Utils::ResolveRelativePath(path, relativeDescriptorFilePath);

	MicroCodeDescriptor descriptor = MicroCodeDescriptor(descriptorFilePath.c_str());

	std::map<uint64_t, Function> Code;
	for (size_t i = 1; i < Parts.size(); i++) {
		Function f(Utils::RemoveSpaces(Parts[i]), descriptor);
		Code.insert({ f.Alias,f });
	}
	return Runtime(Code, descriptor);
}

void MicroCodeCompiler::Write(MicroCodeCompiler::Runtime& r, char* FilePath) {
	std::string path = Utils::ResolveRelativePath(FilePath, "./out/");
	for (int i = 0; i < r.EepromCount; i++) {
		uint64_t* a;
		auto size= r.Run(i, a);
		if (!std::filesystem::exists(path)) {
			std::filesystem::create_directory(path);
		}
		std::ofstream stream = std::ofstream(path +std::to_string(i)+ ".out",std::ios::binary);
		stream.write((char*)a, size*sizeof(uint64_t));
		stream.close();
	}
	return;
}