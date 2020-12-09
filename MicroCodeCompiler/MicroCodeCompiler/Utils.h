#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <map>

namespace Utils {

	void Error(const std::string& error);

	bool AllNewLines(std::string& str);
	/// <summary>
	/// takes a path and turns all \ into /
	/// </summary>
	/// <param name="Path"></param>
	void ChangePathFormat(char* Path, char*& Out);

	/// <summary>
	/// takes an absolute path and a relative path and resolves them into an absolute path
	/// </summary>
	/// <param name="Path"></param>
	/// <param name="RelativePath"></param>
	/// <returns></returns>
	std::string ResolveRelativePath(const char* Path, std::string RelativePath);

	/// <summary>
	/// takes a file stream reference, reads the file and removes all comments
	/// </summary>
	/// <param name="file"></param>
	/// <returns></returns>
	std::string RemoveComments(std::ifstream& file);

	std::string ReadCodeFile(const char* FilePath);

	/// <summary>
	/// removes spaces and tabs from a string
	/// </summary>
	/// <param name="str"></param>
	/// <returns></returns>
	std::string RemoveSpaces(std::string& str);

	/// <summary>
	/// splits a string by a separator
	/// </summary>
	/// <param name="str"></param>
	/// <param name="separator"></param>
	/// <returns></returns>
	std::vector<std::string> Split(std::string& str, char separator);

	/// <summary>
	/// takes a decimal, binary or hexadecimal number in a string and parses it
	/// Hex: 0x*number*
	/// decimal: *number*
	/// binary: 0b*number*
	/// </summary>
	/// <param name="str"></param>
	/// <returns></returns>
	uint64_t ParseConstantExpression(std::string& str);

	template<typename Key, typename Value >
	bool MapContainsKey(std::map<Key, Value>& map, Key key);
}