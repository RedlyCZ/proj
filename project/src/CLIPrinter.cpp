#include "CLIPrinter.hpp"
#include "runtimePortfolio.hpp"
#include <iostream>
#include <string>


using namespace std;

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