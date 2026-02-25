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


using json = nlohmann::json;
using namespace std;

//apikeys (im sure they are safe here)
const string finnhubApiKey = "d5h90phr01qqequ12ip0d5h90phr01qqequ12ipg";
const string twelveDataApiKey = "4818bbe7eae44f9f9953f16e09e15398";
const string apiNinjaApiKey = "edYYHgdoAENWYD8N1i9k2rWLmlN5QtXPHU4zO3dY";
const string fredApiKey = "bd4aa411be07e1d514736502aabfe3a6";
const string fmpApiKey = "Dgolm4HnDmWEvMhJQ19rCsUhzL4KmR6R";


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


//Financial Modeling Prep, free API 250 calls per day, used for some advanced stock data (like divi)
double FinancialModelingPrepChannel::getStockDividend(const string& ticker) {
    string url = "https://financialmodelingprep.com/stable/dividends";

    cpr::Response r = cpr::Get(
        cpr::Url{ url },
        cpr::Parameters{
            {"symbol", ticker},
            {"apikey", fmpApiKey}
        }
    );

    if (r.status_code == 200) {
        json data = json::parse(r.text);

        if (data.is_array() && !data.empty()) {

            double amount = data[0].value("dividend", 0.0);

            string frequency = data[0].value("frequency", "Annual");

            int payoutsPerYear = 1;

            transform(frequency.begin(), frequency.end(), frequency.begin(), [](unsigned char c) { return tolower(c); });

            if (frequency == "quarterly" || frequency == "quarter") {
                payoutsPerYear = 4;
            }
            else if (frequency == "monthly" || frequency == "month") {
                payoutsPerYear = 12;
            }
            else if (frequency == "semi-annual" || frequency == "semiannual" || frequency == "semi-annually") {
                payoutsPerYear = 2;
            }

            return amount * payoutsPerYear;
        }
        else {
            return 0.0;
        }
    }
    else {
        cout << "Error 10 - Loading dividend problem FinancialModelingPrepChannel\n";
    }
    return -1.0;
}

//FRED API - St. Louis Fed. Free, high limits. Best for Interest Rates.

string FredChannel::resolveSeriesId(const string& ticker) {
    // Map tickers to FRED Series IDs
    static const std::unordered_map<string, string> bankMap = {
        {"USD", "FEDFUNDS"},       // Federal Funds Effective Rate
        {"EUR", "ECBDFR"},         // ECB Deposit Facility Rate
        {"GBP", "BOERUKM"},        // Bank of England Official Bank Rate
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





//Direct connection to cnb API, cause our national bank is not so important to be mapped by bigger APIs

double CnbChannel::getCzechInterestRate() {
    string url = "https://api.cnb.cz/cnbapi/pribor/daily";

    time_t t = time(nullptr);
    tm now = *localtime(&t);

    // Go back 3 days in seconds (3 * 24 * 60 * 60 = 259200)
    // This ensures we don't hit the 48h delay wall or weekends
    time_t past_t = t - 259200;
    tm past = *localtime(&past_t);

    std::ostringstream dateStream;
    dateStream << std::put_time(&past, "%Y-%m-%d");
    std::string validDate = dateStream.str();

    cpr::Response r = cpr::Get(
        cpr::Url{ url },
        cpr::Parameters{
            {"lang", "EN"},
            {"date", validDate}
        }
    );

    if (r.status_code == 200) {
        json data = json::parse(r.text);

        if (data.contains("pribor") && !data["pribor"].empty()) {
            json& latestData = data["pribor"].back();

            if (latestData.contains("values") && !latestData["values"].empty()) {
                return latestData["values"][0];
            }
        }
    }
    else {
        cout << "Error 16 - CNB request failed\n";
    }

    return -1.0;
}