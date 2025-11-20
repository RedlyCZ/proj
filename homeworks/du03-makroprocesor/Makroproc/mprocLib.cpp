#include "mprocLib.h"
#include <string>
#include <algorithm>
#include <cctype>
#include <print>

using namespace std;

bool MProc::isStringAlnum(const string& word) {
	// Check if string is not empty and all chars are alphanumeric
	if (word.empty()) {
		return false;
	}
	return std::all_of(word.begin(), word.end(), [](unsigned char c) { return std::isalnum(c); });
}



string MProc::processWord(const string& word) {
	if (!defining) {
		if (word.at(0) == '#') {
			if (word.size() > 1) {
				string macroName = word.substr(1);
				if (isalpha(macroName.at(0)) && isStringAlnum(macroName)) {
					//So it indeed is a definition of macro
					definedMacroName = macroName;
					definedMacroValue = "";
					defining = true;
					return("");
				}
				else {
					return word;
				}
			}
		}
		else { //if not definition of macro
			if (macros.count(word) == 1) { //should be replaced by macro
				return macros.find(word)->second; //return its value
			}
			else { //count of map always returns 1 or 0, so this else means there is no macro of this name
				return word;
			}
		}
	}
	else {
		if (word.at(0) == '#' && word.size() == 1) { //end of defining
			definedMacroValue.pop_back(); //removes extra space at the end of macro value
			addMacro(definedMacroName, definedMacroValue);
			defining = false;
		}
		else {
			if (macros.count(word) == 1) { //should be replaced by macro
				definedMacroValue.append(macros.find(word)->second + " ");
			}
			else {
				definedMacroValue.append(word + " ");
			}
		}
		return("");
	}
}

void MProc::addMacro(const string& macroName, const string& macroValue) {
	macros.emplace(macroName, macroValue);
}