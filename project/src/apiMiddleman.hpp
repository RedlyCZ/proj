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


#endif