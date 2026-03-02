#include <iostream>
#include <string>
#include "apiMiddleman.hpp"
#include "runtimePortfolio.hpp"

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

int main() {
    //testApi();


    //testRTPortfolioBasic();
    
    return 0;
}