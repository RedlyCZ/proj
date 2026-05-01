#ifndef SNAPSHOT_HPP
#define SNAPSHOT_HPP

#include <string>
#include <vector>
#include "runtimePortfolio.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <optional>
#include <expected>

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(instrumentPosition, positionType, ticker, quantity, activePrice, yield, averageBuyPrice)

enum class SnapshotError {
	FILE_NOT_FOUND,
	FOLDER_NOT_FOUND,
	JSON_CORRUPTED,
	NONE_ERROR
};

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
	std::expected<RTPortfolio, SnapshotError> readSnapshot(const std::chrono::year_month_day& searchedDate);		//returns nullopt if snapshot not found
	bool deleteSnapshot(const std::chrono::year_month_day& deleteDate);
};



#endif