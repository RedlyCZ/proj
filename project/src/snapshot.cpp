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
            cout << "No snapshot folder set. Use setpath <path> to properly set it!\n";       //implementing this without print here would take a lot of work (another std::expected, enum class, etc)
            return false;
        }
    }
    std::filesystem::path snapshotPath = std::filesystem::path(storagePath) / currentDate();        //we use the date as unique name for the snapshot 
    ofstream out_file(snapshotPath);
    if (!out_file) {
        std::cout << "Error: Could not open the file for writing.\n";
        return false;
    }

    json j;
    j["stocks"] = portfolio.stocks;
    j["cryptos"] = portfolio.cryptos;
    j["cashes"] = portfolio.cashes;

    j["totalDeposited"] = portfolio.totalDeposited;
    j["totalWithdrawn"] = portfolio.totalWithdrawn;

    out_file << j.dump(4);

    return true;
}

expected<RTPortfolio, SnapshotError> Snapshoter::readSnapshot(const chrono::year_month_day& searchedDate) {
    string stringDate = format("{}", searchedDate); //bit ineffective here, but one string doesnt kill anyone
    std::filesystem::path snapshotPath = std::filesystem::path(storagePath) / stringDate;
    if(!folderExists()) {
        return std::unexpected(SnapshotError::FOLDER_NOT_FOUND);
    }
    if (!fs::exists(snapshotPath)) {
        return std::unexpected(SnapshotError::FILE_NOT_FOUND);
    }
    ifstream readFile(snapshotPath);
    try {
        json snapshotJSON = json::parse(readFile);
        RTPortfolio pfReturn;

        pfReturn.stocks = snapshotJSON.at("stocks").get<std::vector<instrumentPosition>>();
        pfReturn.cryptos = snapshotJSON.at("cryptos").get<std::vector<instrumentPosition>>();
        pfReturn.cashes = snapshotJSON.at("cashes").get<std::vector<instrumentPosition>>();
        
        pfReturn.totalDeposited = snapshotJSON.at("totalDeposited").get<double>();
        pfReturn.totalWithdrawn = snapshotJSON.at("totalWithdrawn").get<double>();

        return pfReturn;
    }
    catch (...) {
        return std::unexpected(SnapshotError::JSON_CORRUPTED);
    }
}

bool Snapshoter::deleteSnapshot(const std::chrono::year_month_day& deleteDate) {
    string stringDate = format("{}", deleteDate); //bit ineffective here, but one string doesnt kill anyone
    std::filesystem::path snapshotPath = std::filesystem::path(storagePath) / stringDate;
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
