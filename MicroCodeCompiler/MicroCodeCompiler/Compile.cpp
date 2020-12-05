#include "MicroCodeCompiler.h"
std::string ParseFirstLine(std::string line) {
	size_t i = line.find('"');
#ifdef _DEBUG
	if (i == -1)Utils::Error("Expected descriptor file path");
#endif // _DEBUG

	line = line.substr(i + 1ULL);
	i = line.find('"');
#ifdef _DEBUG
	if (i == -1)Utils::Error("Error parsing description file path");
#endif // _DEBUG

	line = line.substr(0, i);
	return line;
}
MicroCodeCompiler::Runtime MicroCodeCompiler::Compile(char* FilePath) {
	
	Utils::ChangePathFormat(FilePath);

	std::string content = Utils::ReadCodeFile(FilePath);

	//since every function starts with *, we can use it to split the file into the first line and the function
	std::vector<std::string> Parts = Utils::Split(content, '*');
	//gets the relative path of the descriptor file
	std::string relativeDescriptorFilePath = ParseFirstLine(Parts[0]);

	//resolves the path of the descriptor file
	std::string descriptorFilePath = Utils::ResolveRelativePath(FilePath, relativeDescriptorFilePath);

	MicroCodeDescriptor descriptor = MicroCodeDescriptor(descriptorFilePath.c_str());

	std::map<uint64_t, Function> Code;
	for (int i = 1; i < Parts.size(); i++) {
		Function f(Utils::RemoveSpaces(Parts[i]), descriptor);
		Code.insert({ f.Alias,f });
	}
	return Runtime(Code, descriptor);
}

