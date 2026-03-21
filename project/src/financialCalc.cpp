#include "financialCalc.hpp"
#include "runtimePortfolio.hpp"
#include "apiMiddleman.hpp"
#include <cmath>
#include <random>

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

double FinancialCalculator::monteCarloChance(instrumentType type, const string& ticker, int duration, double price, bool hit) {
    //this method is kind of longer than i would like, but decomposition would make it even harder to read

    if (duration <= 0 || price <= 0.0) {
        return -1.0;
    }

    vector<double> histPrices;
    int fetchDays = 252; // one year data is ideal for approximation

    switch (type) {
    case(instrumentType::STOCK): {
        StockDataChannel stockApi;
        histPrices = stockApi.getHistoricalPrices(ticker, fetchDays);
        break;
    }
    case(instrumentType::CRYPTO): {
        CryptoDataChannel cryptoApi;
        histPrices = cryptoApi.getHistoricalPrices(ticker, fetchDays);
        break;
    }
    case(instrumentType::CASH): {
        // Cash historicals are not yet implemented in CashDataChannel
        return -1.0;
    }
    default: {
        return -1.0;
    }
    }

    if (histPrices.size() < 2) {
        return -1.0; //not enough data
    }

    // calculate historical metrics
    double sumReturns = 0.0;
    vector<double> returns;
    returns.reserve(histPrices.size() - 1);

    for (size_t i = 1; i < histPrices.size(); ++i) {
        double dailyReturn = log(histPrices[i] / histPrices[i - 1]);
        returns.push_back(dailyReturn);
        sumReturns += dailyReturn;
    }

    double meanReturn = sumReturns / returns.size();
    double varianceSum = 0.0;
    for (double r : returns) {
        varianceSum += (r - meanReturn) * (r - meanReturn);
    }
    double variance = varianceSum / returns.size();
    double stdDev = sqrt(variance);

    // monte carlo itself
    int numSimulations = 10000;
    int successCount = 0;
    double currentPrice = histPrices.back();
    bool isTargetAbove = (price > currentPrice);
    double drift = meanReturn - (variance / 2.0);

    std::mt19937 gen(std::random_device{}());
    std::normal_distribution<double> dist(0.0, 1.0);

    //simulation
    for (int i = 0; i < numSimulations; ++i) {
        double simulatedPrice = currentPrice;
        bool pathHit = false;

        for (int d = 0; d < duration; ++d) {
            double z = dist(gen);
            simulatedPrice *= std::exp(drift + stdDev * z);

            if (hit) {
                // hit checked mid simulation
                if ((isTargetAbove && simulatedPrice >= price) ||
                    (!isTargetAbove && simulatedPrice <= price)) {
                    pathHit = true;
                    break;
                }
            }
        }

        //evaluation
        if (hit) { //hit only
            if (pathHit) {
                successCount++;
            }
        }
        else { //price above in the end
            if ((isTargetAbove && simulatedPrice >= price) ||
                (!isTargetAbove && simulatedPrice <= price)) {
                successCount++;
            }
        }
    }
    return static_cast<double>(successCount) / numSimulations;
}