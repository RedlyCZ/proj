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

    // ZMENA: Pouzijeme bezpecne API (falesna data o produktech), 
    // ktere Eduroam neblokuje.
    std::string url = "https://dummyjson.com/products/1";

    std::cout << "\n[NETWORK] Stahuji data z: " << url << " ..." << std::endl;

    // 1. Stažení dat
    cpr::Response r = cpr::Get(cpr::Url{ url });

    if (r.status_code == 200) {
        std::cout << "[OK] Data uspesne stazena (" << r.text.length() << " bytu)." << std::endl;

        try {
            // 2. Parsování JSON
            json data = json::parse(r.text);

            // Vytahování dat z jineho formatu (DummyJSON)
            std::string nazev = data["title"];
            std::string popis = data["description"];
            double cena = data["price"];

            std::cout << "\n----------------------------------------" << std::endl;
            std::cout << " PRIJATA DATA (Test API)" << std::endl;
            std::cout << "----------------------------------------" << std::endl;
            std::cout << " Produkt: " << nazev << std::endl;
            std::cout << " Cena:    $" << cena << std::endl;
            std::cout << " Popis:   " << popis << std::endl;
            std::cout << "----------------------------------------" << std::endl;

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