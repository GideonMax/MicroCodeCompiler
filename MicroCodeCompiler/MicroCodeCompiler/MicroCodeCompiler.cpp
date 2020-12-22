#include "MicroCodeCompiler.h"
#include "Utils.h"
#include <filesystem>

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
		//the reason for the *& is for some reason, it can't cast an std::string into an std::string&
		Function f(*&Utils::RemoveSpaces(Parts[i]), descriptor);
		Code.insert({ f.Opcode,f });
	}
	return Runtime(Code, descriptor);
}

void MicroCodeCompiler::Write(MicroCodeCompiler::Runtime& r, char* FilePath) {
	char* Path;
	Utils::ChangePathFormat(FilePath, Path);
	std::string path = Utils::ResolveRelativePath(Path, "./out/");
	for (int i = 0; i < r.EepromCount; i++) {
		uint64_t* a;
		auto size = r.Run(i, a);
		if (!std::filesystem::exists(path)) {
			std::filesystem::create_directory(path);
		}
		std::ofstream stream = std::ofstream(path + std::to_string(i) + ".out", std::ios::binary);
		stream.write((char*)a, size * sizeof(uint64_t));
		stream.close();
	}
	return;
}

size_t MicroCodeCompiler::Size() {
	return sizeof(MicroCodeCompiler::Runtime);
}