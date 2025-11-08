#include "VocabularyLib.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <functional>
#include <print>

using namespace std;

int main(int argc, char** argv) {
	vector<string> args(argv+1, argv + argc);
	TransVoc tester;
	tester.add("ahoj", "hello");
	tester.add("ahoj", "wassup");
	tester.add("naschle", "bye");
	tester.add("modra", "blue");
	vector<std::reference_wrapper<const string>> answer1 = tester.find("ahoj");
	println("{}", answer1[0].get());
	println("{}", answer1[1].get());
	println("{}", answer1.size());
}