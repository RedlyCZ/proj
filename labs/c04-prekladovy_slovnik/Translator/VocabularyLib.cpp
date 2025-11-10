#include "VocabularyLib.h"
#include <vector>
#include <string>
#include <functional>
#include <map>

using namespace std;

vector<reference_wrapper<const string>> TransVoc::find(const string& slovo) const {
	//For performance issues, method return references of string, not string themselves to avoid copy
	vector<reference_wrapper<const string>> returnVec;

	auto it = vocabulary.lower_bound({ slovo, "" });

	while (it != vocabulary.end() && it->first == slovo) {
		returnVec.push_back(std::cref(it->second));
		++it;
	}

	return returnVec;
}

vector<pair<string, vector<reference_wrapper<const string>>>> TransVoc::prefix(const string& slovo) const {
	vector<pair<string, vector<reference_wrapper<const string>>>> returnVec;

	auto it = setOfKeys.lower_bound(slovo);

	while (it != setOfKeys.end() && it->starts_with(slovo) == true) {
		//starts_vec is not fully optimal as it many times reads the same prefix again and again (starts_with)
		//but going for performance here would require some alphabetic hacks, which would make it error-prone
		returnVec.push_back({ *it, this->find(*it) });
		++it;
	}

	return returnVec;
}

void TransVoc::del(const std::string& slovo) {
	auto range_start = vocabulary.lower_bound({ slovo, "" });
	auto it = range_start;

	while (it != vocabulary.end() && it->first == slovo) {
		++it;
	}
	auto range_end = it;
	//here the same problem as in prefix, also reads many strings, but further optimisation would require alphabetic hacks

	if (range_start != range_end) {
		vocabulary.erase(range_start, range_end);
		setOfKeys.erase(slovo);
	}
}