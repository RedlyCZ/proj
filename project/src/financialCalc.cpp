#include "financialCalc.hpp"
#include "runtimePortfolio.hpp"
#include "apiMiddleman.hpp"
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

double FinancialCalculator::calculateRSI(instrumentType type, const string& ticker, int period) {
    if (period <= 0) {
        return -1.0;
    }

    vector<double> prices;
    //fetching more history for a stabilization (wilders smoothing)
    int fetchDays = period * 4;

    switch (type) {
    case(instrumentType::STOCK): {
        StockDataChannel stockApi;
        prices = stockApi.getHistoricalPrices(ticker, fetchDays);
        break;
    }
    case(instrumentType::CRYPTO): {

    
        CryptoDataChannel cryptoApi;
        prices = cryptoApi.getHistoricalPrices(ticker, fetchDays);
        break;
    }
    default: {
        return -1.0; //not implemented, RSI cant be negative
    }
    }

    if (prices.size() <= period) {
        return -1.0;
    }

    double avgGain = 0.0;
    double avgLoss = 0.0;

    for (int i = 1; i <= period; ++i) {
        double change = prices[i] - prices[i - 1];
        if (change > 0) {
            avgGain += change;
        }
        else {
            avgLoss += std::abs(change);
        }
    }

    avgGain /= period;
    avgLoss /= period;

    // wilders smoothing
    for (size_t i = period + 1; i < prices.size(); ++i) {
        double change = prices[i] - prices[i - 1];
        double gain = (change > 0) ? change : 0.0;
        double loss = (change < 0) ? std::abs(change) : 0.0;

        avgGain = ((avgGain * (period - 1)) + gain) / period;
        avgLoss = ((avgLoss * (period - 1)) + loss) / period;
    }

    //rsi computation
    if (avgLoss == 0.0) {
        return 100.0; // if no losses RSI is by definition 100
    }

    double rs = avgGain / avgLoss;
    return (100.0 - (100.0 / (1.0 + rs)));
}