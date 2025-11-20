#include "mprocLib.h"
#include <iostream>
#include <string>
#include <print>

using namespace std;

int main(int argc, int argv) {
	MProc processor;
	string word;
	string output;
	string spaceBetween = "";
	for (;;) {
		cin >> word;
		if (cin.fail()) {
			break;
		}
		output = processor.processWord(word);
		if (output.size() != 0) {
			print("{}{}", spaceBetween, output);
			spaceBetween = " ";
		}
	}
}