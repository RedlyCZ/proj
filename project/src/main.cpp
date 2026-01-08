#include <iostream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main() {
    std::cout << "--- Financial Analyzer Demo ---" << std::endl;

    // Test JSON knihovny
    json j;
    j["projekt"] = "Financial Analyzer";
    j["verze"] = 0.1;
    std::cout << "JSON funguje: " << j.dump() << std::endl;

    // Test CPR knihovny (stazeni webove stranky)
    std::cout << "Zkousim pripojeni k internetu..." << std::endl;
    cpr::Response r = cpr::Get(cpr::Url{ "https://www.google.com" });

    if (r.status_code == 200) {
        std::cout << "CPR funguje! Stazeno " << r.text.length() << " bytu." << std::endl;
    }
    else {
        std::cout << "CPR chyba: " << r.status_code << std::endl;
    }

    return 0;
}