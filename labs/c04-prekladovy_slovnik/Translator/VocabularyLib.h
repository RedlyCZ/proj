#include <string>
#include <vector>
#include <map>
#include <functional>
#include <set>

#ifndef VocabularyLib
#define VocabularyLib

struct CustomComparator {
	bool operator()(const std::pair<std::string, std::string>& a, const std::pair<std::string, std::string>& b) const {
		if (a.first != b.first) {
			return a.first < b.first;
		}
		return a.second < b.second;
	}
};

class TransVoc {
public:
	void add(const std::string& slovo , const std::string& cizi) {
 		vocabulary.insert({ slovo, cizi });
		setOfKeys.insert(slovo);
	}
	void del(const std::string& slovo, const std::string& cizi) {
		vocabulary.erase({ slovo, cizi });
		if (this->find(slovo).size() == 0) {
			setOfKeys.erase(slovo);
		}
	}
	void del(const std::string& slovo);
	std::vector<std::reference_wrapper<const std::string>> find(const std::string& slovo) const;
	std::vector<std::pair<std::string, std::vector<std::reference_wrapper<const std::string>>>> prefix(const std::string& slovo) const;
private:
	std::set<std::pair<std::string, std::string>, CustomComparator> vocabulary;
	std::set<std::string> setOfKeys;
};




#endif
