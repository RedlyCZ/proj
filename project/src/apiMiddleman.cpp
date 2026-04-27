#include "apiMiddleman.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <ctime>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <thread>
#include <chrono>
#include <format>
#include <fstream>
#include <optional>
#include <cctype>


using json = nlohmann::json;
using namespace std;

//loads apikeys from config.json
struct ApiKeys {
    string finnhub;
    string twelveData;
    string apiNinja;
    string fred;

    ApiKeys() {
        std::ifstream configFile("config.json");
        if (configFile.is_open()) {
            try {
                json configJson = json::parse(configFile);
                finnhub = configJson.value("finnhubApiKey", "");
                twelveData = configJson.value("twelveDataApiKey", "");
                apiNinja = configJson.value("apiNinjaApiKey", "");
                fred = configJson.value("fredApiKey", "");
            }
            catch (const json::parse_error& e) {
                cerr << "Error parsing config.json: " << e.what() << "\n";
            }
        }
        else {
            cerr << "Warning: Could not open config.json. Ensure it's in the working directory.\n";
        }
    }
};


//to load keys only once
const ApiKeys& getApiKeys() {
    static ApiKeys keys;
    return keys;
}

//helper methods

std::string formatYMD(const std::chrono::year_month_day& ymd) {
    return std::format("{:%Y-%m-%d}", ymd);
}

long long ymdToMsEpoch(const std::chrono::year_month_day& ymd) {
    std::chrono::sys_days days = ymd;
    return std::chrono::duration_cast<std::chrono::milliseconds>(days.time_since_epoch()).count();
}


//FinnHub, free API 60 calls per minute, used for stock data

std::optional<double> FinnHubChannel::getActivePrice(const string& ticker) {
    string url = "https://finnhub.io/api/v1/quote";
    cpr::Response r = cpr::Get(
        cpr::Url{ url },
        cpr::Parameters{
            {"symbol", ticker},
            {"token", getApiKeys().finnhub}
        }
    );
    if (r.status_code == 200) { //success
        json data = json::parse(r.text);
        if (data.contains("c")) { //success
            return data["c"].get<double>();
        }
        else {
            cout << "Error 2 - getActivePrice\n";
        }
    }
    else {
        cout << "Error 1 - getActivePrice\n";
    }
    return std::nullopt;
}

std::optional<double> FinnHubChannel::getActiveDividend(const string& ticker) {
    string url = "https://finnhub.io/api/v1/stock/metric";

    cpr::Response r = cpr::Get(
        cpr::Url{ url },
        cpr::Parameters{
            {"symbol", ticker},
            {"metric", "all"},
            {"token", getApiKeys().finnhub}
        }
    );

    if (r.status_code == 200) {
        try {
            json data = json::parse(r.text);

            // Check if the metric object and dividendPerShareAnnual exist
            if (data.contains("metric") && data["metric"].contains("dividendPerShareAnnual")) {
                if (data["metric"]["dividendPerShareAnnual"].is_null()) {
                    return 0.0; // No dividend
                }
                return data["metric"]["dividendPerShareAnnual"].get<double>();
            }
            else {
                // If it's a valid response but no dividend metric is found, yield is 0
                return 0.0;
            }
        }
        catch (...) {
            cout << "Error - FinnHub metric parsing failed for " << ticker << "\n";
            return std::nullopt;
        }
    }
    else {
        cout << "Error 10 - FinnHub Metric HTTP Status: " << r.status_code << "\n";
        return std::nullopt;
    }
}

//TwelveData offers free calls to stock price history (which finnhub blocks only for premium)
std::optional<std::vector<double>> TwelveDataChannel::getHistoricalPrices(const string& ticker, int days) {
    std::vector<double> prices;
    string url = "https://api.twelvedata.com/time_series";

    cpr::Response r = cpr::Get(
        cpr::Url{ url },
        cpr::Parameters{
            {"symbol", ticker},
            {"interval", "1day"},
            {"outputsize", std::to_string(days)},
            {"apikey", getApiKeys().twelveData}
        }
    );

    if (r.status_code == 200) {
        json data = json::parse(r.text);

        if (data.contains("status") && data["status"] == "error") {
            cout << "TwelveData Error: " << data["message"] << "\n";
            return std::nullopt;
        }

        if (data.contains("values") && data["values"].is_array()) {
            for (auto&& dayData : data["values"]) {
                if (dayData.contains("close")) {
                    prices.push_back(stod(dayData["close"].get<string>()));
                }
            }
            std::reverse(prices.begin(), prices.end()); //orders wrong so we reverse
            return prices;
        }
        else {
            cout << "Error - TwelveData getHistoricalPrices: 'values' key missing.\n";
        }
    }
    else {
        cout << "Error - TwelveData getHistoricalPrices failed with status: " << r.status_code << "\n";
    }

    return std::nullopt;
}

