#ifndef CLIPrinterLib
#define CLIPrinterLib



class RTPortfolio;

class perfRatios;

class CLIPrinter {
public:
	void printStockPositions(const RTPortfolio& pf, bool perf);
	void printCashPositions(const RTPortfolio& pf, bool perf);
	void printCryptoPositions(const RTPortfolio& pf, bool perf);
	void printPortfolioPositions(const RTPortfolio& pf, bool performances);
	void printCumulativeMetrics(double totalValue, double stocksVal, double cashesVal, double cryptoVal, double totalPerf);
};

#endif