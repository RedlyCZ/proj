#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <print>

#ifndef psortLib
#define psortLib

class AbstractVal {
public:
	virtual bool operator<(const AbstractVal& rhs) const = 0;
	virtual ~AbstractVal() = default;
	virtual std::string toString() const = 0;
};

template <typename T>
class Val : public AbstractVal {
public:
	Val(T x) : x_(x) {}
	virtual bool operator<(const AbstractVal& rhs) const override {
		const Val<T>& rightSide = static_cast<const Val<T>&>(rhs);
		return this->x_ < rightSide.x_;
	}
	virtual std::string toString() const override {
		std::ostringstream oss;
		oss << x_;
		return oss.str();
	}
private:
	T x_;
};

class PolyContainer {
public:
	void add(std::unique_ptr<AbstractVal> p) {
		polyVec.push_back(std::move(p));
	}
	std::string toString(const std::string& sep) {
		std::string rowString = "";
		std::string elementsep = "";
		for (auto&& element : polyVec) {
			rowString.append(elementsep);
			rowString.append(element->toString());
			elementsep = sep;
		}
		return rowString;
	}
	std::vector<std::unique_ptr<AbstractVal>> polyVec;
};



class PolySorter {
public:
	PolySorter(char sep = ' ') : separator(sep) {}
	std::vector<PolyContainer> rowsDatabase;
	void setupColumnTypes(const std::vector<std::string>& clnTypes);
	void addRow(const std::string& inputRow);
	void sortDB(const std::vector<std::string>& sortClnOrderRaw);
	void printAll();
	void writeAllToFile(const std::string& path);
	bool error = false;
	void readRowsFromFile(const std::string& path);
private:
	int firstRowAdjustment = true;
	char separator;
	std::vector<std::string> columnTypes;
	std::vector<std::string> splitString(const std::string& input, char sep);

};

#endif