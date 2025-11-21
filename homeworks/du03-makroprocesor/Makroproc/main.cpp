#include "mprocLib.h"
#include <iostream>
#include <string>
#include <print>
#include <vector>

using namespace std;

int main(int argc, char ** argv) {
	vector<string> args(argv + 1, argv + argc);
	MProc processor;
	if (args.size() > 1) {
		processor.addArgsMacro(args);
	}
	string output;
	for (;;) {
		char nextChar = cin.get();
		if (cin.fail()) {
			break;
		}
		output = processor.processChar(nextChar);
		print("{}", output);
		if (processor.inputFail) {
			break;
		}
	}
	print("{}", processor.returnLastWord());
}