std::optional<double> TwelveDataChannel::getHistoricalPriceByDate(const string& ticker, const chrono::year_month_day& date) {
    string url = "https://api.twelvedata.com/time_series";
    string dateStr = formatYMD(date);

    // outputsize=1 and end_date ensures we get the close price of that exact day 
    cpr::Response r = cpr::Get(
        cpr::Url{ url },
        cpr::Parameters{
            {"symbol", ticker},
            {"interval", "1day"},
            {"outputsize", "1"},
            {"end_date", dateStr + " 23:59:59"},
            {"apikey", getApiKeys().twelveData}
        }
    );

    if (r.status_code == 200) {
        json data = json::parse(r.text);
        if (data.contains("values") && data["values"].is_array() && !data["values"].empty()) {
            if (data["values"][0].contains("close")) {
                return stod(data["values"][0]["close"].get<string>());
            }
        }
        else if (data.contains("status") && data["status"] == "error") {
            cout << "TwelveData Error: " << data["message"] << "\n";
        }
        else {
            cout << "Error - TwelveData getHistoricalPriceByDate: No data found.\n";
        }
    }
    else {
        cout << "Error - TwelveData getHistoricalPriceByDate failed with status: " << r.status_code << "\n";
    }
    return std::nullopt;
}

//Frankfurter, completely free API, used for forex data

std::optional<double> FrankfurterChannel::conversionRate(const string& baseCurrency, const string& targetCurrency) {
    string url = "https://api.frankfurter.app/latest";
    cpr::Response r = cpr::Get(
        cpr::Url{ url },
        cpr::Parameters{
            {"from", baseCurrency},
            {"to", targetCurrency}
        }
    );
    if (r.status_code == 200) {
        json data = json::parse(r.text);
        if (data.contains("rates") && data["rates"].contains(targetCurrency)) {
            return data["rates"][targetCurrency].get<double>();
        }
        else {
            cout << "Error 4 - conversion rate.\n";
        }
    }
    else {
        cout << "Error 5 - conversion rate\n";
    }
    return std::nullopt;
}

std::optional<double> FrankfurterChannel::getHistoricalRateByDate(const string& baseCurrency, const chrono::year_month_day& date, const string& targetCurrency) {
    string dateStr = formatYMD(date);
    string url = "https://api.frankfurter.app/" + dateStr;

    cpr::Response r = cpr::Get(
        cpr::Url{ url },
        cpr::Parameters{
            {"from", baseCurrency},
            {"to", targetCurrency}
        }
    );

    if (r.status_code == 200) {
        json data = json::parse(r.text);
        if (data.contains("rates") && data["rates"].contains(targetCurrency)) {
            return data["rates"][targetCurrency].get<double>();
        }
        else {
            cout << "Error 6 - historical conversion rate key missing.\n";
        }
    }
    else {
        cout << "Error 7 - historical conversion rate failed with status: " << r.status_code << "\n";
    }
    return std::nullopt;
}



// Binance API, free, no auth required for basic prices, 1200 calls per minute
// Uses tickers (e.g., BTC, ETH) paired with USDT

std::optional<double> BinanceChannel::getActivePrice(const string& ticker) {
    // Append USDT to the ticker (e.g., BTC -> BTCUSDT)
    string symbol = ticker + "USDT";

    transform(symbol.begin(), symbol.end(), symbol.begin(), [](unsigned char c) { return std::toupper(c); });

    string url = "https://api.binance.com/api/v3/ticker/price";

    cpr::Response r = cpr::Get(
        cpr::Url{ url },
        cpr::Parameters{
            {"symbol", symbol}
        }
    );

    if (r.status_code == 200) {
        json data = json::parse(r.text);
        if (data.contains("price")) {
            string priceStr = data["price"];
            return stod(priceStr);
        }
        else {
            cout << "Error - Binance price key missing\n";
        }
    }
    else {
        cout << "Error - Binance getActivePrice failed with status: " << r.status_code << "\n";
    }
    return std::nullopt;
}

