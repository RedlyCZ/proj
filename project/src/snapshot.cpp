#include <string>
#include <vector>
#include "runtimePortfolio.hpp"
#include <nlohmann/json.hpp>
#include "snapshot.hpp"
#include <iostream>
#include <filesystem>
#include <chrono>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;
using json = nlohmann::json;

bool Snapshoter::folderExists() {
    if (fs::exists(storagePath) && fs::is_directory(storagePath)) {
        return true;
    }
    else {
        return false;
    }
}

bool Snapshoter::createStorage() {          //false if failed
    fs::path relativePath = storagePath;
    try {
        fs::create_directories(relativePath);
    }
    catch(...){                     //creating folder failed
        return false;
    }
    return true;
}

string Snapshoter::currentDate() {
    auto now = chrono::system_clock::now();
    chrono::year_month_day current_date{ std::chrono::floor<std::chrono::days>(now) };
    return format("{}", current_date);
}

bool Snapshoter::writeSnapshot(const RTPortfolio& portfolio) {                  //false if failed
    if (!folderExists()) {
        if (!createStorage()) {             //we dont have folder and its creation failed
            return false;
        }
    }
    std::filesystem::path snapshotPath = std::filesystem::path(storagePath) / currentDate();        //we use the date as unique name for the snapshot 
    ofstream out_file(snapshotPath);
    if (!out_file) {
        std::cout << "Error: Could not open the file for writing.\n";
        return false;
    }
    PortfolioToSave pfWrite;
    for (auto&& pos : portfolio.stocks) {
        pfWrite.stocks.push_back({ pos.ticker, pos.quantity, pos.yield, pos.averageBuyPrice, pos.activePrice });
    }
    for (auto&& pos : portfolio.cryptos) {
        pfWrite.cryptos.push_back({ pos.ticker, pos.quantity, pos.yield, pos.averageBuyPrice, pos.activePrice });
    }
    for (auto&& pos : portfolio.cashes) {
        pfWrite.cashes.push_back({ pos.ticker, pos.quantity, pos.yield, pos.averageBuyPrice, pos.activePrice });
    }

    json j = pfWrite;

    out_file << j.dump(4);

    return true;
}

optional<RTPortfolio> Snapshoter::readSnapshot(const chrono::year_month_day& searchedDate) {
    string stringDate = format("{}", searchedDate); //bit ineffective here, but one string doesnt kill anyone
    std::filesystem::path snapshotPath = std::filesystem::path(storagePath) / stringDate;
    if(!folderExists() || !fs::exists(snapshotPath)) {
        return nullopt;
    }
    ifstream readFile(snapshotPath);
    json snapshotJSON;
    try {
        snapshotJSON = json::parse(readFile);
        PortfolioToSave pfRead = snapshotJSON.get<PortfolioToSave>();
        RTPortfolio pfReturn;
        for (auto&& savedPos : pfRead.stocks) {
            pfReturn.stocks.push_back({ instrumentType::STOCK, savedPos.ticker, savedPos.quantity, savedPos.thenPrice, savedPos.yield, savedPos.avgBuyPrice });
        }

        for (auto&& savedPos : pfRead.cryptos) {
            pfReturn.cryptos.push_back({ instrumentType::CRYPTO, savedPos.ticker, savedPos.quantity, savedPos.thenPrice, savedPos.yield, savedPos.avgBuyPrice });
        }

        for (auto&& savedPos : pfRead.cashes) {
            pfReturn.cashes.push_back({ instrumentType::CASH, savedPos.ticker, savedPos.quantity, savedPos.thenPrice, savedPos.yield, savedPos.avgBuyPrice });
        }
        return pfReturn;
    }
    catch (...) {
        return nullopt;
    }
}

bool Snapshoter::deleteSnapshot(const std::chrono::year_month_day& deleteDate) {
    string stringDate = format("{}", deleteDate); //bit ineffective here, but one string doesnt kill anyone
    string snapshotPath = storagePath + "/" + stringDate;
    if (!folderExists() || !fs::exists(snapshotPath)) {
        return true;                                        //although deleting the file "failed", there is no snapshot now, so we return it as success
    }
    try {
        fs::remove(snapshotPath);
        return true;
    }
    catch (...) { //deleting failed
        return false;
    }
}
