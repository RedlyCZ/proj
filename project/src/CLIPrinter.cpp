#include "CLIPrinter.hpp"
#include "runtimePortfolio.hpp"
#include "financialCalc.hpp"
#include <iostream>
#include <string>
#include <iomanip>


using namespace std;

// Helper function to print the maps cleanly
namespace {
	void printReturnMap(const string& category, const std::unordered_map<string, double>& returns) {
		if (returns.empty()) return;

		cout << "  " << category << ":\n";
		for (const auto& [ticker, perf] : returns) {
			// Convert performance ratio to a percentage (e.g., 1.05 -> +5.00%)
			double percentage = (perf - 1.0) * 100.0;
			cout << "    - " << left << setw(8) << ticker << ": "
				<< right << setw(8) << fixed << setprecision(2) << percentage << "%\n";
		}
	}
}

void CLIPrinter::printStockPositions(const RTPortfolio& pf, bool perf) {
	cout << "Stocks:\n";
	if (perf) {
		for (size_t i = 0; i < pf.stocks.size(); i++) {
			double performance = pf.stocks[i].activePrice / pf.stocks[i].averageBuyPrice;
			cout << "Ticker: " << pf.stocks[i].ticker << " Active Price: " << pf.stocks[i].activePrice << " Performance: " << performance << " Value: " << pf.stocks[i].quantity*(pf.stocks[i].activePrice) << "\n";
		}
	}
	else {
		for (size_t i = 0; i < pf.stocks.size(); i++) {
			cout << "Ticker: " << pf.stocks[i].ticker << " Active Price: " << pf.stocks[i].activePrice << " Active Yield: " << pf.stocks[i].yield << " Quantity: " << pf.stocks[i].quantity << "\n";
		}
	}
}

void CLIPrinter::printCashPositions(const RTPortfolio& pf, bool perf) {
	cout << "Cashes:\n";
	if (perf) {
		for (size_t i = 0; i < pf.cashes.size(); i++) {
			double performance = pf.cashes[i].activePrice / pf.cashes[i].averageBuyPrice;
			cout << "Ticker: " << pf.cashes[i].ticker << " Active Price: " << pf.cashes[i].activePrice << " Performance: " << performance << " Value: " << pf.cashes[i].quantity * (pf.cashes[i].activePrice) << "\n";
		}
	}
	else {
		for (size_t i = 0; i < pf.cashes.size(); i++) {
			cout << "Ticker: " << pf.cashes[i].ticker << " Active Price: " << pf.cashes[i].activePrice << " Active Yield: " << pf.cashes[i].yield << " Quantity: " << pf.cashes[i].quantity << "\n";
		}
	}
}

void CLIPrinter::printCryptoPositions(const RTPortfolio& pf, bool perf) {
	cout << "Crypto:\n";
	if (perf) {
		for (size_t i = 0; i < pf.cryptos.size(); i++) {
			double performance = pf.cryptos[i].activePrice / pf.cryptos[i].averageBuyPrice;
			cout << "Name: " << pf.cryptos[i].ticker << " Active Price: " << pf.cryptos[i].activePrice << " Performance: " << performance << " Value: " << pf.cryptos[i].quantity * (pf.cryptos[i].activePrice) << "\n";
		}
	}
	else {
		for (size_t i = 0; i < pf.cryptos.size(); i++) {
			cout << "Name: " << pf.cryptos[i].ticker << " Active Price: " << pf.cryptos[i].activePrice << " Quantity: " << pf.cryptos[i].quantity << "\n";
		}
	}
}


void CLIPrinter::printPortfolioPositions(const RTPortfolio& pf, bool performances) {
	cout << "Complete portfolio print\n";
	cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n";
	printStockPositions(pf, performances);
	cout << "------------------------------------------------------------\n";
	printCashPositions(pf, performances);
	cout << "------------------------------------------------------------\n";
	printCryptoPositions(pf, performances);
}