std::optional<std::vector<double>> BinanceChannel::getHistoricalPrices(const string& cryptoName, int days) {
    std::vector<double> prices;

    string symbol = cryptoName + "USDT";
    transform(symbol.begin(), symbol.end(), symbol.begin(), [](unsigned char c) { return std::toupper(c); });

    string url = "https://api.binance.com/api/v3/klines";

    cpr::Response r = cpr::Get(
        cpr::Url{ url },
        cpr::Parameters{
            {"symbol", symbol},
            {"interval", "1d"},
            {"limit", std::to_string(days)}
        }
    );

    if (r.status_code == 200) {
        json data = json::parse(r.text);
        if (data.is_array()) {
            for (auto&& kline : data) {
                if (kline.is_array() && kline.size() > 4) {
                    string closePriceStr = kline[4].get<string>();
                    prices.push_back(stod(closePriceStr));
                }
            }
            return prices;
        }
        else {
            cout << "Error - Binance getHistoricalPrices: Expected an array of arrays.\n";
        }
    }
    else {
        cout << "Error - Binance getHistoricalPrices failed with status: " << r.status_code << "\n";
    }

    return std::nullopt;
}

std::optional<double> BinanceChannel::getHistoricalPriceByDate(const string& cryptoName, const chrono::year_month_day& date) {
    string symbol = cryptoName + "USDT";
    transform(symbol.begin(), symbol.end(), symbol.begin(), [](unsigned char c) { return std::toupper(c); });

    string url = "https://api.binance.com/api/v3/klines";

    long long startTime = ymdToMsEpoch(date);
    long long endTime = startTime + 86400000; // +1 day in milliseconds

    cpr::Response r = cpr::Get(
        cpr::Url{ url },
        cpr::Parameters{
            {"symbol", symbol},
            {"interval", "1d"},
            {"startTime", std::to_string(startTime)},
            {"endTime", std::to_string(endTime)},
            {"limit", "1"}
        }
    );

    if (r.status_code == 200) {
        json data = json::parse(r.text);
        if (data.is_array() && !data.empty()) {
            if (data[0].is_array() && data[0].size() > 4) {
                return stod(data[0][4].get<string>());
            }
        }
        else {
            cout << "Error - Binance getHistoricalPriceByDate: Expected array data missing.\n";
        }
    }
    else {
        cout << "Error - Binance getHistoricalPriceByDate failed with status: " << r.status_code << "\n";
    }
    return std::nullopt;
}




//FRED API - St. Louis Fed. Free, high limits. Best for Interest Rates.

string FredChannel::resolveSeriesId(const string& ticker) {
    // Map tickers to FRED Series IDs
    static const std::unordered_map<string, string> bankMap = {
        {"USD", "FEDFUNDS"},       // Federal Funds Effective Rate
        {"EUR", "ECBDFR"},         // ECB Deposit Facility Rate
        {"GBP", "IRSTCI01GBM156N"},        // Bank of England Official Bank Rate
        {"JPY", "IRSTCI01JPM156N"},// Japan Call Rate (overnight)
        {"CAD", "IRSTCI01CAM156N"},// Canada Overnight
        {"CHF", "IRSTCI01CHM156N"},// Swiss SARON/Overnight
        {"AUD", "IRSTCI01AUM156N"}, // Australia Cash Rate
        { "CZK", "IRSTCI01CZM156N"} //czech repo rate
    };

    auto it = bankMap.find(ticker);
    if (it != bankMap.end()) {
        return it->second;
    }
    return "";
}


std::optional<double> FredChannel::getInterestRate(const string& ticker) {
    string seriesId = resolveSeriesId(ticker);

    if (seriesId.empty()) {
        cout << "Error 20 - FRED: Currency/Country not mapped for interest rate.\n";
        return std::nullopt;
    }

    string url = "https://api.stlouisfed.org/fred/series/observations";

    cpr::Response r = cpr::Get(
        cpr::Url{ url },
        cpr::Parameters{
            {"series_id", seriesId},
            {"api_key", getApiKeys().fred},
            {"file_type", "json"},
            {"sort_order", "desc"}, // Get latest first
            {"limit", "1"}          // Only need the latest
        }
    );

    if (r.status_code == 200) {
        try {
            json data = json::parse(r.text);
            if (data.contains("observations") && !data["observations"].empty()) {
                // FRED returns values as strings ("5.33"), so we must convert
                string valStr = data["observations"][0]["value"];

                // FRED returns "." if data is missing for that specific date
                if (valStr == ".") {
                    cout << "Error 21 - FRED: Data point is missing/incomplete.\n";
                    return std::nullopt;
                }

                return stod(valStr);
            }
        }
        catch (const std::exception& e) {
            cout << "Error 22 - FRED parsing error: " << e.what() << "\n";
        }
    }
    else {
        cout << "Error 23 - FRED request failed. Status: " << r.status_code << "\n";
    }
    return std::nullopt;
}