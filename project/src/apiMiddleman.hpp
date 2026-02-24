#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <string>

#ifndef API_MIDDLEMAN
#define API_MIDDLEMAN


//direct specific API abstractions

class FinnHubChannel {
public:
	double getActivePrice(const std::string& ticker);
};

class FrankfurterChannel {
public:
	double conversionRate(const std::string& baseCurrency, const std::string& targetCurrency = "USD");
};

class CoinGeckoChannel {
public:
	double getActivePrice(const std::string& cryptoName);
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

class CnbChannel {
public:
	double getCzechInterestRate();
};


//generalized abstractions of instrument types

class StockDataChannel {
public:
	double getActivePrice(const std::string& ticker) {
		FinnHubChannel apisrc;
		return apisrc.getActivePrice(ticker);
	}
	double getActiveDividend(const std::string& ticker) {
		FinancialModelingPrepChannel apisrc;
		return apisrc.getStockDividend(ticker);
	}
};

class CashDataChannel {
public:
	double getConversionRate(const std::string& ticker) {
		FrankfurterChannel apisrc;
		return apisrc.conversionRate(ticker);
	}
	double getInterestRate(const std::string& ticker) {
		FredChannel fredApiSrc;
		return fredApiSrc.getInterestRate(ticker);
	}
};

class CryptoDataChannel {
public:
	double getActivePrice(const std::string& cryptoName) {
		CoinGeckoChannel apisrc;
		return apisrc.getActivePrice(cryptoName);
	}
};


#endif