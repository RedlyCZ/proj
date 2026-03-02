#include <string>
#include <vector>
#include "runtimePortfolio.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <filesystem>
#include <chrono>


#ifndef SNAPSHOT
#define SNAPSHOT

//custom made for converting data into json before saving
struct PositionToSave {
	std::string ticker;
	double quantity;
	double yield;
	double avgBuyPrice;
	double thenPrice;
};

struct PortfolioToSave {
	std::vector<PositionToSave> stocks;
	std::vector<PositionToSave> cryptos;
	std::vector<PositionToSave> cashes;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PositionToSave, ticker, quantity, yield, avgBuyPrice, thenPrice)

class Snapshoter {
private:
	std::string storagePath;
	bool folderExists();
public:
	Snapshoter(const std::string& _storagePath) {
		storagePath = _storagePath;
	}
	bool writeSnapshot(RTPortfolio portfolio);		//false if failed
	RTPortfolio readSnapshot(std::chrono::year_month_day searchedDate);
	void deleteSnapshot(std::string snapshotName);
};



#endif