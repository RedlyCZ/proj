#include <map>
#include <string>
#include <vector>
#include "freqOfWords.h"
#include <print>
using namespace std;

int main(int argc, char ** argv) {
	vector<string> arg(argv + 1, argv + argc);

	freqAnalyser a;

	if (argc>1 && arg[0] == "-k") {
		int rank = stoi(arg[1]);
		arg.erase(arg.begin(), arg.begin() + 1);
		a.processWordVector(arg);
		bool firsttime = true;
		for (auto&& x : a.kthMostFrequent(rank)) {
			if (firsttime) {
				print("{}", a.freqMap[x]);
				firsttime = false;
			}
			print(",{}", x);
		}
	}
	else {
		a.processWordVector(arg);
		a.printWholeMap();
	}

	

	
	
	

	/*
	for (auto&& x : a.kthMostFrequent(2)) {
		println("{}", x);
	}
	println("{}", a.wordCount);
	println("{} s frekvenci : {}", slovo, a.freqMap[slovo]);
	*/
}