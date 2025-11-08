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
	TransVoc vocab;
	string r, cmd, arg, secondArg;
	for (;;) {
		getline(cin, r);
		if (cin.fail()) break;
		istringstream line(r);
		line >> cmd;
		if (cmd == "add" || cmd == "del") {
			if (line >> arg) {
				if (line >> secondArg) {
					if (cmd == "add") {
						vocab.add(arg, secondArg);
					}
					else {
						vocab.del(arg, secondArg);
					}
				}
				else {
					if (cmd == "del") {
						vocab.del(arg);
					}
				}
			}
		}
		else {
			if (cmd == "find") {
				if (line >> arg) {
					auto answer = vocab.find(arg);
					if (!answer.empty()) {
						print("{}", answer.front().get());
						for (size_t i = 1; i < answer.size(); ++i) {
							print(" {}", answer[i].get());
						}
						println("");
					}
				}
			}
			else {
				if (cmd == "prefix") {
					if (line >> arg) {
						auto answer = vocab.prefix(arg);
						for (auto&& x : answer) {
							print("{}:", x.first);
							for (auto&& y : x.second) {
								print(" {}", y.get());
							}
							println("");
						}
					}
				}
			}
		}
	}
}

