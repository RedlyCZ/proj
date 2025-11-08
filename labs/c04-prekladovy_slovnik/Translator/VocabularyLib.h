#include <string>
#include <vector>
#include <map>
#include <functional>

#ifndef VocabularyLib
#define VocabularyLib

class TransVoc {
public:
	void add(const std::string& slovo , const std::string& cizi) {
		vocabulary.insert({ slovo, cizi });
	}
	void del(const std::string& slovo, const std::string& cizi) {
		vocabulary.erase(slovo);
	}
	std::vector<std::reference_wrapper<const std::string>> find(const std::string& slovo) const;
private:
	std::multimap<std::string, std::string> vocabulary;

};




#endif
