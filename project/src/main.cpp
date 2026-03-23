#include <iostream>
#include <string>
#include "apiMiddleman.hpp"
#include "runtimePortfolio.hpp"
#include "snapshot.hpp"
#include <chrono>
#include "CLIPrinter.hpp"
#include "financialCalc.hpp"

using namespace std;

// Helper function to print results cleanly
void printResult(const std::string& testName, double value, const std::string& unit = "") {
    std::cout << "TEST: " << testName << "\n";
    if (value == -1.0) {
        std::cout << "RESULT: [ERROR] API returned -1\n";
    }
    else {
        std::cout << "RESULT: " << value << " " << unit << "\n";
    }
    std::cout << "----------------------------------------\n";
}

void testApi() {
    std::cout << "========================================\n";
    std::cout << "    API MIDDLEMAN INTERFACE TESTER      \n";
    std::cout << "========================================\n\n";
    
    std::cout << "[ StockDataChannel ]\n";
    StockDataChannel stockChannel;
    printResult("Stock Price (AAPL)", stockChannel.getActivePrice("AAPL"), "USD");
    printResult("Stock Dividend (AAPL)", stockChannel.getActiveDividend("AAPL"), "USD");
    
    std::cout << "\n[ CashDataChannel ]\n";
    CashDataChannel cashChannel;
    printResult("Forex Rate (EUR -> USD)", cashChannel.getConversionRate("EUR"), "USD");
    printResult("Interest Rate (USD - Fed)", cashChannel.getInterestRate("USD"), "%");
    printResult("Interest Rate (CZK - PRIBOR)", cashChannel.getInterestRate("CZK"), "%");
    
    std::cout << "\n[ CryptoDataChannel ]\n";
    CryptoDataChannel cryptoChannel;
    printResult("Crypto Price (BTC)", cryptoChannel.getActivePrice("BTC"), "USD");
    
    std::cout << "\nTests Complete.\n";
}

void testRTPortfolioBasic() {
    RTPortfolio pfOne;
    CLIPrinter printer;
    pfOne.buyInstrument(instrumentType::STOCK, "AAPL", 5);
    pfOne.buyInstrument(instrumentType::STOCK, "GOOGL", 5);
    pfOne.buyInstrument(instrumentType::CASH, "EUR", 100);
    pfOne.buyInstrument(instrumentType::CASH, "GBP", 50);
    pfOne.buyInstrument(instrumentType::CRYPTO, "ETH", 0.01);
    pfOne.buyInstrument(instrumentType::CRYPTO, "BTC", 0.1);
    pfOne.loadActivePrices();
    pfOne.loadActiveYields();
    printer.printPortfolioPositions(pfOne, false);
}

void testSnapshoterSaving() {
    RTPortfolio pf;
    pf.buyInstrument(instrumentType::STOCK, "AAPL", 5);
    pf.buyInstrument(instrumentType::STOCK, "GOOGL", 5);
    pf.buyInstrument(instrumentType::CASH, "EUR", 100);
    pf.buyInstrument(instrumentType::CASH, "GBP", 50);
    pf.buyInstrument(instrumentType::CRYPTO, "ETH", 0.01);
    pf.buyInstrument(instrumentType::CRYPTO, "BTC", 0.1);
    pf.loadActivePrices();
    pf.loadActiveYields();
    pf.setStoragePath("snapshots");
    pf.saveSnapshot();
}

void testSnapshoterLoading() {
    RTPortfolio pf;
    CLIPrinter printer;
    pf.setStoragePath("snapshots");
    chrono::year_month_day date1{ chrono::year{2026}, chrono::March, chrono::day{8} };
    pf.loadSnapshot(date1);
    printer.printPortfolioPositions(pf, false);
    printer.printPortfolioPositions(pf, true);
}

void testCumulativeMetrics() {
    RTPortfolio pf;
    CLIPrinter printer;
    FinancialCalculator calc;
    pf.setStoragePath("snapshots");
    chrono::year_month_day date1{ chrono::year{2026}, chrono::March, chrono::day{8} };
    pf.loadSnapshot(date1);
    pf.loadActivePrices();
    printer.printCumulativeMetrics(calc.totalValue(pf), calc.valueOfType(pf.stocks), calc.valueOfType(pf.cashes), calc.valueOfType(pf.cryptos), calc.totalPerformance(pf));
}

void testHistoricalData() {
    std::cout << "=== Testing Historical Data APIs ===\n\n";
    StockDataChannel stockChannel;
    std::string stockTicker = "AAPL";
    int stockDays = 5;

    std::cout << "Fetching " << stockDays << " days of historical data for Stock: " << stockTicker << "\n";
    std::vector<double> stockPrices = stockChannel.getHistoricalPrices(stockTicker, stockDays);

    if (stockPrices.empty()) {
        std::cout << "Result: Failed to fetch stock prices or no data available.\n";
    }
    else {
        std::cout << "Result: Successfully fetched " << stockPrices.size() << " days of data.\n";
        std::cout << "Prices: ";
        for (double price : stockPrices) {
            std::cout << price << " ";
        }
        std::cout << "\n";
    }

    std::cout << "------------------------------------\n\n";
    CryptoDataChannel cryptoChannel;
    std::string cryptoTicker = "BTC";
    int cryptoDays = 5;

    std::cout << "Fetching " << cryptoDays << " days of historical data for Crypto: " << cryptoTicker << " (paired with USDT)\n";
    std::vector<double> cryptoPrices = cryptoChannel.getHistoricalPrices(cryptoTicker, cryptoDays);

    if (cryptoPrices.empty()) {
        std::cout << "Result: Failed to fetch crypto prices or no data available.\n";
    }
    else {
        std::cout << "Result: Successfully fetched " << cryptoPrices.size() << " days of data.\n";
        std::cout << "Prices: ";
        for (double price : cryptoPrices) {
            std::cout << price << " ";
        }
        std::cout << "\n";
    }

    std::cout << "====================================\n";
}

void testRSI() {
    FinancialCalculator calc;

    double stockRSI = calc.calculateRSI(instrumentType::STOCK, "AAPL", 14);
    printResult("RSI 14-day (AAPL - Stock)", stockRSI);

    double cryptoRSI = calc.calculateRSI(instrumentType::CRYPTO, "BTC", 14);
    printResult("RSI 14-day (BTC - Crypto)", cryptoRSI);
}

void testMonteCarlo() {
    FinancialCalculator calc;

    double aaplHit = calc.monteCarloChance(instrumentType::STOCK, "AAPL", 30, 250.0, true);
    printResult("Chance AAPL touches $250 within 30 days", aaplHit);

    double aaplNoHit = calc.monteCarloChance(instrumentType::STOCK, "AAPL", 30, 250.0, false);
    printResult("Chance AAPL finishes >= $250 exactly at day 30", aaplNoHit);

    double btcHit = calc.monteCarloChance(instrumentType::CRYPTO, "BTC", 60, 100000.0, true);
    printResult("Chance BTC touches $100k within 60 days", btcHit);

}

int main() {
    //testApi();


    //testRTPortfolioBasic();
    

    //testSnapshoterSaving();

    //testSnapshoterLoading();

    //testCumulativeMetrics();

    //testHistoricalData();

    //testRSI();

    //testMonteCarlo();

    return 0;
}