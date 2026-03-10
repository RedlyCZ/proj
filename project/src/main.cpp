#include <iostream>
#include <string>
#include "apiMiddleman.hpp"
#include "runtimePortfolio.hpp"
#include "snapshot.hpp"
#include <chrono>

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
    printResult("Crypto Price (Bitcoin)", cryptoChannel.getActivePrice("bitcoin"), "USD");
    
    std::cout << "\nTests Complete.\n";
}

void testRTPortfolioBasic() {
    RTPortfolio pfOne;
    pfOne.buyInstrument(instrumentType::STOCK, "AAPL", 5);
    pfOne.buyInstrument(instrumentType::STOCK, "GOOGL", 5);
    pfOne.buyInstrument(instrumentType::CASH, "EUR", 100);
    pfOne.buyInstrument(instrumentType::CASH, "GBP", 50);
    pfOne.buyInstrument(instrumentType::CRYPTO, "ETH", 0.01);
    pfOne.buyInstrument(instrumentType::CRYPTO, "BTC", 0.1);
    pfOne.loadActivePrices();
    pfOne.loadActiveYields();
    pfOne.printAllPositions();
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
    pf.setStoragePath("snapshots");
    chrono::year_month_day date1{ chrono::year{2026}, chrono::March, chrono::day{9} };
    pf.loadSnapshot(date1);
    pf.printAllPositions();
}

int main() {
    //testApi();


    //testRTPortfolioBasic();
    

    //testSnapshoterSaving();

    //testSnapshoterLoading();
    return 0;
}