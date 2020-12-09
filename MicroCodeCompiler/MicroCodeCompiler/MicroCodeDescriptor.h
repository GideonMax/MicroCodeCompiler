#pragma once
#include <string>
#include <map>
#include <vector>

/// <summary>
/// represents the description file.
/// this is finished
/// </summary>
class MicroCodeDescriptor {
public:
	uint64_t EepromCount = 0, EepromAddressLength = 0, EepromOutputLength = 0;
	std::vector<uint64_t> DefaultValues;
	MicroCodeDescriptor();
	MicroCodeDescriptor(const char* FilePath);

	/// <summary>
	/// these are only used in the parsing stage to tokenize names into address/output locations
	/// </summary>
	struct AddressWordPart {
		uint64_t Index;
		uint64_t Length;
	};
	std::map<std::string, AddressWordPart> Address;
	struct ControlWordPart {
		uint64_t Index;
		uint64_t Eeprom;
		uint64_t Length;
		bool ActiveLow;
	};
	std::map<std::string, ControlWordPart> ControlLine;

private:
	enum class Header {
		EepromCount,
		EepromAddressLength,
		EepromOutputLength,
		Address,
		Output
	};
	/// <summary>
	/// this struct is both for address parts and control line parts
	/// </summary>
	const std::map<std::string, Header> HeaderNames =
	{
		{"eepromcount",Header::EepromCount},
		{"eepromaddresslength",Header::EepromAddressLength},
		{"eepromoutputlength", Header::EepromOutputLength},
		{"address", Header::Address},
		{"output",Header::Output}
	};
	void ParseAddress(std::string address);
	void ParseOutput(std::string output);
};