void CLIPrinter::printCumulativeMetrics(double totalValue, double stocksVal, double cashesVal, double cryptoVal, double totalPerf) {
	cout << "Portfolio statistics\n";
	cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n";
	cout << "Total portfolio value (in USD):         " << totalValue << "\n";
	cout << "    - value of STOCKS:                  " << stocksVal << "\n";
	cout << "    - value of CASH:                    " << cashesVal << "\n";
	cout << "    - value of CRYPTO:                  " << cryptoVal << "\n";
	cout << "Total perfomance                        " << totalPerf << "\n";
}

void CLIPrinter::printPerformanceReport(const perfRatios& perf) {
	cout << "\n================ PERFORMANCE REPORT ================\n";
	printReturnMap("Stocks", perf.stockReturns);
	printReturnMap("Cash", perf.cashReturns);
	printReturnMap("Crypto", perf.cryptoReturns);
	cout << "====================================================\n\n";
}

void CLIPrinter::printBacktestResult(const perfRatios& perf, double totalPerf, const chrono::year_month_day& date) {
	cout << "\n================ BACKTEST RESULTS ================\n";
	cout << "Start Date: " << date << "\n";

	double totalPercentage = (totalPerf - 1.0) * 100.0;
	cout << "Total Portfolio Return: " << fixed << setprecision(2) << totalPercentage << "%\n\n";

	cout << "Individual Asset Returns:\n";
	printReturnMap("Stocks", perf.stockReturns);
	printReturnMap("Cash", perf.cashReturns);
	printReturnMap("Crypto", perf.cryptoReturns);
	cout << "==================================================\n\n";
}

void CLIPrinter::printYieldPrediction(const perfRatios& perf, double years) {
	cout << "\n============= YIELD PREDICTION (" << years << " Yrs) =============\n";
	cout << "Expected compound growth from fixed yields:\n";

	// For yield, the perfRatios contain the compound return directly, not a price ratio
	auto printYieldMap = [](const string& category, const std::unordered_map<string, double>& returns) {
		if (returns.empty()) return;
		cout << "  " << category << ":\n";
		for (const auto& [ticker, yield] : returns) {
			cout << "    - " << left << setw(8) << ticker << ": +"
				<< right << setw(8) << fixed << setprecision(2) << (yield * 100.0) << "%\n";
		}
		};

	printYieldMap("Stocks", perf.stockReturns);
	printYieldMap("Cash", perf.cashReturns);
	printYieldMap("Crypto", perf.cryptoReturns);
	cout << "====================================================\n\n";
}

void CLIPrinter::printIndicatorRSI(const string& ticker, double rsi) {
	cout << ">> RSI (" << ticker << "): " << fixed << setprecision(2) << rsi << " - ";
	if (rsi >= 70.0) {
		cout << "OVERBOUGHT (Consider Selling)\n";
	}
	else if (rsi <= 30.0) {
		cout << "OVERSOLD (Consider Buying)\n";
	}
	else {
		cout << "NEUTRAL\n";
	}
}

void CLIPrinter::printIndicatorBollinger(const string& ticker, int bollingerState) {
	cout << ">> Bollinger Bands (" << ticker << "): ";
	switch (bollingerState) {
	case 2:  cout << "STRONGLY OVERBOUGHT (Price > Upper Band + 2 StdDev)\n"; break;
	case 1:  cout << "SLIGHTLY OVERBOUGHT (Price > Upper Band + 1 StdDev)\n"; break;
	case -1: cout << "SLIGHTLY OVERSOLD (Price < Lower Band - 1 StdDev)\n"; break;
	case -2: cout << "STRONGLY OVERSOLD (Price < Lower Band - 2 StdDev)\n"; break;
	case 0:  cout << "FAIR VALUE (Price within normal standard deviation)\n"; break;
	default: cout << "DATA ERROR\n"; break;
	}
}

void CLIPrinter::printMonteCarloResult(const string& ticker, int duration, double targetPrice, double chance) {
	cout << "\n============== MONTE CARLO SIMULATION ==============\n";
	cout << "Asset: " << ticker << "\n";
	cout << "Target Price: $" << fixed << setprecision(2) << targetPrice << "\n";
	cout << "Timeframe: " << duration << " days\n";
	cout << "Probability of success: " << fixed << setprecision(2) << (chance * 100.0) << "%\n";
	cout << "====================================================\n\n";
}