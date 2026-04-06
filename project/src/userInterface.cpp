#include "userInterface.hpp"
#include "financialCalc.hpp"
#include "CLIPrinter.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <optional>

using namespace std;
using namespace std::chrono;

// helper functions
namespace {
    // convert string to type
    optional<instrumentType> parseInstrumentType(string str) {
        ranges::transform(str, str.begin(), ::tolower);
        if (str == "stock") return instrumentType::STOCK;
        if (str == "cash") return instrumentType::CASH;
        if (str == "crypto") return instrumentType::CRYPTO;
        return nullopt;
    }

    // chrono parsing
    optional<year_month_day> parseDateString(const string& dateStr) {
        istringstream in{ dateStr };
        year_month_day ymd;
        if (in >> parse("%F", ymd) && ymd.ok()) {
            return ymd;
        }
        return nullopt;
    }

    // bool parsing
    bool parseBool(string str) {
        ranges::transform(str, str.begin(), ::tolower);
        return (str == "true" || str == "1" || str == "yes");
    }
}

void CLIManager::showAggregates() const {
    FinancialCalculator calc;
    CLIPrinter printer;

    double totalVal = calc.totalValue(portfolio);
    double stocksVal = calc.valueOfType(portfolio.stocks);
    double cashesVal = calc.valueOfType(portfolio.cashes);
    double cryptoVal = calc.valueOfType(portfolio.cryptos);
    double totalPerf = calc.totalPerformance(portfolio);

    printer.printCumulativeMetrics(totalVal, stocksVal, cashesVal, cryptoVal, totalPerf);
}

void CLIManager::showPositions() const {
    CLIPrinter printer;
    printer.printPortfolioPositions(portfolio, false); //false -> without performance
}

void CLIManager::showPerformance() const {
    FinancialCalculator calc;
    CLIPrinter printer;

    perfRatios perf = calc.performance(portfolio);
    printer.printPerformanceReport(perf);
    printer.printPortfolioPositions(portfolio, true);
}

void CLIManager::backtestPortfolio(const year_month_day& timeSince) const {
    FinancialCalculator calc;
    CLIPrinter printer;

    perfRatios perf = calc.backtestPerformace(portfolio, timeSince);
    double totalPerf = calc.backtestTotalPerformance(portfolio, timeSince);

    printer.printBacktestResult(perf, totalPerf, timeSince);
}

void CLIManager::buy(instrumentType type, const string& newTicker, double newQuantity) {
    double spent = portfolio.buyInstrument(type, newTicker, newQuantity);
    if (spent > 0) {
        cout << "Successfully bought " << newQuantity << " of " << newTicker << " for " << spent << " USD.\n";
    }
}

void CLIManager::sell(instrumentType type, const string& newTicker, double newQuantity, bool closePosition) {
    double gained = portfolio.sellInstrument(type, newTicker, newQuantity, closePosition);
    if (gained > 0) {
        cout << "Successfully sold " << (closePosition ? "all" : to_string(newQuantity))
            << " of " << newTicker << " gaining " << gained << " USD.\n";
    }
}

void CLIManager::depositUSD(double quantity) {
    if (portfolio.depositCash(quantity)) {
        cout << "Successfully deposited " << quantity << " USD.\n";
    }
}

void CLIManager::cashoutUSD(double quantity) {
    if (portfolio.withdrawCash(quantity)) {
        cout << "Successfully withdrew " << quantity << " USD.\n";
    }
}

void CLIManager::fixYieldPrediction(double years) const {
    FinancialCalculator calc;
    CLIPrinter printer;

    perfRatios predictions = calc.fixedYield(portfolio, years);
    printer.printYieldPrediction(predictions, years);
}

void CLIManager::calcRSI(instrumentType type, const string& ticker, int period) const {
    FinancialCalculator calc;
    CLIPrinter printer;

    double rsi = calc.calculateRSI(type, ticker, period);
    if (rsi >= 0) {
        printer.printIndicatorRSI(ticker, rsi);
    }
    else {
        cout << "Failed to calculate RSI for " << ticker << ".\n";
    }
}

void CLIManager::calcBollinger(instrumentType type, const string& ticker) const {
    FinancialCalculator calc;
    CLIPrinter printer;

    int bollingerState = calc.bollingerOverbought(type, ticker);
    if (bollingerState != -10) {
        printer.printIndicatorBollinger(ticker, bollingerState);
    }
    else {
        cout << "Failed to calculate Bollinger Bands for " << ticker << ".\n";
    }
}

void CLIManager::monteCarloChance(instrumentType type, const string& ticker, int duration, double targetPrice, bool hit) const {
    FinancialCalculator calc;
    CLIPrinter printer;

    double chance = calc.monteCarloChance(type, ticker, duration, targetPrice, hit);
    if (chance >= 0) {
        printer.printMonteCarloResult(ticker, duration, targetPrice, chance);
    }
    else {
        cout << "Failed to run Monte Carlo simulation for " << ticker << ".\n";
    }
}

