#include <vector>
#include <string>
#include <memory>
#include <sstream>

#ifndef mprocLib
#define mprocLib

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

class polyContainer {
public:
	void add(std::unique_ptr<AbstractVal> p) {
		polyVec.push_back(std::move(p));
	}
	std::string toString() {
		std::string rowString = "";
		for (auto&& element : polyVec) {
			rowString.append(element->toString());
		}
		return rowString;
	}
private:
	std::vector<std::unique_ptr<AbstractVal>> polyVec;
};



class PolySorter {
public:
	PolySorter(char sep = ' ') : separator(sep) {}
	std::vector<polyContainer> rowsDatabase;
	void setupColumnTypes(const std::vector<std::string>& clnTypes);
	void addRow(const std::string& inputRow);
	void sort(const std::vector<int>& sortClnOrder);
	void printAll();
	void writeAllToFile(const std::string& path);
private:
	int columnCount = -1; //for first row, to setup
	char separator;
	std::vector<std::string> columnTypes;
};

#endif