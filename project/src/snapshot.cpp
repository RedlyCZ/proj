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
    std::chrono::year_month_day current_date{ std::chrono::floor<std::chrono::days>(now) };
    return std::format("{}", current_date);
}

bool Snapshoter::writeSnapshot(const RTPortfolio& portfolio) {                  //false if failed
    if (!folderExists()) {
        if (!createStorage()) {             //we dont have folder and its creation failed
            return false;
        }
    }
    string snapshotPath = storagePath + "/" + currentDate();              //we use the date as unique name for the snapshot
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

RTPortfolio Snapshoter::readSnapshot(const chrono::year_month_day& searchedDate) {

}
