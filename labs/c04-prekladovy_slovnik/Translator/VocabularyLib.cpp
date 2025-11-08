#include "VocabularyLib.h"
#include <vector>
#include <string>
#include <functional>
#include <map>

using namespace std;

std::vector<std::reference_wrapper<const string>> TransVoc::find(const string& slovo) const {
	//For performance issues, method return references of string, not string themselves
	vector<std::reference_wrapper<const string>> returnVec;
	//pair<multimap<string, string>::const_iterator, multimap<string, string>::const_iterator> range= vocabulary.equal_range(slovo);
	auto range = vocabulary.equal_range(slovo);
	auto first = range.first;
	auto ender = range.second;
	for (auto i = first; i != ender; ++i) {
		returnVec.push_back(std::cref(i->second));
	}
	return returnVec;
}