#include <vector>
#include <string>
#include <memory>

#ifndef mprocLib
#define mprocLib

class AbstractVal {
public:
	virtual bool operator<(const AbstractVal& rhs) const = 0;
	virtual ~AbstractVal() = default;
};

template <typename T>
class Val : public AbstractVal {
public:
	Val(T x) : x_(x) {}
	virtual bool operator<(const AbstractVal& rhs) const override {
		const Val<T>& rightSide = static_cast<const Val<T>&>(rhs);
		return this->x_ < rightSide.x_;
	}
private:
	T x_;
};



class PolySorter {
public:
	std::vector<std::vector<unique_ptr<AbstractVal>>> rowsDatabase;
	void setupColumnTypes(std::vector<char> clnTypes);
	void addRow(std::string inputRow);
	void sort();
	void printAll();
	void writeAllToFile(const std::string& path);
private:
	int columnCount = -1; //for first row, to setup
};

#endif