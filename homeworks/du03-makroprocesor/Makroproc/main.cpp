#include "mprocLib.h"
#include <iostream>
#include <string>
#include <print>

using namespace std;

int main(int argc, char ** argv) {
	MProc processor;
	string word;
	string output;
	string spaceBetween;
	for (;;) {
		cin >> word;
		if (cin.fail() || processor.inputFail) {
			break;
		}
		output = processor.processWord(word);
		if (output.size() != 0) {
			print("{}", output);
			
		}
		spaceBetween = cin.get();
		if (cin.fail()) {
			break;
		}
		print("{}", spaceBetween);
	}
}