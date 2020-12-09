#include "Utils.h"
#include <algorithm>

void Utils::Error(const std::string& error) {
	throw std::runtime_error(error);
}

bool Utils::AllNewLines(std::string& str) {
	for (char c : str) {
		if (c != '\n')return false;
	}
	return true;
}

void Utils::ChangePathFormat(char* Path, char *& Out) {
	size_t length = strlen(Path)+1;
	Out = new char[length];
	for (int i = 0;i<length; i++) {
		if (Path[i] == '\\')Out[i] = '/';
		else Out[i] = Path[i];
	}
	return;
}

std::string Utils::ResolveRelativePath(const char* Path, std::string RelativePath) {
	std::string a = Path;
	a = a.substr(0, a.find_last_of('/'));//chop off the file name in the absolute path

	while (RelativePath[1] == '.') {//happens when path start with ../
		RelativePath = RelativePath.substr(3);//chop off ../
		a = a.substr(0, a.find_last_of('/'));//chop off the last directory in a
	}


	return a + '/' + RelativePath.substr(2);//chop off ./ in relative path
}

std::string Utils::RemoveComments(std::ifstream& file) {
	std::string ret = "";
	char c;
	bool InComment = false;
	while (file.get(c)) {
		switch (c)
		{
		case '/':
			if (!InComment) {
				int p = file.peek();//get the next character to decide the comment's type
				if (p == '*') {
					InComment = true;
					file.get();
				}
				else if (p == '/') {
					while (file.get() != '\n' && !file.eof()) {}//read until the end of the line
					if (!file.eof())ret += '\n';
				}
				else
				{
					ret += c;
				}
			}
			break;
		case '*':
			if (InComment) {
				if (file.peek() == '/') {
					InComment = false;
					file.get();
				}
			}
			else {
				ret += c;
			}
			break;
		default:
			if (!InComment)ret += c;
		}
	}
	return ret;
}

std::string Utils::ReadCodeFile(const char* FilePath) {
	std::ifstream file(FilePath, std::ios::binary);
	if (file.fail()) {
		auto error = file.rdstate();
		Utils::Error("Error reading file: " + error);
	}
	std::string ret = RemoveComments(file);
	file.close();
	return ret;
}

std::string Utils::RemoveSpaces(std::string& str) {
	std::string ret = "";
	for (unsigned int i = 0; i < str.length(); i++) {
		char current = str[i];
		if (current != ' ' && current != '\t' && current != '\r')ret += current;
	}
	return ret;
}

std::vector<std::string> Utils::Split(std::string& str, char separator) {
	std::vector<std::string> ret;
	int pos = 0;
	for (size_t i = 0; i < str.length(); i++) {
		if (str[i] == separator) {
			ret.push_back(str.substr(pos, i - pos));
			pos = i + 1;
		}
	}
	if (pos != str.length())ret.push_back(str.substr(pos, str.length() - pos));
	return ret;
}

uint64_t Utils::ParseConstantExpression(std::string& str) {
	if (str.size() == 0)Utils::Error("empty expression");
	if (str.size() == 1)return (uint64_t)str[0] - '0';
	if (str[0] == '\'')return (uint64_t)str[1];//char

	int base = 10;
	if ((str[0] == 'x') || (str[0] == 'X'))base = 16;
	else if (str[0] == 'b')base = 2;
	if (base != 10)str = str.substr(1);

	return std::stoi(str, nullptr, base);
}

template<typename Key, typename Value >
bool Utils::MapContainsKey(std::map<Key, Value>& map, Key key) {
	return map.count(key) == 1;
}