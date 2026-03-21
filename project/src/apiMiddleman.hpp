#ifndef API_MIDDLEMAN
#define API_MIDDLEMAN

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <string>

//direct specific API abstractions
//we use a lot of them cause we rely only on free tiers -> we collect pieces from many sources

class FinnHubChannel {
public:
	double getActivePrice(const std::string& ticker);
};

class TwelveDataChannel {
public:
	std::vector<double> getHistoricalPrices(const std::string& ticker, int days);
};

class FrankfurterChannel {
public:
	double conversionRate(const std::string& baseCurrency, const std::string& targetCurrency = "USD");
};

class BinanceChannel{
public:
	double getActivePrice(const std::string& cryptoName);
	std::vector<double> getHistoricalPrices(const std::string& cryptoName, int days);
};

class FinancialModelingPrepChannel {
public:
	double getStockDividend(const std::string& ticker);
};

class FredChannel {
private:
	std::string resolveSeriesId(const std::string& ticker);
public:
	double getInterestRate(const std::string& ticker);
};


//generalized abstractions of instrument types
//exception that it catches is usually the one you get when you run out of tokens, but it returning -1 (or empty vec) signalises a problem in general

class StockDataChannel {
public:
	double getActivePrice(const std::string& ticker) {
		FinnHubChannel apisrc;
		try {
			return apisrc.getActivePrice(ticker);
		}
		catch(...){
			return -1;
		}
		
	}
	double getActiveDividend(const std::string& ticker) {
		FinancialModelingPrepChannel apisrc;
		try {
			return apisrc.getStockDividend(ticker);
		}
		catch (...) {
			return -1;
		}
	}
	std::vector<double> getHistoricalPrices(const std::string& ticker, int days) {
		TwelveDataChannel apisrc;
		try {
			return apisrc.getHistoricalPrices(ticker, days);
		}
		catch (...) {
			return std::vector<double>();
		}
	}
};

class CashDataChannel {
public:
	double getConversionRate(const std::string& ticker) {
		FrankfurterChannel apisrc;
		try {
			return apisrc.conversionRate(ticker);
		}
		catch (...) {
			return -1;
		}
	}
	double getInterestRate(const std::string& ticker) {
		FredChannel fredApiSrc;
		try {
			return fredApiSrc.getInterestRate(ticker) / 100;	//division by 100 to go from percent to normal normal notation
		}
		catch (...) {
			return -1;
		}
	}
};

class CryptoDataChannel {
public:
	double getActivePrice(const std::string& cryptoName) {
		BinanceChannel apisrc;
		try {
			return apisrc.getActivePrice(cryptoName);
		}
		catch (...) {
			return -1;
		}
	}
	std::vector<double> getHistoricalPrices(const std::string& cryptoName, int days) {
		BinanceChannel apisrc;
		try {
			return apisrc.getHistoricalPrices(cryptoName, days);
		}
		catch (...) {
			return std::vector<double>();
		}
	}
};


#endif