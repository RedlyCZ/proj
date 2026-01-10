#include <iostream>
#include <string>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   FINANCIAL ANALYZER - TECH DEMO v0.1  " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Platforma: " << (sizeof(void*) == 8 ? "64-bit" : "32-bit") << std::endl;

    std::string apiKey = "d5h90phr01qqequ12ip0d5h90phr01qqequ12ipg";
    std::string symbol = "SQM";
    std::string url = "https://finnhub.io/api/v1/quote";

    std::cout << "\n[NETWORK] Stahuji data z: " << url << " ..." << std::endl;

    cpr::Response r = cpr::Get(
        cpr::Url{ url },
        cpr::Parameters{
            {"symbol", symbol},
            {"token", apiKey}
        }
    );

    if (r.status_code == 200) {
        std::cout << "[OK] Data uspesne stazena (" << r.text.length() << " bytu)." << std::endl;

        try {
            // 2. Parsování JSON
            json data = json::parse(r.text);

            double price = data["c"];
            double change = data["d"];
            double percent = data["dp"];

            if (data.contains("c")) {
                double price = data["c"];
                double change = data["d"];
                double percent = data["dp"];

                std::cout << "Symbol: " << symbol << "\n";
                std::cout << "Price:  $" << price << "\n";
                std::cout << "Change: " << change << " (" << percent << "%)\n";
            }
            else {
                std::cout << "API Error or Invalid Symbol.\n";
            }

        }
        catch (const json::parse_error& e) {
            std::cerr << "[CHYBA] Nepodarilo se zpracovat JSON: " << e.what() << std::endl;
        }

    }
    else {
        std::cerr << "[CHYBA] Server vratil kod: " << r.status_code << std::endl;
        std::cerr << "Detail chyby: " << r.error.message << std::endl;
    }

    return 0;
}