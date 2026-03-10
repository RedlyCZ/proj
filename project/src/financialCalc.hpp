#ifndef FINANCIALCALC
#define FINANCIALCALC

#include <vector>
#include <string>
#include <unordered_map>


class RTPortfolio;

class instrumentPosition;

struct perfRatios {
	std::unordered_map<std::string, double> stockReturns;		//ticker, return
	std::unordered_map<std::string, double> cashReturns;
	std::unordered_map<std::string, double> cryptoReturns;
};

class FinancialCalculator {
public:
	double totalValue(const RTPortfolio& portfolio);
	double valueOfType(const std::vector<instrumentPosition>& container);
	perfRatios performance(const RTPortfolio& portfolio);
	perfRatios fixedYield(const RTPortfolio& portfolio, double years);
};


#endif