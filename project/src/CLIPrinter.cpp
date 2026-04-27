#include "CLIPrinter.hpp"
#include "runtimePortfolio.hpp"
#include "financialCalc.hpp"
#include <iostream>
#include <string>
#include <iomanip>
#include <string>
#include <chrono>


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

	const auto& stocks = pf.getStocks();

	if (perf) {
		for (size_t i = 0; i < stocks.size(); i++) {
			double performance = stocks[i].activePrice / stocks[i].averageBuyPrice;
			cout << "Ticker: " << stocks[i].ticker
				<< " Active Price: " << fixed << setprecision(2) << stocks[i].activePrice
				<< " Performance: " << fixed << setprecision(2) << (performance * 100.0) << "%"
				<< " Value: " << fixed << setprecision(2) << (stocks[i].quantity * stocks[i].activePrice) << "\n";
		}
	}
	else {
		for (size_t i = 0; i < stocks.size(); i++) {
			cout << "Ticker: " << stocks[i].ticker
				<< " Active Price: " << fixed << setprecision(2) << stocks[i].activePrice
				<< " Active Yield: " << fixed << setprecision(4) << stocks[i].yield
				<< " Quantity: " << fixed << setprecision(4) << stocks[i].quantity << "\n";
		}
	}
}

void CLIPrinter::printCashPositions(const RTPortfolio& pf, bool perf) {
	cout << "Cashes:\n";
	
	const auto& cashes = pf.getCashes();

	if (perf) {
		for (size_t i = 0; i < cashes.size(); i++) {
			double performance = cashes[i].activePrice / cashes[i].averageBuyPrice;
			cout << "Ticker: " << cashes[i].ticker
				<< " Active Price: " << fixed << setprecision(2) << cashes[i].activePrice
				<< " Performance: " << fixed << setprecision(2) << (performance * 100.0) << "%"
				<< " Value: " << fixed << setprecision(2) << (cashes[i].quantity * cashes[i].activePrice) << "\n";
		}
	}
	else {
		for (size_t i = 0; i < cashes.size(); i++) {
			cout << "Ticker: " << cashes[i].ticker
				<< " Active Price: " << fixed << setprecision(2) << cashes[i].activePrice
				<< " Active Yield: " << fixed << setprecision(4) << cashes[i].yield
				<< " Quantity: " << fixed << setprecision(2) << cashes[i].quantity << "\n";
		}
	}
}

void CLIPrinter::printCryptoPositions(const RTPortfolio& pf, bool perf) {
	cout << "Crypto:\n";

	const auto& cryptos = pf.getCryptos();

	if (perf) {
		for (size_t i = 0; i < cryptos.size(); i++) {
			double performance = cryptos[i].activePrice / cryptos[i].averageBuyPrice;
			cout << "Name: " << cryptos[i].ticker
				<< " Active Price: " << fixed << setprecision(2) << cryptos[i].activePrice
				<< " Performance: " << fixed << setprecision(2) << (performance * 100.0) << "%"
				<< " Value: " << fixed << setprecision(2) << (cryptos[i].quantity * cryptos[i].activePrice) << "\n";
		}
	}
	else {
		for (size_t i = 0; i < cryptos.size(); i++) {
			cout << "Name: " << cryptos[i].ticker
				<< " Active Price: " << fixed << setprecision(2) << cryptos[i].activePrice
				<< " Quantity: " << fixed << setprecision(6) << cryptos[i].quantity << "\n";
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
	cout << "Total portfolio value (in USD):         " << fixed << setprecision(2) << totalValue << "\n";
	cout << "    - value of STOCKS:                  " << fixed << setprecision(2) << stocksVal << "\n";
	cout << "    - value of CASH:                    " << fixed << setprecision(2) << cashesVal << "\n";
	cout << "    - value of CRYPTO:                  " << fixed << setprecision(2) << cryptoVal << "\n";
	cout << "Total perfomance                        " << fixed << setprecision(2) << (totalPerf * 100.0) << " %\n";
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