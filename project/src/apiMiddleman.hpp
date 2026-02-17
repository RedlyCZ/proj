#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <string>

#ifndef API_MIDDLEMAN
#define API_MIDDLEMAN

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

class TwelveDataChannel {
public:
	double getStockDividend(const std::string& ticker);
};

class ApiNinjasChannel {
private:
	std::string resolveCentralBankName(const std::string& ticker); //for conversion currencyticker -> centralbankname
public:
	double getInterestRate(const std::string& ticker); 
};

class CnbChannel {
public:
	double getCzechInterestRate();
};


#endif