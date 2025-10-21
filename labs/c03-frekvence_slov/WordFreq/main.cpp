#include <map>
#include <string>
#include "freqOfWords.h"
#include <print>
using namespace std;

int main() {
	freqAnalyser a;
	string slovo = "apt";
	string dalsiSlovo = "ahaaha";
	string tretiSlovo = "dont";
	string ctvrte = "you";
	a.processWord(slovo);
	a.processWord(slovo);
	a.processWord(slovo);
	a.processWord(dalsiSlovo);
	a.processWord(dalsiSlovo);
	a.processWord(tretiSlovo);
	a.processWord(tretiSlovo);
	a.processWord(ctvrte);

	map<string, int>::const_iterator ctecka = a.freqMap.begin();
	//while (ctecka != a.freqMap.end()) {
	//	println("klic {}, payload {}", ctecka->first, ctecka->second);
	//	++ctecka;
	//}
	for (auto&& x : a.kthMostFrequent(2)) {
		println("{}", x);
	}
	
}