#include "userInterface.hpp"
#include "financialCalc.hpp"
#include "CLIPrinter.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <optional>
#include <unordered_map>
#include <functional>
#include <cctype>

using namespace std;
using namespace std::chrono;

// helper functions
namespace {
    // convert string to type
    optional<instrumentType> parseInstrumentType(string str) {
        ranges::transform(str, str.begin(), [](unsigned char c) { return std::tolower(c); });
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
        ranges::transform(str, str.begin(), [](unsigned char c) { return std::tolower(c); });
        return (str == "true" || str == "1" || str == "yes");
    }
}

void CLIManager::showAggregates() const {
    FinancialCalculator calc;
    CLIPrinter printer;

    double totalVal = calc.totalValue(portfolio);

    double stocksVal = calc.valueOfType(portfolio.getStocks());
    double cashesVal = calc.valueOfType(portfolio.getCashes());
    double cryptoVal = calc.valueOfType(portfolio.getCryptos());

    double totalPerf = calc.totalPerformance(portfolio).value_or(0.0);

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

    double totalPerf = calc.backtestTotalPerformance(portfolio, timeSince).value_or(0.0);

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

    auto rsiOpt = calc.calculateRSI(type, ticker, period);

    if (rsiOpt) {
        printer.printIndicatorRSI(ticker, *rsiOpt);
    }
    else {
        cout << "Failed to calculate RSI for " << ticker << ".\n";
    }
}

void CLIManager::calcBollinger(instrumentType type, const string& ticker) const {
    FinancialCalculator calc;
    CLIPrinter printer;

    auto bollingerStateOpt = calc.bollingerOverbought(type, ticker);

    if (bollingerStateOpt) {
        printer.printIndicatorBollinger(ticker, *bollingerStateOpt);
    }
    else {
        cout << "Failed to calculate Bollinger Bands for " << ticker << ".\n";
    }
}

void CLIManager::monteCarloChance(instrumentType type, const string& ticker, int duration, double targetPrice, bool hit) const {
    FinancialCalculator calc;
    CLIPrinter printer;

    auto chanceOpt = calc.monteCarloChance(type, ticker, duration, targetPrice, hit);

    if (chanceOpt) {
        printer.printMonteCarloResult(ticker, duration, targetPrice, *chanceOpt);
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

void CLIManager::savePF() {
    if (portfolio.saveSnapshot()) {
        cout << "Snapshot saved successfully.\n";
    }
    else {
        cout << "Failed to save snapshot.\n";
    }
}

void CLIManager::executeCommand(const string& cmd, istringstream& iss) {
    if (cmd == "exit") {
        failed = true;
    }
    else if (cmd == "help" || cmd == "?") {
        printHelp();
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

void CLIManager::interpretInputRow(const string& inputRow) {
    istringstream iss(inputRow);
    string cmd;

    if (!(iss >> cmd)) return; //empty row

    ranges::transform(cmd, cmd.begin(), [](unsigned char c) { return std::tolower(c); });

    executeCommand(cmd, iss);

    cout << "\n";
}

void CLIManager::printHelp() const {
    cout << "\n=================== COMMAND REFERENCE ===================\n";
    cout << "NOTE: Ensure 'config.json' with valid API keys is in the app directory.\n";
    cout << "* <type> must be: stock, cash, or crypto.\n";
    cout << "---------------------------------------------------------\n\n";

    cout << "1. Money & Trading:\n";
    cout << "  deposit <amount>                                  - Add USD to portfolio\n";
    cout << "  withdraw <amount>                                 - Remove USD from portfolio\n";
    cout << "  buy <type> <ticker> <quantity>                    - Buy an asset\n";
    cout << "  sell <type> <ticker> <quantity> [close:true/false]- Sell an asset\n\n";

    cout << "2. Data & Overview:\n";
    cout << "  update <prices|yields>                            - Fetch fresh API data\n";
    cout << "  show <aggregates|positions|performance>           - Print portfolio stats\n\n";

    cout << "3. Analysis & Prediction:\n";
    cout << "  backtest <YYYY-MM-DD>                             - Run historical backtest\n";
    cout << "  yield <years>                                     - Predict compound growth\n";
    cout << "  rsi <type> <ticker> <period>                      - Calculate RSI indicator\n";
    cout << "  bollinger <type> <ticker>                         - Check Bollinger Bands\n";
    cout << "  montecarlo <type> <ticker> <days> <price> <hit>   - Run Monte Carlo sim (hit:true/false)\n\n";

    cout << "4. File Management & System:\n";
    cout << "  setpath <folder_path>                             - Set save location\n";
    cout << "  save                                              - Save snapshot (JSON)\n";
    cout << "  load <YYYY-MM-DD>                                 - Load snapshot\n";
    cout << "  help, ?                                           - Show this message\n";
    cout << "  exit                                              - Quit application\n";
    cout << "=========================================================\n";
}