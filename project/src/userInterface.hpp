#ifndef USER_INTERFACE
#define USER_INTERFACE

#include "runtimePortfolio.hpp"
#include "financialCalc.hpp"
#include "CLIPrinter.hpp"
#include <string>
#include <chrono>
#include <sstream>

class CLIManager {
private:
	RTPortfolio portfolio;
	bool failed = false;
	void showAggregates() const;
	void showPositions() const;
	void showPerformance() const;
	void backtestPortfolio(const std::chrono::year_month_day& timeSince) const;
	void buy(instrumentType type, const std::string& newTicker, double newQuantity);
	void sell(instrumentType type, const std::string& newTicker, double newQuantity, bool closePosition);
	void depositUSD(double quantity);
	void cashoutUSD(double quantity);
	void fixYieldPrediction(double years) const;
	void calcRSI(instrumentType type, const std::string& ticker, int period) const;
	void calcBollinger(instrumentType type, const std::string& ticker) const;
	void monteCarloChance(instrumentType type, const std::string& ticker, int duration, double price, bool hit) const;
	void loadPF(const std::chrono::year_month_day& date);
	void savePF();
	void executeCommand(const std::string& cmd, std::istringstream& iss);
	void printHelp() const;
public:
	void interpretInputRow(const std::string& inputRow);
	bool is_failed() const noexcept {
		return failed;
	}
};


#endif