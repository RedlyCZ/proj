#ifndef SNAPSHOT
#define SNAPSHOT

#include <string>
#include <vector>
#include "runtimePortfolio.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <optional>

//custom made for converting data into json before saving
struct PositionToSave {
	std::string ticker;
	double quantity = 0;
	double yield = 0;
	double avgBuyPrice = 0;
	double thenPrice = 0;
};

struct PortfolioToSave {
	std::vector<PositionToSave> stocks;
	std::vector<PositionToSave> cryptos;
	std::vector<PositionToSave> cashes;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PositionToSave, ticker, quantity, yield, avgBuyPrice, thenPrice)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PortfolioToSave, stocks, cryptos, cashes)


class Snapshoter {
private:
	std::string storagePath;
	bool folderExists();
	bool createStorage();			//false if failed
	std::string currentDate();
public:
	Snapshoter(const std::string& _storagePath) {
		storagePath = _storagePath;
	}
	bool writeSnapshot(const RTPortfolio& portfolio);							//false if failed
	std::optional<RTPortfolio> readSnapshot(const std::chrono::year_month_day& searchedDate);		//returns nullopt if snapshot not found
	bool deleteSnapshot(const std::chrono::year_month_day& deleteDate);
};



#endif