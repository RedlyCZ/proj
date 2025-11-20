#include <string>
#include <map>

#ifndef mprocLib
#define mprocLib

class MProc {
public:
	MProc() : inputFail{ false }, defining{ false } {}
	std::string processWord(const std::string& word);
	bool inputFail;
private:
	std::map<std::string, std::string> macros;
	void addMacro(const std::string& name, const std::string& value);
	bool isStringAlnum(const std::string& word);
	bool defining;
	std::string definedMacroName;
	std::string definedMacroValue;
};


#endif