#include "psortLib.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;


int main(int argc, char ** argv) {
	vector<string> args(argv + 1, argv + argc); //first without triggers only columns
	string line;
	PolySorter ps; //need to enable different separator
	ps.setupColumnTypes(args);
	while (getline(cin, line)){
		ps.addRow(line);
	}
	ps.printAll();
}