#include "VocabularyLib.h"
#include <vector>
#include <string>
#include <functional>
#include <map>

using namespace std;

vector<reference_wrapper<const string>> TransVoc::find(const string& slovo) const {
	//For performance issues, method return references of string, not string themselves
	vector<reference_wrapper<const string>> returnVec;
	auto first = vocabulary.lower_bound({slovo, ""}); //surely predecessor to all searched elements
	string stopper = slovo;
	stopper[slovo.length()-1]++; //surely alphabetical successor of this key 
	auto ender = vocabulary.upper_bound({ stopper, "" });
	for (auto i = first; i != ender; i++) {
		returnVec.push_back(std::cref(i->second));
	}
	return returnVec;
}

vector<pair<string, vector<reference_wrapper<const string>>>> TransVoc::prefix(const string& slovo) const {
	vector<pair<string,vector<reference_wrapper<const string>>>> returnVec;
	auto it = vocabulary.lower_bound({ slovo, "" }); //surely predecessor to all searched elements
	string stopper = slovo;
	stopper[slovo.length() - 1]++; //surely alphabetical successor to all possible keys with this prefix
	auto ender = vocabulary.upper_bound({ stopper, "" });
	string activeKey;
	while(it!=ender){
		//Cycle over keys
		activeKey = it->first;
		vector<reference_wrapper<const string>> insideVec;
		string keyStopper = activeKey;
		keyStopper[activeKey.length() - 1]++; //successor to elements with this specific key
		auto keyEnder = vocabulary.upper_bound({ keyStopper, "" });
		while (it != keyEnder) {
		//Cycle over values
			insideVec.push_back(std::cref(it->second));
			++it;
		}
		returnVec.push_back({ activeKey,insideVec });
	}
	return returnVec;
}