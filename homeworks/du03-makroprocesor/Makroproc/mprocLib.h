#include <string>
#include <map>
#include <vector>

#ifndef mprocLib
#define mprocLib

class MProc {
public:
	bool inputFail = false;
	std::string processChar(char nextChar);
	std::string returnLastWord();
	void addArgsMacro(const std::vector<std::string>& args);
private:
	bool hashtagPrev = false;
	enum statetype { THROUGH, NAMING, DEFINING } state = THROUGH;
	const std::string errorMessage = "Error\n";
	std::map<std::string, std::string> macros;
	void addMacro(const std::string& name, const std::string& value);
	std::string definedMacroName = "";
	std::string definedMacroValue = "";
	std::string buildWord = "";
	std::string processCharThrough(char nextChar);
	std::string processCharNaming(char nextChar);
	std::string processCharDefining(char nextChar);
};


#endif