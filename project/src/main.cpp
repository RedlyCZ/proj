#include <iostream>
#include <string>
#include "apiMiddleman.hpp"

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

int main() {
    std::cout << "========================================\n";
    std::cout << "    API MIDDLEMAN INTERFACE TESTER      \n";
    std::cout << "========================================\n\n";

    // 1. Test Stock Data Channel
    // Wraps FinnHub (Price) and TwelveData (Dividend)
    std::cout << "[ StockDataChannel ]\n";
    stockDataChannel stockChannel;

    // Test 1: Get Stock Price (FinnHub) -> Using Apple (AAPL)
    printResult("Stock Price (AAPL)", stockChannel.getActivePrice("AAPL"), "USD");

    // Test 2: Get Stock Dividend (TwelveData) -> Using Apple (AAPL)
    // Note: If this returns 0, it might mean no recent dividend or API limit, 
    // but -1 indicates a connection/parsing error.
    printResult("Stock Dividend (AAPL)", stockChannel.getActiveDividend("AAPL"), "USD");


    std::cout << "\n[ CashDataChannel ]\n";
    cashDataChannel cashChannel;

    // Test 3: Get Forex Rate (Frankfurter) -> EUR to USD
    // The library defaults target to USD if not specified
    printResult("Forex Rate (EUR -> USD)", cashChannel.getConversionRate("EUR"), "USD");

    // Test 4: Get Global Interest Rate (ApiNinjas) -> USD (Fed)
    // This tests the logic that resolves "USD" to "United States Federal Reserve"
    printResult("Interest Rate (USD - Fed)", cashChannel.getInterestRate("USD"), "%");

    // Test 5: Get specific CZK Interest Rate (CNB Custom Implementation)
    // The library explicitly checks for "CZK" to switch to CnbChannel
    printResult("Interest Rate (CZK - PRIBOR)", cashChannel.getInterestRate("CZK"), "%");


    std::cout << "\n[ CryptoDataChannel ]\n";
    cryptoDataChannel cryptoChannel;

    // Test 6: Get Crypto Price (CoinGecko)
    // CoinGecko requires the full ID (e.g., "bitcoin") not the ticker ("BTC")
    printResult("Crypto Price (Bitcoin)", cryptoChannel.getActivePrice("bitcoin"), "USD");

    std::cout << "\nTests Complete.\n";
    return 0;
}