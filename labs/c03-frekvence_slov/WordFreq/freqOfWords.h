#include <string>
#include <map>
#include <vector>

#ifndef freqOfWords
#define freqOfWords

class freqAnalyser {
public:
	void processWord(const std::string& s);
	void processWordVector(const std::vector<std::string>& words);
	std::map<std::string, int> freqMap;
	int wordCount = 0;
	std::vector<std::string> kthMostFrequent(int k);
	void printWholeMap();
};

#endif