void CLIManager::loadPF(const year_month_day& date) {
    if (portfolio.loadSnapshot(date)) {
        cout << "Snapshot loaded successfully.\n";
    }
    else {
        cout << "Failed to load snapshot.\n";
    }
}

void CLIManager::savePF() const {
    // Requires a mutable cast or portfolio to be modified if saveSnapshot wasn't const. 
    // Since saveSnapshot operates on the instance, we cast constness away locally just for the CLI call,
    // assuming saving doesn't structurally alter the portfolio state.
    if (const_cast<RTPortfolio&>(portfolio).saveSnapshot()) {
        cout << "Snapshot saved successfully.\n";
    }
    else {
        cout << "Failed to save snapshot.\n";
    }
}

void CLIManager::interpretInputRow(const string& inputRow) { 
    //user command interpreter
    istringstream iss(inputRow);
    string cmd;
    if (!(iss >> cmd)) return;

    ranges::transform(cmd, cmd.begin(), ::tolower);

    if (cmd == "exit") {
        failed = true;
    }
    else if (cmd == "update") {
        string target;
        iss >> target;
        if (target == "prices") {
            if (portfolio.loadActivePrices()) cout << "Prices updated successfully.\n";
            else cout << "Warning: Some prices failed to update.\n";
        }
        else if (target == "yields") {
            if (portfolio.loadActiveYields()) cout << "Yields updated successfully.\n";
            else cout << "Warning: Some yields failed to update.\n";
        }
    }
    else if (cmd == "show") {
        string target;
        iss >> target;
        if (target == "aggregates") showAggregates();
        else if (target == "positions") showPositions();
        else if (target == "performance") showPerformance();
        else cout << "Invalid show target. Use: aggregates, positions, performance.\n";
    }
    else if (cmd == "buy" || cmd == "sell") {
        string typeStr, ticker;
        double quantity = 0;
        if (iss >> typeStr >> ticker >> quantity) {
            auto type = parseInstrumentType(typeStr);
            if (!type) {
                cout << "Invalid instrument type. Use: stock, cash, crypto.\n";
                return;
            }
            if (cmd == "buy") {
                buy(*type, ticker, quantity);
            }
            else {
                string closeStr;
                bool closePos = false;
                if (iss >> closeStr) closePos = parseBool(closeStr);
                sell(*type, ticker, quantity, closePos);
            }
        }
        else {
            cout << "Usage: " << cmd << " <type> <ticker> <quantity> " << (cmd == "sell" ? "[close:true/false]" : "") << "\n";
        }
    }
    else if (cmd == "deposit") {
        double amount = 0;
        if (iss >> amount) depositUSD(amount);
        else cout << "Usage: deposit <amount>\n";
    }
    else if (cmd == "withdraw") {
        double amount = 0;
        if (iss >> amount) cashoutUSD(amount);
        else cout << "Usage: withdraw <amount>\n";
    }
    else if (cmd == "setpath") {
        string path;
        if (iss >> path) {
            portfolio.setStoragePath(path);
            cout << "Storage path set to: " << path << "\n";
        }
    }
    else if (cmd == "save") {
        savePF();
    }
    else if (cmd == "load" || cmd == "backtest") {
        string dateStr;
        if (iss >> dateStr) {
            auto date = parseDateString(dateStr);
            if (!date) {
                cout << "Invalid date format. Use YYYY-MM-DD.\n";
                return;
            }
            if (cmd == "load") loadPF(*date);
            else backtestPortfolio(*date);
        }
        else {
            cout << "Usage: " << cmd << " <YYYY-MM-DD>\n";
        }
    }
    else if (cmd == "yield") {
        double years;
        if (iss >> years) fixYieldPrediction(years);
        else cout << "Usage: yield <years>\n";
    }
    else if (cmd == "rsi") {
        string typeStr, ticker;
        int period;
        if (iss >> typeStr >> ticker >> period) {
            auto type = parseInstrumentType(typeStr);
            if (type) calcRSI(*type, ticker, period);
            else cout << "Invalid instrument type.\n";
        }
        else {
            cout << "Usage: rsi <type> <ticker> <period>\n";
        }
    }
    else if (cmd == "bollinger") {
        string typeStr, ticker;
        if (iss >> typeStr >> ticker) {
            auto type = parseInstrumentType(typeStr);
            if (type) calcBollinger(*type, ticker);
            else cout << "Invalid instrument type.\n";
        }
        else {
            cout << "Usage: bollinger <type> <ticker>\n";
        }
    }
    else if (cmd == "montecarlo") {
        string typeStr, ticker, hitStr;
        int duration;
        double targetPrice;
        if (iss >> typeStr >> ticker >> duration >> targetPrice >> hitStr) {
            auto type = parseInstrumentType(typeStr);
            if (type) monteCarloChance(*type, ticker, duration, targetPrice, parseBool(hitStr));
            else cout << "Invalid instrument type.\n";
        }
        else {
            cout << "Usage: montecarlo <type> <ticker> <duration_days> <target_price> <hit:true/false>\n";
        }
    }
    else {
        cout << "Unknown command.\n";
    }
}