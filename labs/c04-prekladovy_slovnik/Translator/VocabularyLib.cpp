#include "VocabularyLib.h"
#include <vector>
#include <string>
#include <functional>
#include <map>

using namespace std;

vector<reference_wrapper<const string>> TransVoc::find(const string& slovo) const {
	//For performance issues, method return references of string, not string themselves
	vector<reference_wrapper<const string>> returnVec;

	/**
	auto first = vocabulary.lower_bound({slovo, ""}); //surely predecessor to all searched elements
	string stopper = slovo + '0'; //surely direct alphabetical successor of this key 
	auto ender = vocabulary.upper_bound({ stopper, "" });
	for (auto i = first; i != ender; i++) {
		returnVec.push_back(std::cref(i->second));
	}
	return returnVec;
	**/
	auto it = vocabulary.lower_bound({ slovo, "" });

	while (it != vocabulary.end() && it->first == slovo) {
		returnVec.push_back(std::cref(it->second));
		++it;
	}

	return returnVec;
}

vector<pair<string, vector<reference_wrapper<const string>>>> TransVoc::prefix(const string& slovo) const {
	vector<pair<string, vector<reference_wrapper<const string>>>> returnVec;
	/** auto it = setOfKeys.lower_bound(slovo); //first word with such prefix
	string stopper = slovo;
	stopper[slovo.length() - 1]++; //surely alphabetical successor to all possible keys with this prefix
	auto ender = setOfKeys.lower_bound(stopper);
	for (auto iter = it; iter != ender; iter++) {
		returnVec.push_back({ (*iter), this->find((*iter)) });
	}
	return returnVec;
	**/
	auto it = setOfKeys.lower_bound(slovo);

	while (it != setOfKeys.end() && it->starts_with(slovo) == true) {
		returnVec.push_back({ *it, this->find(*it) });
		++it;
	}

	return returnVec;
}

void TransVoc::del(const std::string& slovo) {
	/**
	auto first = vocabulary.lower_bound({slovo, ""});
	auto stopper = slovo + (char)0;
	auto end = vocabulary.lower_bound({ stopper, "" });
	if (first != end) {
			vocabulary.erase(first, end);
			setOfKeys.erase(slovo);
	}
	**/
	auto range_start = vocabulary.lower_bound({ slovo, "" });
	auto it = range_start;

	while (it != vocabulary.end() && it->first == slovo) {
		++it;
	}
	auto range_end = it;

	if (range_start != range_end) {
		vocabulary.erase(range_start, range_end);
		setOfKeys.erase(slovo);
	}
}