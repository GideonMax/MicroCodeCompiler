#include "MicroCodeDescriptor.h"
#include <algorithm>
#include <sstream>
#include "Utils.h"

MicroCodeDescriptor::MicroCodeDescriptor() {}

MicroCodeDescriptor::MicroCodeDescriptor(const char* FilePath) {
	//read file and remove comments
	std::string content = Utils::RemoveSpaces(*&Utils::ReadCodeFile(FilePath));

	//go over every line and parse it
	std::stringstream stream(content);
	std::string line;
	while (std::getline(stream, line)) {

		//finds the location of the colon
		size_t i = line.find(':');
		if (i == -1)continue;

		//get key and value
		std::string key = line.substr(0, i);
		std::string value = line.substr(i + 1ULL);

		//turns the key to lowercase
		std::transform(key.begin(), key.end(), key.begin(), [](char c) {return std::tolower(c); });

		//gets the enum value that fits the key
		switch (HeaderNames.at(key)) {
		case Header::EepromCount:
			EepromCount = std::stoi(value, nullptr);
			break;
		case Header::EepromAddressLength:
			EepromAddressLength = std::stoi(value, nullptr);
			break;
		case Header::EepromOutputLength:
			EepromOutputLength = std::stoi(value, nullptr);
			break;
		case Header::Address:
			ParseAddress(value);
			break;
		case Header::Output:
			ParseOutput(value);
			break;
		}
	}
	
	DefaultValues = std::vector<uint64_t>(EepromCount, 0);//get the default values for each eeprom
	for (auto& pair : ControlLine) {
		ControlWordPart& word = pair.second;
		if (word.ActiveLow) {
			(DefaultValues)[word.Eeprom] |= ((1ULL << word.Length) - 1) << word.Index;
		}
	}
	if (!Utils::MapContainsKey<std::string, AddressWordPart>(Address, "instruction") ||
		!Utils::MapContainsKey<std::string, AddressWordPart>(Address, "step"))
		Utils::Error("Did not find step and/or instruction address part");

}


void MicroCodeDescriptor::ParseAddress(std::string address) {

	std::vector<std::string> parts = Utils::Split(address, ',');//separate all the address parts
	unsigned int index = 0;
	for (auto part : parts) {
		if (part.length() == 0)continue;//in case of dumb input code with empty parts, this line forgives
		std::string name;
		unsigned int length = 1;

		size_t i = part.find('[');
		if (i == -1) {//if there is no '[' then the entire part is the name
			name = part;
		}
		else {
			//separate in name and number+']', std::stoi automatically removes the ']'
			name = part.substr(0, i);
			length = std::stoi(part.substr(i + 1ULL));
		}
		if (name[0] == '0') {
			index += length;
			continue;
		}
		AddressWordPart a = { index,length };
		Address.insert({ name,a });
		index += length;
	}
}

void MicroCodeDescriptor::ParseOutput(std::string output) {

	//default values
	size_t index = 0;
	uint64_t eeprom = 0;
	size_t length = 1;
	bool inName = true;
	bool activeLow = false;
	std::string name = "";

	ControlWordPart a;

	//loop over each character
	for (char c : output) {
		switch (c)
		{
			//the cases for , and ; are very similar.
		case ',': {
			if (length > EepromOutputLength)throw "control part too large for eeprom";
			//if the part is too long for the current eeprom, move on to the next one
			if (index + length > EepromOutputLength) {
				index = 0;
				eeprom++;
			}
			if (name != "0") {//insert line only if it is not 0
				a = { index , eeprom , length , activeLow };
				ControlLine.insert({ name,a });
			}
			index += length;//move the index
			eeprom += index / EepromOutputLength;//move the eeprom by the appropriate amount
			index %= EepromOutputLength;//truncate the index down if it is too big

			//set default values
			name = "";
			length = 1;
			inName = true;
			activeLow = false;
			break;
		}
		case ';': {
			if (index + length > EepromOutputLength) {
				eeprom++;
				index = 0;
			}
			if (name != "0") {
				a = { index , eeprom , length , activeLow };
				ControlLine.insert({ name,a });
			}
			// This is the difference between ; and ,
			// These lines move to the next eeprom
			eeprom++;
			index = 0;

			name = "";
			length = 1;
			inName = true;
			activeLow = false;
			break;
		}
		case ']'://this does nothing
			break;
		case '['://if you reach the [, you have finished the name and are now parsing the number
			inName = false;
			break;
		default: {
			if (inName)//if currently parsing name, added character to name unless it is ! which marks active low
			{
				if (c == '!')activeLow = true;
				else name += c;
			}
			else
			{
				length *= 10;//if you are not parsing the name and all the other conditions have failed, you are parsing the length, in that case, this parses the number
				length += c - '0';
			}
			break;
		}
		}
	}
	if (name != "0") {
		a = { index , eeprom , length , activeLow };
		ControlLine.insert({ name,a });
	}
}
