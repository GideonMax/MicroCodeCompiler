#pragma once
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <map>

namespace Utils {

	void Error(const std::string& error) {
		throw std::runtime_error(error);
	}

	bool AllNewLines(std::string& str) {
		for (char c : str) {
			if (c != '\n')return false;
		}
		return true;
	}

	/// <summary>
	/// takes a path and turns all \ into /
	/// </summary>
	/// <param name="Path"></param>
	void ChangePathFormat(char* Path) {
		int i = 0;
		while (Path[i] != 0) {
			if (Path[i] == '\\')Path[i] = '/';
			i++;
		}
		return;
	}

	/// <summary>
	/// takes an absolute path and a relative path and resolves them into an absolute path
	/// </summary>
	/// <param name="Path"></param>
	/// <param name="RelativePath"></param>
	/// <returns></returns>
	std::string ResolveRelativePath(const char* Path, std::string RelativePath) {
		std::string a = Path;
		a = a.substr(0, a.find_last_of('/'));

		while (RelativePath[1] == '.') {
			RelativePath = RelativePath.substr(3);
		}


		return a + '/' + RelativePath.substr(2);
	}

	/// <summary>
	/// takes a file stream reference, reads the file and removes all comments
	/// </summary>
	/// <param name="file"></param>
	/// <returns></returns>
	std::string RemoveComments(std::ifstream& file) {
		std::string ret = "";
		char c;
		bool InComment = false;
		while (file.get(c)) {
			switch (c)
			{
			case '/':
				if (!InComment) {
					int p = file.peek();
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
						ret += '\n';
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

	std::string ReadCodeFile(const char* FilePath) {
		std::ifstream file(FilePath, std::ios::binary);
		if (file.fail()) {
			auto error = file.rdstate();
			Utils::Error("Error reading file: " + error);
		}
		std::string ret=RemoveComments(file);
		file.close();
		return ret;
	}

	/// <summary>
	/// removes spaces and tabs from a string
	/// </summary>
	/// <param name="str"></param>
	/// <returns></returns>
	std::string RemoveSpaces(std::string& str) {
		std::string ret = "";
		for (unsigned int i = 0; i < str.length(); i++) {
			char current = str[i];
			if (current != ' ' && current != '\t' && current != '\r')ret += current;
		}
		return ret;
	}

	/// <summary>
	/// splits a string by a separator
	/// </summary>
	/// <param name="str"></param>
	/// <param name="separator"></param>
	/// <returns></returns>
	std::vector<std::string> Split(std::string& str, char separator) {
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


	/// <summary>
	/// takes a decimal, binary or hexadecimal number in a string and parses it
	/// Hex: 0x*number*
	/// decimal: *number*
	/// binary: 0b*number*
	/// </summary>
	/// <param name="str"></param>
	/// <returns></returns>
	uint64_t ParseConstantExpression(std::string& str) {
		if (str.size() == 0)Utils::Error("empty expression");
		if (str.size() == 1)return (uint64_t)str[0] - '0';
		if (str[0] == '\'')return (uint64_t)str[1];
		int base = 10;
		if ((str[0] == 'x') || (str[0] == 'X'))base = 16;
		else if (str[0] == 'b')base = 2;
		if (base != 10)str = str.substr(1);

		return std::stoi(str, nullptr, base);
	}

	template<typename Key, typename Value >
	bool MapContainsKey(std::map<Key, Value>& map, Key key) {
		return map.count(key) == 1;
	}
}