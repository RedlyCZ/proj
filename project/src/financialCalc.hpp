#ifndef FINANCIALCALC
#define FINANCIALCALC

#include <vector>
#include <string>
#include <unordered_map>
#include "runtimePortfolio.hpp"
#include <chrono>
#include <optional>


class RTPortfolio;
struct instrumentPosition;

struct perfRatios {
    std::unordered_map<std::string, double> stockReturns;       //ticker, return
    std::unordered_map<std::string, double> cashReturns;
    std::unordered_map<std::string, double> cryptoReturns;
};

class FinancialCalculator {
public:
    double totalValue(const RTPortfolio& portfolio);
    double valueOfType(const std::vector<instrumentPosition>& container);
    perfRatios performance(const RTPortfolio& portfolio);
    perfRatios fixedYield(const RTPortfolio& portfolio, double years);
    perfRatios backtestPerformace(const RTPortfolio& portfolio, const std::chrono::year_month_day& startDate);
    std::optional<double> totalPerformance(const RTPortfolio& portfolio);
    std::optional<double> backtestTotalPerformance(const RTPortfolio& portfolio, const std::chrono::year_month_day& startDate);
    std::optional<double> calculateRSI(instrumentType type, const std::string& ticker, int period);
    std::optional<double> monteCarloChance(instrumentType type, const std::string& ticker, int duration, double price, bool hit);
    std::optional<int> bollingerOverbought(instrumentType type, const std::string& ticker);
};

#endif