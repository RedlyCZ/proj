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
    total += valueOfType(portfolio.getStocks());
    total += valueOfType(portfolio.getCashes());
    total += valueOfType(portfolio.getCryptos());
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

    for (const auto& pos : portfolio.getStocks()) {
        result.stockReturns[pos.ticker] = calculateReturn(pos.activePrice, pos.averageBuyPrice);
    }

    for (const auto& pos : portfolio.getCashes()) {
        result.cashReturns[pos.ticker] = calculateReturn(pos.activePrice, pos.averageBuyPrice);
    }

    for (const auto& pos : portfolio.getCryptos()) {
        result.cryptoReturns[pos.ticker] = calculateReturn(pos.activePrice, pos.averageBuyPrice);
    }

    return result;
}

perfRatios FinancialCalculator::fixedYield(const RTPortfolio& portfolio, double years) {
    perfRatios result;

    //we use quartal compounding for everything regardless of reality, cause its the most common and results wouldnt vary much otherwise (and would be hard to get data)
    const double compoundsPerYear = 4;
    auto calculateCompoundYield = [](double annualYield, double years, const double compoundsPerYear) -> double {
        if (annualYield <= 0.0 || years <= 0.0) {
            return 0.0;
        }
        double periods = years * compoundsPerYear;
        double ratePerPeriod = annualYield / compoundsPerYear;

        return std::pow(1.0 + ratePerPeriod, periods) - 1.0;    //standard formula for compound interest
        };

    //containing reinvestments
    for (const auto& pos : portfolio.getStocks()) {
        result.stockReturns[pos.ticker] = calculateCompoundYield(pos.yield, years, compoundsPerYear);
    }

    for (const auto& pos : portfolio.getCashes()) {
        result.cashReturns[pos.ticker] = calculateCompoundYield(pos.yield, years, compoundsPerYear);
    }

    //Didnt find api for stacking yields, so this goes without calculation, but if yields ARE given, will work
    for (const auto& pos : portfolio.getCryptos()) {
        result.cryptoReturns[pos.ticker] = calculateCompoundYield(pos.yield, years, compoundsPerYear);
    }

    return result;
}

std::optional<double> FinancialCalculator::totalPerformance(const RTPortfolio& pf) {
    double netInvested = pf.getTotalDeposited() - pf.getTotalWithdrawn();

    if (netInvested <= 0.0) {
        return std::nullopt;
    }

    double totalValueNow = totalValue(pf);
    return (totalValueNow - netInvested) / netInvested;
}

