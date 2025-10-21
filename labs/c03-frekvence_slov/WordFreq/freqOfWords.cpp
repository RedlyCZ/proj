#include <map>
#include <string>
#include "freqOfWords.h"
#include <algorithm>
#include <functional>
using namespace std;

void freqAnalyser::processWord(std::string& slovo) {
	auto it = freqMap.find(slovo);
	if (it != freqMap.end()) {
		++(it->second);
	}
	else {
		freqMap[slovo] = 1;
	}
}

vector<string> freqAnalyser::kthMostFrequent(int k) {
	vector<int> freqVector;
	for (auto it = freqMap.cbegin(); it != freqMap.cend(); it++) {
		freqVector.push_back(it->second);
	}
	sort(freqVector.begin(), freqVector.end(), greater<int>());
	int kFrequency = freqVector[k-1];
	vector<std::string> returnVec;
	for (auto it = freqMap.cbegin(); it != freqMap.cend(); it++) {
		if (it->second == kFrequency) {
			returnVec.push_back(it->first);
		}
	}
	return returnVec;
}