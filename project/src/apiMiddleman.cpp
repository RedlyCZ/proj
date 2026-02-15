#include "apiMiddleman.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

constexpr string finnhubApiKey = "d5h90phr01qqequ12ip0d5h90phr01qqequ12ipg";

double finnHubChannel::getActivePrice(string ticker) {
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
            cout << "Error 2.\n";
        }
    }
    else {
        cout << "Error 1\n";
    }
    return -1;
}