#include "psortLib.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
    string inputPath = "";
    string outputPath = "";
    char separator = ' ';
    vector<string> columnArgs;

    //args parsing
    for (int i = 1; i < argc; ++i) { //this loop would be hard to decompose, it also reports errors and stops program if needed
        string arg = argv[i];
        if (arg.rfind("-i", 0) == 0) { //starts with -i
            if (arg.length() > 2) {
                inputPath = arg.substr(2); //one arg structure without space
            }
            else if (i + 1 < argc) {
                inputPath = argv[++i];     //two arg structure, we read the second and then step over it
            }
            else {
                cerr << "error: missing argument for -i" << endl;
                return 1;
            }
        }
        else if (arg.rfind("-o", 0) == 0) {
            if (arg.length() > 2) {
                outputPath = arg.substr(2);
            }
            else if (i + 1 < argc) {
                outputPath = argv[++i];
            }
            else {
                cerr << "error: missing argument for -o" << endl;
                return 1;
            }
        }
        else if (arg.rfind("-s", 0) == 0) {
            if (arg.length() > 2) {
                separator = arg[2];
            }
            else if (i + 1 < argc) {
                string nextArg = argv[++i];
                if (!nextArg.empty()) separator = nextArg[0];
            }
            else {
                cerr << "error: missing separator" << endl;
                return 1;
            }
        }
        else {
            columnArgs.push_back(arg);
        }
    }

    //calling of library
    PolySorter ps(separator);
    ps.setupColumnTypes(columnArgs);

    if (!inputPath.empty()) { //reading from file enabled
        ps.readRowsFromFile(inputPath);
    }
    else { //read from stdin
        string line;
        while (getline(cin, line)) {
            ps.addRow(line);
            if (ps.error) break;
        }
    }

    if (!ps.error) {
        ps.sortDB(columnArgs);

        if (!outputPath.empty()) { //output to file
            ps.writeAllToFile(outputPath);
        }
        else {
            ps.printAll();
        }
    }
}