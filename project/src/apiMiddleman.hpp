#ifndef API_MIDDLEMAN
#define API_MIDDLEMAN

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include <chrono>
#include <optional>

//direct specific API abstractions
//we use a lot of them cause we rely only on free tiers -> we collect pieces from many sources

class FinnHubChannel {
public:
	std::optional<double> getActivePrice(const std::string& ticker);
	std::optional<double> getActiveDividend(const std::string& ticker);
};

class TwelveDataChannel {
public:
	std::optional<std::vector<double>> getHistoricalPrices(const std::string& ticker, int days);
	std::optional<double> getHistoricalPriceByDate(const std::string& ticker, const std::chrono::year_month_day& date);
};

class FrankfurterChannel {
public:
	std::optional<double> conversionRate(const std::string& baseCurrency, const std::string& targetCurrency = "USD");
	std::optional<double> getHistoricalRateByDate(const std::string& baseCurrency, const std::chrono::year_month_day& date, const std::string& targetCurrency = "USD");
};

class BinanceChannel {
public:
	std::optional<double> getActivePrice(const std::string& cryptoName);
	std::optional<std::vector<double>> getHistoricalPrices(const std::string& cryptoName, int days);
	std::optional<double> getHistoricalPriceByDate(const std::string& cryptoName, const std::chrono::year_month_day& date);
};

class FredChannel {
private:
	std::string resolveSeriesId(const std::string& ticker);
public:
	std::optional<double> getInterestRate(const std::string& ticker);
};


//generalized abstractions of instrument types
//exception that it catches is usually the one you get when you run out of tokens, but it returning -1 (or empty vec) signalises a problem in general

class StockDataChannel {
public:
	std::optional<double> getActivePrice(const std::string& ticker) {
		FinnHubChannel apisrc;
		try {
			return apisrc.getActivePrice(ticker);
		}
		catch (...) {
			return std::nullopt;
		}
	}
	std::optional<double> getActiveDividend(const std::string& ticker) {
		FinnHubChannel apisrc;
		try {
			return apisrc.getActiveDividend(ticker);
		}
		catch (...) {
			return std::nullopt;
		}
	}
	std::optional<std::vector<double>> getHistoricalPrices(const std::string& ticker, int days) {
		TwelveDataChannel apisrc;
		try {
			return apisrc.getHistoricalPrices(ticker, days);
		}
		catch (...) {
			return std::nullopt;
		}
	}
	std::optional<double> getHistoricalPriceByDate(const std::string& ticker, const std::chrono::year_month_day& date) {
		TwelveDataChannel apisrc;
		try {
			return apisrc.getHistoricalPriceByDate(ticker, date);
		}
		catch (...) {
			return std::nullopt;
		}
	}
};

class CashDataChannel {
public:
	std::optional<double> getConversionRate(const std::string& ticker) {
		FrankfurterChannel apisrc;
		if (ticker == "USD") return 1.0;
		try {
			return apisrc.conversionRate(ticker);
		}
		catch (...) {
			return std::nullopt;
		}
	}
	std::optional<double> getInterestRate(const std::string& ticker) {
		FredChannel fredApiSrc;
		try {
			auto rate = fredApiSrc.getInterestRate(ticker);
			if (rate) {
				return *rate / 100.0; //division by 100 to go from percent to normal notation
			}
			return std::nullopt;
		}
		catch (...) {
			return std::nullopt;
		}
	}
	std::optional<double> getHistoricalPriceByDate(const std::string& ticker, const std::chrono::year_month_day& date) {
		FrankfurterChannel apisrc;
		if (ticker == "USD") return 1.0;
		try {
			return apisrc.getHistoricalRateByDate(ticker, date);
		}
		catch (...) {
			return std::nullopt;
		}
	}
};

class CryptoDataChannel {
public:
	std::optional<double> getActivePrice(const std::string& cryptoName) {
		BinanceChannel apisrc;
		try {
			return apisrc.getActivePrice(cryptoName);
		}
		catch (...) {
			return std::nullopt;
		}
	}
	std::optional<std::vector<double>> getHistoricalPrices(const std::string& cryptoName, int days) {
		BinanceChannel apisrc;
		try {
			return apisrc.getHistoricalPrices(cryptoName, days);
		}
		catch (...) {
			return std::nullopt;
		}
	}
	std::optional<double> getHistoricalPriceByDate(const std::string& cryptoName, const std::chrono::year_month_day& date) {
		BinanceChannel apisrc;
		try {
			return apisrc.getHistoricalPriceByDate(cryptoName, date);
		}
		catch (...) {
			return std::nullopt;
		}
	}
};


#endif