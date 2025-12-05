#include "psortLib.h"
#include <string>
#include <sstream>
#include <print>

using namespace std;

void PolySorter::addRow(const string& rowString) {
	polyContainer container;
	string token;
	istringstream tokenStream(rowString);
	int i = 0;
	while (getline(tokenStream, token, separator)) {
		if (columnTypes[i] == "string") { //here this is really ugly, but i didnt find a way to reliably convert string to all specific types
			container.add(make_unique<Val<string>>(token));
		}
		if (columnTypes[i] == "int") {
			container.add(make_unique<Val<int>>(stoi(token)));
		}
		++i;
	}
	rowsDatabase.push_back(move(container));
}

void PolySorter::printAll() { //maybe return the separators in the strings, that may need to modify toString
	for (auto&& row : rowsDatabase) {
		println("{}", row.toString());
	}
}

void PolySorter::setupColumnTypes(const vector<string>& clnTypes) {
	for (auto&& element : clnTypes) {
		int number = stoi(element.substr(1));
		while (columnTypes.size() < number) {
			columnTypes.push_back("string");
		}
		string identifier = element.substr(0, 1);
		if (identifier == "N") { //also pretty ugly but also hard to do otherway
			columnTypes[number - 1] = "int";
		}
	}
}

void PolySorter::sort(const vector<int>& sortClnOrder) {

}

void PolySorter::writeAllToFile(const string& path) {

}