#include "mprocLib.h"
#include <string>
#include <algorithm>
#include <cctype>
#include <print>

using namespace std;

string MProc::processChar(char nextChar) {
	switch (this->state) {
	case THROUGH:
		return processCharThrough(nextChar);
	case NAMING:
		return processCharNaming(nextChar);
	case DEFINING:
		return processCharDefining(nextChar);
	}
}

string MProc::processCharThrough(char nextChar) {
	string strReturn = "";
	if (isspace(nextChar)) {
		if (this->hashtagPrev) { //space after hashtag when not defining is not permitted
			this->inputFail = true;
			return "Error\n";
		}
		hashtagPrev = false; //space is not hashtag
		if (buildWord.size() == 0) {
			return (strReturn + nextChar);
		}
		else {
			if (macros.count(buildWord) == 1) { //should be replaced by macro
				strReturn = macros.find(buildWord)->second + nextChar; //use the macro and add the space (nextChar)
				buildWord = "";
				return strReturn;
			}
			else {
				strReturn = buildWord + nextChar;
				buildWord = "";
				return strReturn;
			}
		}
	}
	if (nextChar == '#') {
		this->hashtagPrev = true;
		return strReturn;
	}
	if (isalpha(nextChar) && hashtagPrev) {
		hashtagPrev = false;
		state = NAMING;
		processChar(nextChar);
		return strReturn;
	}
	hashtagPrev = false;
	buildWord += nextChar; //if any other char append it to the word being build
	return strReturn; //dont want return nothing yet, dont know whether it will transfrom
}

string MProc::returnLastWord(){
	if (macros.count(buildWord) == 1) {
		return(macros.find(buildWord)->second);
	}
	else {
		return buildWord;
	}
}

string MProc::processCharNaming(char nextChar) {
	if (isspace(nextChar)) {
		state = DEFINING;
	}
	if (isalnum(nextChar)){
		definedMacroName += nextChar;
	}
	//Would do unexpected things if definition is not correct (not only from alnums) -- Correct
	return ""; //when naming, we dont want to return nothing
}

string MProc::processCharDefining(char nextChar) {
	if (isspace(nextChar)) {
		if (hashtagPrev) { //end of definition
			this->addMacro(definedMacroName, definedMacroValue);
			definedMacroName = "";
			definedMacroValue = "";
			state = THROUGH;
			this->hashtagPrev = false;
			return "";
		}
		if (buildWord.size() == 0) {
			definedMacroValue += nextChar;
			return "";
		}
		else {
			if (macros.count(buildWord) == 1) {
				definedMacroValue += (macros.find(buildWord)->second + nextChar);
			}
			else {
				definedMacroValue += (buildWord + nextChar);
			}
			buildWord = "";
			return "";
		}
	}
	if (nextChar == '#') {
		this->hashtagPrev = true;
		return "";
	}
	if (hashtagPrev) { 
		if (isalpha(nextChar)) { //definition inside definition - which is banned
			this->inputFail = true;
			return "Error\n";
		}
		else { //also may need some handling here :D, this works with good input without many # symbols used incorrectly
			this->hashtagPrev = false;
		}
	}
	//if still here, nextChar is just some nonimportant symbol (alnum or sth)
	buildWord += nextChar;
	return ""; //when defining, we dont want to return nothinf
}

void MProc::addMacro(const string& macroName, const string& macroValue) {
	macros.emplace(macroName, macroValue);
}

void MProc::addArgsMacro(const vector<string>& args) {
	string macroBody = args[1];
	for (auto it = args.cbegin() + 2; it != args.cend(); it++) {
		macroBody.append(" " + (*it));
	}
	addMacro(args[0], macroBody);
}