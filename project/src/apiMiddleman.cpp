#include "apiMiddleman.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

//apikeys (im sure they are safe here)
constexpr string finnhubApiKey = "d5h90phr01qqequ12ip0d5h90phr01qqequ12ipg";
constexpr string twelveDataApiKey = "4818bbe7eae44f9f9953f16e09e15398";
constexpr string apiNinjaApiKey = "edYYHgdoAENWYD8N1i9k2rWLmlN5QtXPHU4zO3dY";

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



//CoinGecko, free API 30 calls per minute, used for crypto data
//kinda hate them, cause they dont use tickers, but fullname

double CoinGeckoChannel::getActivePrice(const string& cryptoName) {
    string url = "https://api.coingecko.com/api/v3/simple/price";
    cpr::Response r = cpr::Get(
        cpr::Url{ url },
        cpr::Parameters{
            {"ids", cryptoName},
            {"vs_currencies", "usd"}
        }
    );
    if (r.status_code == 200) {
        json data = json::parse(r.text);
        if (data.contains(cryptoName) && data[cryptoName].contains("usd")) {
            return data[cryptoName]["usd"];
        }
        else {
            cout << "Error 6 - CoinGecko getActivePrice\n";
        }
    }
    else {
        cout << "Error 7 - CoinGecko getActivePrice\n";
    }
    return -1;
}


//TwelveData, free API 800 calls per minute, used for some advanced stock data
double TwelveDataChannel::getStockDividend(const string& ticker) {
    string url = "https://api.twelvedata.com/dividends";
    cpr::Response r = cpr::Get(
        cpr::Url{ url },
        cpr::Parameters{
            {"symbol", ticker},
            {"apikey", twelveDataApiKey}
        }
    );
    if (r.status_code == 200) {
        json data = json::parse(r.text);
        if (data.contains("dividends") && !data["dividends"].empty()) {
            if (data["dividends"][0].contains("amount")) {
                return data["dividends"][0]["amount"];
            }
        }
        else {
            return 0.0; //no dividends found for this stock
        }
    }
    else {
        cout << "Error 10 - loading dividend problem twelvedatachannel\n";
    }
    return -1.0;
}



//ApiNinja - free api tier with 3000 queries per month. Used for current central bank interest rates.

double ApiNinjasChannel::getInterestRate(const string& ticker) {
    string properName = resolveCentralBankName(ticker); //becasue the api works with full central bank name

    string url = "https://api.api-ninjas.com/v1/interestrate";

    cpr::Response r = cpr::Get(
        cpr::Url{ url },
        cpr::Parameters{
            {"name", properName}
        },
        cpr::Header{
            {"X-Api-Key", apiNinjaApiKey}
        }
    );

    if (r.status_code == 200) {
        json data = json::parse(r.text);
        if (data.is_array() && !data.empty()) {
            if (data[0].contains("rate_pct")) {
                return data[0]["rate_pct"];
            }
        }
        else if (data.contains("rate_pct")) {
            return data["rate_pct"];
        }
        else {
            cout << "Error 11 - Interest rate not found in response - apininja\n";
        }
    }
    else {
        cout << "Error 13 - ApiNinjas request failed\n";
    }
    return -1.0;
}

string ApiNinjasChannel::resolveCentralBankName(const string& ticker) {
    static const std::unordered_map<string, string> bankMap = {
        {"USD", "United States Federal Reserve"},
        {"EUR", "European Central Bank"},
        {"GBP", "Bank of England"},
        {"JPY", "Bank of Japan"},
        {"CAD", "Bank of Canada"},
        {"AUD", "Reserve Bank of Australia"},
        {"CHF", "Swiss National Bank"},
        {"CNY", "People's Bank of China"},
        {"NZD", "Reserve Bank of New Zealand"},
        {"INR", "Reserve Bank of India"},
        {"BRL", "Central Bank of Brazil"},
        {"RUB", "Central Bank of the Russian Federation"},
        {"ZAR", "South African Reserve Bank"}
    };

    auto it = bankMap.find(ticker);
    if (it != bankMap.end()) {
        return it->second;
    }

    //if there is some unknown thing, return the currency ticker
    return ticker;
}


//Direct connection to cnb API, cause our national bank is not so important to be mapped by bigger APIs

double CnbChannel::getCzechInterestRate() {
    string url = "https://api.cnb.cz/cnbapi/pribor";

    cpr::Response r = cpr::Get(
        cpr::Url{ url },
        cpr::Parameters{
            {"lang", "EN"}
        }
    );

    if (r.status_code == 200) {
        json data = json::parse(r.text);

        if (data.contains("pribor") && !data["pribor"].empty()) {
            json& dailyData = data["pribor"][0];

            if (dailyData.contains("cells") && dailyData.contains("values")) {
                json& cells = dailyData["cells"];
                json& values = dailyData["values"];

                if (cells.size() == values.size()) {
                    for (size_t i = 0; i < cells.size(); ++i) {
                        string cellName = cells[i];
                        if (cellName == "2W" || cellName == "2T") {
                            return values[i];
                        }
                    }
                    if (!values.empty()) return values[0];
                }
            }
        }
        else if (data.is_array() && !data.empty()) {
        }
        else {
            cout << "Error 14 - CNB Data empty or invalid format\n";
        }
    }
    else {
        cout << "Error 16 - CNB request failed\n";
    }

    return -1.0;
}