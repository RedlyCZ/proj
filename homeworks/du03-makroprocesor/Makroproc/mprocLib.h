#include <string>
#include <map>
#include <vector>

#ifndef mprocLib
#define mprocLib

class MProc {
public:
	MProc() : inputFail{ false }, state{ THROUGH } {}
	std::string processChar(char nextChar);
	bool inputFail;
	std::string returnLastWord();
	void addArgsMacro(const std::vector<std::string>& args);
private:
	std::map<std::string, std::string> macros;
	void addMacro(const std::string& name, const std::string& value);
	std::string definedMacroName = "";
	std::string definedMacroValue = "";
	enum statetype { THROUGH, NAMING, DEFINING } state;
	std::string processCharThrough(char nextChar);
	std::string buildWord = "";
	std::string processCharNaming(char nextChar);
	std::string processCharDefining(char nextChar);
	bool hashtagPrev = false;
};


#endif