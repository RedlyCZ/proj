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
	void printPerformanceReport(const perfRatios& perf);
	void printBacktestResult(const perfRatios& perf, double totalPerf, const std::chrono::year_month_day& date);
	void printYieldPrediction(const perfRatios& perf, double years);
	void printIndicatorRSI(const std::string& ticker, double rsi);
	void printIndicatorBollinger(const std::string& ticker, int bollingerState);
	void printMonteCarloResult(const std::string& ticker, int duration, double targetPrice, double chance);
};

#endif