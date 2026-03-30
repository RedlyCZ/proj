#ifndef FINANCIALCALC
#define FINANCIALCALC

#include <vector>
#include <string>
#include <unordered_map>
#include "runtimePortfolio.hpp"
#include <chrono>


class RTPortfolio;

struct instrumentPosition;

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
	double totalPerformance(const RTPortfolio& portfolio);
	perfRatios backtestPerformace(const RTPortfolio& portfolio, const std::chrono::year_month_day& startDate);
	double backtestTotalPerformance(const RTPortfolio& portfolio, const std::chrono::year_month_day& startDate);
	perfRatios fixedYield(const RTPortfolio& portfolio, double years);
	double calculateRSI(instrumentType type, const std::string& ticker, int period);
	double monteCarloChance(instrumentType type, const std::string& ticker, int duration, double price, bool hit);
	int bollingerOverbought(instrumentType type, const std::string& ticker); //0 is fair, -1 slightly oversold, 2 very overbought etc. [-2;2]
};


#endif