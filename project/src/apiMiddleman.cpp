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


using json = nlohmann::json;
using namespace std;

//apikeys (im sure they are safe here)
const string finnhubApiKey = "d5h90phr01qqequ12ip0d5h90phr01qqequ12ipg";
const string twelveDataApiKey = "4818bbe7eae44f9f9953f16e09e15398";
const string apiNinjaApiKey = "edYYHgdoAENWYD8N1i9k2rWLmlN5QtXPHU4zO3dY";
const string fredApiKey = "bd4aa411be07e1d514736502aabfe3a6";

//helper methods

std::string formatYMD(const std::chrono::year_month_day& ymd) {
    return std::format("{:%Y-%m-%d}", ymd);
}

long long ymdToMsEpoch(const std::chrono::year_month_day& ymd) {
    std::chrono::sys_days days = ymd;
    return std::chrono::duration_cast<std::chrono::milliseconds>(days.time_since_epoch()).count();
}


//FinnHub, free API 60 calls per minute, used for stock data

double FinnHubChannel::getActivePrice(const string& ticker) {
    string url = "https://finnhub.io/api/v1/quote";
    cpr::Response r = cpr::Get(
        cpr::Url{ url },
        cpr::Parameters{
            {"symbol", ticker},
            {"token", finnhubApiKey}
        }
    );
    if (r.status_code == 200) { //success
        json data = json::parse(r.text);
        if (data.contains("c")) { //success
            return data["c"];
        }
        else {
            cout << "Error 2 - getActivePrice\n";
        }
    }
    else {
        cout << "Error 1 - getActivePrice\n";
    }
    return -1;
}

double FinnHubChannel::getActiveDividend(const string& ticker) {
    string url = "https://finnhub.io/api/v1/stock/metric";

    cpr::Response r = cpr::Get(
        cpr::Url{ url },
        cpr::Parameters{
            {"symbol", ticker},
            {"metric", "all"},
            {"token", finnhubApiKey}
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
            return -1.0;
        }
    }
    else {
        cout << "Error 10 - FinnHub Metric HTTP Status: " << r.status_code << "\n";
        return -1.0;
    }
}

//TwelveData offers free calls to stock price history (which finnhub blocks only for premium)
std::vector<double> TwelveDataChannel::getHistoricalPrices(const string& ticker, int days) {
    std::vector<double> prices;
    string url = "https://api.twelvedata.com/time_series";

    cpr::Response r = cpr::Get(
        cpr::Url{ url },
        cpr::Parameters{
            {"symbol", ticker},
            {"interval", "1day"},
            {"outputsize", std::to_string(days)},
            {"apikey", twelveDataApiKey}
        }
    );

    if (r.status_code == 200) {
        json data = json::parse(r.text);

        if (data.contains("status") && data["status"] == "error") {
            cout << "TwelveData Error: " << data["message"] << "\n";
            return prices;
        }

        if (data.contains("values") && data["values"].is_array()) {
            for (auto&& dayData : data["values"]) {
                if (dayData.contains("close")) {
                    prices.push_back(stod(dayData["close"].get<string>()));
                }
            }
            std::reverse(prices.begin(), prices.end()); //orders wrong so we reverse
        }
        else {
            cout << "Error - TwelveData getHistoricalPrices: 'values' key missing.\n";
        }
    }
    else {
        cout << "Error - TwelveData getHistoricalPrices failed with status: " << r.status_code << "\n";
    }

    return prices;
}

double TwelveDataChannel::getHistoricalPriceByDate(const string& ticker, const chrono::year_month_day& date) {
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
            {"apikey", twelveDataApiKey}
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
    return -1.0;
}

//Frankfurter, completely free API, used for forex data

double FrankfurterChannel::conversionRate(const string& baseCurrency, const string& targetCurrency) {
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
            return data["rates"][targetCurrency];
        }
        else {
            cout << "Error 4 - conversion rate.\n";
        }
    }
    else {
        cout << "Error 5 - conversion rate\n";
    }
    return -1;
}

double FrankfurterChannel::getHistoricalRateByDate(const string& baseCurrency, const chrono::year_month_day& date, const string& targetCurrency) {
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
            return data["rates"][targetCurrency];
        }
        else {
            cout << "Error 6 - historical conversion rate key missing.\n";
        }
    }
    else {
        cout << "Error 7 - historical conversion rate failed with status: " << r.status_code << "\n";
    }
    return -1.0;
}



// Binance API, free, no auth required for basic prices, 1200 calls per minute
// Uses tickers (e.g., BTC, ETH) paired with USDT

double BinanceChannel::getActivePrice(const string& ticker) {
    // Append USDT to the ticker (e.g., BTC -> BTCUSDT)
    string symbol = ticker + "USDT";

    transform(symbol.begin(), symbol.end(), symbol.begin(), ::toupper);

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
    return -1;
}

std::vector<double> BinanceChannel::getHistoricalPrices(const string& cryptoName, int days) {
    std::vector<double> prices;

    string symbol = cryptoName + "USDT";
    transform(symbol.begin(), symbol.end(), symbol.begin(), ::toupper);

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
        }
        else {
            cout << "Error - Binance getHistoricalPrices: Expected an array of arrays.\n";
        }
    }
    else {
        cout << "Error - Binance getHistoricalPrices failed with status: " << r.status_code << "\n";
    }

    return prices;
}

double BinanceChannel::getHistoricalPriceByDate(const string& cryptoName, const chrono::year_month_day& date) {
    string symbol = cryptoName + "USDT";
    transform(symbol.begin(), symbol.end(), symbol.begin(), ::toupper);

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
    return -1.0;
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


double FredChannel::getInterestRate(const string& ticker) {
    string seriesId = resolveSeriesId(ticker);

    if (seriesId.empty()) {
        cout << "Error 20 - FRED: Currency/Country not mapped for interest rate.\n";
        return -1.0;
    }

    string url = "https://api.stlouisfed.org/fred/series/observations";

    cpr::Response r = cpr::Get(
        cpr::Url{ url },
        cpr::Parameters{
            {"series_id", seriesId},
            {"api_key", fredApiKey},
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
                    return -1.0;
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
    return -1.0;
}