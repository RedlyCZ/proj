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
	return ""; //This wont really happen unless some very big problem, but compiler showed warning C4715, that not all paths have return
}

string MProc::processCharThrough(char nextChar) {
	if (isspace(nextChar)) { //if space came, we want to process the word that is loaded inside the buildword buffer
		if (this->hashtagPrev) { //space after hashtag when not defining is not permitted
			this->inputFail = true;
			return this->errorMessage;
		}
		hashtagPrev = false; //space is not hashtag
		if (buildWord.size() == 0) {
			return string(1,nextChar);
		}
		else {
			string strReturn;
			if (macros.count(buildWord) == 1) { //should be replaced by macro
				strReturn = macros.find(buildWord)->second + nextChar; //use the macro and add the space (nextChar)
				buildWord = "";
				return strReturn;
			}
			else { //no macro is adequate
				strReturn = buildWord + nextChar;
				buildWord = "";
				return strReturn;
			}
		}
	}
	if (nextChar == '#') { //hashtags are not displayed
		this->hashtagPrev = true;
		return "";
	}
	if (isalpha(nextChar) && hashtagPrev) { //beginning of macro definition
		hashtagPrev = false;
		state = NAMING;
		processChar(nextChar); //for we already loaded first char of macro def
		return "";
	}
	hashtagPrev = false;
	buildWord += nextChar; //if any other char append it to the word being build
	return ""; //dont want return nothing yet, dont know whether it will transfrom
}

string MProc::returnLastWord(){
	if (macros.count(buildWord) == 1) { //if the word built represents some macro
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

	if (!isspace(nextChar) && !isalnum(nextChar)) { //banned by assignment rules
		this->inputFail = true;
		return this->errorMessage;
	}
	else {
		return ""; //when naming, we want to return nothing
	}
}

string MProc::processCharDefining(char nextChar) {
	if (isspace(nextChar)) {
		if (hashtagPrev) { //end of definition
			this->addMacro(definedMacroName, definedMacroValue);
			definedMacroName = "";
			definedMacroValue = "";
			state = THROUGH;
			this->hashtagPrev = false;
			return ""; //when naming, we want to return nothing
		}
		if (buildWord.size() == 0) {
			definedMacroValue += nextChar;
			return ""; //when naming, we want to return nothing
		}
		else {
			if (macros.count(buildWord) == 1) {
				definedMacroValue += (macros.find(buildWord)->second + nextChar); //allows usage of macro in macro definition
			}
			else {
				definedMacroValue += (buildWord + nextChar);
			}
			buildWord = "";
			return ""; //when naming, we want to return nothing
		}
	}
	if (nextChar == '#') {
		this->hashtagPrev = true;
		return "";
	}
	if (hashtagPrev) { 
		if (isalpha(nextChar)) { //definition inside definition - which is banned
			this->inputFail = true;
			return this->errorMessage;
		}
		else {
			this->hashtagPrev = false;
		}
	}
	//if still here, nextChar is just some nonimportant symbol (alnum or sth)
	buildWord += nextChar;
	return ""; //when naming, we want to return nothing
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