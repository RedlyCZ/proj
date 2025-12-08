#include "psortLib.h"
#include <string>
#include <sstream>
#include <print>
#include <algorithm>
#include <iostream>
#include <fstream>

using namespace std;

vector<string> PolySorter::splitString(const string& input, char sep) {
	vector<string> returnVec;
	string tempString = "";
	for (size_t i = 0; i < input.length(); i++) {
		if (input[i] != sep) {
			tempString += input[i];
		}
		else {
			returnVec.push_back(tempString);
			tempString = "";
		}
	}
	returnVec.push_back(tempString);
	return returnVec;
}

void PolySorter::addRow(const string& rowString) {
	PolyContainer container;
	vector<string> chunks = splitString(rowString, separator);
	if (firstRowAdjustment) {
		while (chunks.size() > columnTypes.size()) {
			columnTypes.push_back("string"); //matrix width is determined by first row, if its longer deduced from the types
		}
	}
	else {
		if (chunks.size() > columnTypes.size()) {
			error = true;
			cerr << "matrix is not rectangular";
		}
	}
	for (size_t i = 0; i < chunks.size(); i++) {
		if (columnTypes[i] == "string") { //here this is really ugly, but i didnt find a way to reliably convert string to all specific types
			container.add(make_unique<Val<string>>(chunks[i]));
		}
		if (columnTypes[i] == "int") {
			try {
				size_t pos;
				int val = stoi(chunks[i], &pos);
				if (pos != chunks[i].length()) { // check if we used the whole string
					throw invalid_argument("Trailing characters");
				}
				container.add(make_unique<Val<int>>(val));
			}
			catch (const std::exception&) {
				error = true;
				cerr << "column type error";
				break;
			}
		}
	}
	rowsDatabase.push_back(move(container));
	firstRowAdjustment = false;
}

void PolySorter::printAll() { //maybe return the separators in the strings, that may need to modify toString
	for (auto&& row : rowsDatabase) {
		println("{}", row.toString(string (1, separator)));
	}
}

void PolySorter::setupColumnTypes(const vector<string>& clnTypes) {
	for (const auto& element : clnTypes) {
		try {
			if (element.length() < 2) {
				throw invalid_argument("Argument too short");
			}
			string numPart = element.substr(1);
			size_t pos;
			int number = stoi(numPart, &pos);
			if (pos != numPart.length()) {
				throw invalid_argument("Trailing garbage");
			}
			while (columnTypes.size() < static_cast<size_t>(number)) {
				columnTypes.push_back("string");
			}

			string identifier = element.substr(0, 1);
			if (identifier == "N") {
				columnTypes[number - 1] = "int";
			}

			//here more types if needed

		}
		catch (...) {
			error = true;
			cerr << "error: invalid column definition: " << element << endl;
			return;
		}
	}
}

class polyComparator {
public:
	polyComparator(const vector<int>& sortClnOrder) : order(sortClnOrder) {}
	bool operator()(const PolyContainer& left, const PolyContainer& right) const {
		for (auto&& column : order) {
			if ((*(left.polyVec[column - 1])) < (*(right.polyVec[column - 1]))) {
				return true;
			}
			else {
				if ((*(right.polyVec[column - 1])) < (*(left.polyVec[column - 1]))) {
					return false;
				}
			}
		}
		return false; //if everything compared is the same
	}

private:
	const vector<int>& order;
};

void PolySorter::sortDB(const vector<string>& sortClnOrderRaw) {
	vector<int> sortClnOrder;
	for (auto&& element : sortClnOrderRaw) {
		int number = stoi(element.substr(1));
		sortClnOrder.push_back(number);
	}
	sort(rowsDatabase.begin(), rowsDatabase.end(), polyComparator{ sortClnOrder });
}

void PolySorter::writeAllToFile(const string& path) {
	ofstream outputFile(path);

	if (!outputFile.is_open()) {
		cerr << "error: Cannot open output file: " << path << endl;
		error = true;
		return;
	}

	for (auto&& row : rowsDatabase) {
		outputFile << row.toString(string(1, separator)) << "\n";
	}
}

void PolySorter::readRowsFromFile(const string& path) {
	ifstream inputFile(path);

	if (!inputFile.is_open()) {
		cerr << "error: Cannot open input file: " << path << endl;
		error = true;
		return;
	}

	string line;
	while (getline(inputFile, line)) {
		addRow(line);
		if (error) {
			break;
		}
	}
}