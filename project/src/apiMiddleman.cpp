#include "apiMiddleman.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;


//FinnHub, free API 60 calls per minute, used for stock data

constexpr string finnhubApiKey = "d5h90phr01qqequ12ip0d5h90phr01qqequ12ipg";

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