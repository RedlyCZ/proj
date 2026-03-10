#include "financialCalc.hpp"
#include "runtimePortfolio.hpp"
#include <cmath>

using namespace std;

double FinancialCalculator::valueOfType(const vector<instrumentPosition>& container) {
    double typeTotalValue = 0.0;
    for (auto&& pos : container) {
        typeTotalValue += (pos.quantity * pos.activePrice);
    }
    return typeTotalValue;
}

double FinancialCalculator::totalValue(const RTPortfolio& portfolio) {
    double total = 0.0;
    total += valueOfType(portfolio.stocks);
    total += valueOfType(portfolio.cashes);
    total += valueOfType(portfolio.cryptos);
    return total;
}

perfRatios FinancialCalculator::performance(const RTPortfolio& portfolio) {
    perfRatios result;

    auto calculateReturn = [](double active, double avgBuy) -> double {
        if (avgBuy <= 0.0) {
            return 0.0; // division by zero prevention
        }
        return (active / avgBuy);
        };

    for (const auto& pos : portfolio.stocks) {
        result.stockReturns[pos.ticker] = calculateReturn(pos.activePrice, pos.averageBuyPrice);
    }

    for (const auto& pos : portfolio.cashes) {
        result.cashReturns[pos.ticker] = calculateReturn(pos.activePrice, pos.averageBuyPrice);
    }

    for (const auto& pos : portfolio.cryptos) {
        result.cryptoReturns[pos.ticker] = calculateReturn(pos.activePrice, pos.averageBuyPrice);
    }

    return result;
}

perfRatios FinancialCalculator::fixedYield(const RTPortfolio& portfolio, double years) {
    perfRatios result;

    //we use quartal compounding for everything regardless of reality, cause its the most common and results wouldnt vary much otherwise (and would be hard to get data)
    const double compoundsPerYear = 4;
    auto calculateCompoundYield = [](double annualYield, double years) -> double {
        if (annualYield <= 0.0 || years <= 0.0) {
            return 0.0;
        }
        double periods = years * compoundsPerYear;
        double ratePerPeriod = annualYield / compoundsPerYear;

        return std::pow(1.0 + ratePerPeriod, periods) - 1.0;    //standard formula for compound interest
        };

    //containing reinvestments
    for (const auto& pos : portfolio.stocks) {
        result.stockReturns[pos.ticker] = calculateCompoundYield(pos.yield, years);
    }

    for (const auto& pos : portfolio.cashes) {
        result.cashReturns[pos.ticker] = calculateCompoundYield(pos.yield, years);
    }

    //Didnt find api for stacking yields, so this goes without calculation, but if yields ARE given, will work
    for (const auto& pos : portfolio.cryptos) {
        result.cryptoReturns[pos.ticker] = calculateCompoundYield(pos.yield, years);
    }

    return result;
}

double FinancialCalculator::totalPerformance(const RTPortfolio& pf) {
    //simple weighted profit calculation
    double totalActiveValue = 0.0;
    double totalInvestedValue = 0.0;

    auto accumulateValues = [&totalActiveValue, &totalInvestedValue](const std::vector<instrumentPosition>& container) {
        for (const auto& pos : container) {
            totalActiveValue += (pos.activePrice * pos.quantity);
            totalInvestedValue += (pos.averageBuyPrice * pos.quantity);
        }
        };

    accumulateValues(pf.stocks);
    accumulateValues(pf.cashes);
    accumulateValues(pf.cryptos);

    if (totalInvestedValue <= 0.0) {        // division by zero prevention
        return 0.0; 
    }

    return totalActiveValue / totalInvestedValue;
}