std::optional<double> FinancialCalculator::calculateRSI(instrumentType type, const string& ticker, int period) {
    if (period <= 0) {
        return std::nullopt;
    }

    vector<double> prices;
    int fetchDays = period * 4;

    switch (type) {
    case(instrumentType::STOCK): {
        StockDataChannel stockApi;
        prices = stockApi.getHistoricalPrices(ticker, fetchDays).value_or(std::vector<double>{});
        break;
    }
    case(instrumentType::CRYPTO): {
        CryptoDataChannel cryptoApi;
        prices = cryptoApi.getHistoricalPrices(ticker, fetchDays).value_or(std::vector<double>{});
        break;
    }
    default: {
        return std::nullopt;
    }
    }

    if (prices.size() <= period) {
        return std::nullopt;
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

    for (size_t i = period + 1; i < prices.size(); ++i) {
        double change = prices[i] - prices[i - 1];
        double gain = (change > 0) ? change : 0.0;
        double loss = (change < 0) ? std::abs(change) : 0.0;

        avgGain = ((avgGain * (period - 1)) + gain) / period;
        avgLoss = ((avgLoss * (period - 1)) + loss) / period;
    }

    if (avgLoss == 0.0) {
        return 100.0;
    }

    double rs = avgGain / avgLoss;
    return (100.0 - (100.0 / (1.0 + rs)));
}

std::optional<double> FinancialCalculator::monteCarloChance(instrumentType type, const string& ticker, int duration, double price, bool hit) {
    if (duration <= 0 || price <= 0.0) {
        return std::nullopt;
    }

    vector<double> histPrices;
    int fetchDays = 365;        //nmb of days in a year (could be 252 cause thats the nmb of trading days in year for US exchanges)

    switch (type) {
    case(instrumentType::STOCK): {
        StockDataChannel stockApi;
        histPrices = stockApi.getHistoricalPrices(ticker, fetchDays).value_or(std::vector<double>{});
        break;
    }
    case(instrumentType::CRYPTO): {
        CryptoDataChannel cryptoApi;
        histPrices = cryptoApi.getHistoricalPrices(ticker, fetchDays).value_or(std::vector<double>{});
        break;
    }
    case(instrumentType::CASH): {
        return std::nullopt;
    }
    default: {
        return std::nullopt;
    }
    }

    if (histPrices.size() < 2) {
        return std::nullopt;
    }

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

    int numSimulations = 10000;
    int successCount = 0;
    double currentPrice = histPrices.back();
    bool isTargetAbove = (price > currentPrice);
    double drift = meanReturn - (variance / 2.0);

    std::mt19937 gen(std::random_device{}());
    std::normal_distribution<double> dist(0.0, 1.0);

    for (int i = 0; i < numSimulations; ++i) {
        double simulatedPrice = currentPrice;
        bool pathHit = false;

        for (int d = 0; d < duration; ++d) {
            double z = dist(gen);
            simulatedPrice *= std::exp(drift + stdDev * z);

            if (hit) {
                if ((isTargetAbove && simulatedPrice >= price) ||
                    (!isTargetAbove && simulatedPrice <= price)) {
                    pathHit = true;
                    break;
                }
            }
        }

        if (hit) {
            if (pathHit) {
                successCount++;
            }
        }
        else {
            if ((isTargetAbove && simulatedPrice >= price) ||
                (!isTargetAbove && simulatedPrice <= price)) {
                successCount++;
            }
        }
    }
    return static_cast<double>(successCount) / numSimulations;
}

std::optional<int> FinancialCalculator::bollingerOverbought(instrumentType type, const string& ticker) {
    int period = 20;
    vector<double> prices;

    switch (type) {
    case(instrumentType::STOCK): {
        StockDataChannel stockApi;
        prices = stockApi.getHistoricalPrices(ticker, period).value_or(std::vector<double>{});
        break;
    }
    case(instrumentType::CRYPTO): {
        CryptoDataChannel cryptoApi;
        prices = cryptoApi.getHistoricalPrices(ticker, period).value_or(std::vector<double>{});
        break;
    }
    case(instrumentType::CASH): {
        return std::nullopt;
    }
    default: {
        return std::nullopt;
    }
    }

    if (prices.size() < period) {
        return std::nullopt;
    }

    double sum = 0.0;
    for (double p : prices) {
        sum += p;
    }
    double sma = sum / prices.size();

    double varianceSum = 0.0;
    for (double p : prices) {
        varianceSum += (p - sma) * (p - sma);
    }
    double variance = varianceSum / prices.size();
    double stdDev = std::sqrt(variance);

    double currentPrice = prices.back();

    if (currentPrice > sma + (2.0 * stdDev)) {
        return 2;
    }
    else if (currentPrice > sma + stdDev) {
        return 1;
    }
    else if (currentPrice < sma - (2.0 * stdDev)) {
        return -2;
    }
    else if (currentPrice < sma - stdDev) {
        return -1;
    }

    return 0;
}

perfRatios FinancialCalculator::backtestPerformace(const RTPortfolio& portfolio, const std::chrono::year_month_day& startDate) {
    perfRatios result;

    auto calculateReturn = [](double active, double historical) -> double {
        if (historical <= 0.0) {
            return 0.0; //zero div prevention
        }
        return (active / historical);
        };

    StockDataChannel stockApi;
    for (const auto& pos : portfolio.getStocks()) {
        double histPrice = stockApi.getHistoricalPriceByDate(pos.ticker, startDate).value_or(-1.0);
        result.stockReturns[pos.ticker] = calculateReturn(pos.activePrice, histPrice);
    }

    CashDataChannel cashApi;
    for (const auto& pos : portfolio.getCashes()) {
        double histPrice = cashApi.getHistoricalPriceByDate(pos.ticker, startDate).value_or(-1.0);
        result.cashReturns[pos.ticker] = calculateReturn(pos.activePrice, histPrice);
    }

    CryptoDataChannel cryptoApi;
    for (const auto& pos : portfolio.getCryptos()) {
        double histPrice = cryptoApi.getHistoricalPriceByDate(pos.ticker, startDate).value_or(-1.0);
        result.cryptoReturns[pos.ticker] = calculateReturn(pos.activePrice, histPrice);
    }

    return result;
}

std::optional<double> FinancialCalculator::backtestTotalPerformance(const RTPortfolio& pf, const std::chrono::year_month_day& startDate) {
    double totalActiveValue = 0.0;
    double totalHistoricalValue = 0.0;

    auto accumulateValues = [&totalActiveValue, &totalHistoricalValue, &startDate](auto& container, auto& apiChannel) {
        for (const auto& pos : container) {
            auto histPriceOpt = apiChannel.getHistoricalPriceByDate(pos.ticker, startDate);
            if (histPriceOpt && *histPriceOpt > 0.0) {
                totalActiveValue += (pos.activePrice * pos.quantity);
                totalHistoricalValue += (*histPriceOpt * pos.quantity);
            }
        }
        };

    StockDataChannel stockApi;
    accumulateValues(pf.getStocks(), stockApi);

    CashDataChannel cashApi;
    accumulateValues(pf.getCashes(), cashApi);

    CryptoDataChannel cryptoApi;
    accumulateValues(pf.getCryptos(), cryptoApi);

    if (totalHistoricalValue <= 0.0) {
        return std::nullopt;
    }

    return totalActiveValue / totalHistoricalValue;
}