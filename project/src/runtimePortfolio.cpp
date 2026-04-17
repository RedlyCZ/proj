#include "runtimePortfolio.hpp"
#include "apiMiddleman.hpp"
#include "snapshot.hpp"
#include <vector>
#include <string>
#include <array>
#include <iostream>
#include <nlohmann/json.hpp>
#include <optional>

using json = nlohmann::json;
using namespace std;


int RTPortfolio::findTickerIndex(const string& newTicker, const vector<instrumentPosition>& container) const noexcept {
	int index = 0;
	for (auto&& pos : container) {
		if (pos.ticker == newTicker) {
			return index;
		}
		index++;
	}
	return -1;
}

bool RTPortfolio::depositCash(double quantity) {
	if (quantity <= 0) {
		cout << "Error, cannot deposit zero or negative amounts\n";
		return false;
	}

	totalDeposited += quantity;

	int posIndex = findTickerIndex("USD", cashes);

	if (posIndex == -1) { //no dollar position yet
		instrumentPosition newPos;
		newPos.positionType = instrumentType::CASH;
		newPos.ticker = "USD";
		newPos.quantity = quantity;
		newPos.averageBuyPrice = 1.0; //one dollar is always one dollar
		newPos.activePrice = 1.0;
		cashes.push_back(newPos);
	}
	else {
		cashes[posIndex].quantity += quantity;
	}
	return true; //passed
}

bool RTPortfolio::withdrawCash(double quantity) {
	if (quantity <= 0) {
		cout << "Error, cannot withdraw zero or negative amounts\n";
		return false;
	}

	int posIndex = findTickerIndex("USD", cashes);

	if (posIndex == -1) {
		cout << "Error, no USD balance to withdraw from\n";
		return false;
	}

	instrumentPosition& editPos = cashes[posIndex];

	if (editPos.quantity < quantity) {
		cout << "Error, insufficient USD funds for withdrawal\n";
		return false;
	}

	editPos.quantity -= quantity;
	totalWithdrawn += quantity;

	return true;
}

vector<instrumentPosition>* RTPortfolio::getContainer(instrumentType type) noexcept{
	switch (type) {
	case instrumentType::STOCK:
		return &stocks;
	case instrumentType::CASH:
		return &cashes;
	case instrumentType::CRYPTO:
		return &cryptos;
	}
	return nullptr;
}

std::optional<double> RTPortfolio::getActivePrice(instrumentType type, const string& ticker) const {
	switch (type) {
	case instrumentType::STOCK: {
		StockDataChannel stockApi;
		return stockApi.getActivePrice(ticker);
	}
	case instrumentType::CASH: {
		CashDataChannel cashApi;
		return cashApi.getConversionRate(ticker);
	}
	case instrumentType::CRYPTO: {
		CryptoDataChannel cryptoApi;
		return cryptoApi.getActivePrice(ticker);
	}
	}
	return std::nullopt;
}


double RTPortfolio::buyInstrument(instrumentType type, const string& newTicker, double newQuantity) {
	auto activePriceOpt = getActivePrice(type, newTicker);
	vector<instrumentPosition>* selectedContainer = getContainer(type);

	if (!activePriceOpt) {
		cout << "Error 3, failed to load price, buying\n";
		return 0;
	}

	double activePrice = *activePriceOpt;

	if (selectedContainer == nullptr) {
		cout << "Error, invalid instrument type\n";
		return 0;
	}

	double totalCost = activePrice * newQuantity;
	int usdIndex = findTickerIndex("USD", cashes);

	if (usdIndex == -1 || cashes[usdIndex].quantity < totalCost) {
		cout << "Error, insufficient USD funds for purchase\n";
		return 0;
	}

	cashes[usdIndex].quantity -= totalCost;

	int posIndex = findTickerIndex(newTicker, *selectedContainer);

	if (posIndex == -1) {
		instrumentPosition newPos;
		newPos.positionType = type;
		newPos.ticker = newTicker;
		newPos.quantity = newQuantity;
		newPos.averageBuyPrice = activePrice;
		newPos.activePrice = activePrice;
		selectedContainer->push_back(newPos);
	}
	else {
		instrumentPosition& editPos = (*selectedContainer)[posIndex];
		double totalQuantity = editPos.quantity + newQuantity;
		double totalInvested = (editPos.quantity * editPos.averageBuyPrice) + totalCost;
		editPos.averageBuyPrice = totalInvested / totalQuantity;
		editPos.quantity = totalQuantity;
		editPos.activePrice = activePrice;
	}

	return totalCost;
}

double RTPortfolio::sellInstrument(instrumentType type, const string& newTicker, double newQuantity, bool closePosition) {
	auto activePriceOpt = getActivePrice(type, newTicker);
	vector<instrumentPosition>* selectedContainer = getContainer(type);

	if (!activePriceOpt) {
		cout << "Error 3, failed to load price, selling\n";
		return 0;
	}

	double activePrice = *activePriceOpt;

	if (selectedContainer == nullptr) {
		cout << "Error, invalid instrument type\n";
		return 0;
	}

	int posIndex = findTickerIndex(newTicker, *selectedContainer);
	double totalGained = 0;

	if (posIndex == -1) {
		cout << "Error 8, cant sell unbuyed\n";
		return 0;
	}
	else {
		instrumentPosition& editPos = (*selectedContainer)[posIndex];
		if (closePosition) {
			double savedQuantity = editPos.quantity;
			(*selectedContainer).erase((*selectedContainer).begin() + posIndex);
			totalGained = activePrice * savedQuantity;
		}
		else if (editPos.quantity < newQuantity) {
			cout << "Error 9, cant sell more than you have\n";
			return 0;
		}
		else {
			editPos.quantity = editPos.quantity - newQuantity;
			totalGained = activePrice * newQuantity;
		}
	}

	int usdIndex = findTickerIndex("USD", cashes);
	if (usdIndex == -1) {	//we should always have dollar position when here, but whatever
		instrumentPosition newPos;
		newPos.positionType = instrumentType::CASH;
		newPos.ticker = "USD";
		newPos.quantity = totalGained;
		newPos.averageBuyPrice = 1.0;
		cashes.push_back(newPos);
	}
	else {
		cashes[usdIndex].quantity += totalGained;
	}

	return totalGained;
}


bool RTPortfolio::loadActivePricesStocks() {
	StockDataChannel apiSource;
	for (auto&& loadPos : stocks) {
		auto loadedVal = apiSource.getActivePrice(loadPos.ticker);
		if (!loadedVal) {
			cout << "Error, failed to load stock price\n";
			return false;
		}
		loadPos.activePrice = *loadedVal;
	}
	return true;
}

bool RTPortfolio::loadActivePricesCash() {
	CashDataChannel apiSource;
	for (auto&& loadPos : cashes) {
		if (loadPos.ticker == "USD") {
			loadPos.activePrice = 1.0;
			continue;
		}
		auto loadedVal = apiSource.getConversionRate(loadPos.ticker);
		if (!loadedVal) {
			cout << "Error, failed to load cash price\n";
			return false;
		}
		loadPos.activePrice = *loadedVal;
	}
	return true;
}

bool RTPortfolio::loadActivePricesCrypto() {
	CryptoDataChannel apiSource;
	for (auto&& loadPos : cryptos) {
		auto loadedVal = apiSource.getActivePrice(loadPos.ticker);
		if (!loadedVal) {
			cout << "Error, failed to load crypto price\n";
			return false;
		}
		loadPos.activePrice = *loadedVal;
	}
	return true;
}

bool RTPortfolio::loadActivePrices() {
	if (loadActivePricesStocks() && loadActivePricesCash() && loadActivePricesCrypto()) {	//if even one of them false, return false as failed
		return true;
	}
	else {
		return false;
	}
}

bool RTPortfolio::loadActiveYieldsStocks() {
	StockDataChannel apiSource;
	for (auto&& loadPos : stocks) {
		auto optDividend = apiSource.getActiveDividend(loadPos.ticker);
		auto optPrice = apiSource.getActivePrice(loadPos.ticker);

		if (!optDividend || !optPrice || *optPrice <= 0.0) {
			cout << "Error, failed to load stock yield/price for calculation\n";
			return false;
		}

		loadPos.yield = *optDividend / *optPrice; // annualized divi/price = divirate
	}
	return true;
}

bool RTPortfolio::loadActiveYieldsCash() {
	CashDataChannel apiSource;
	for (auto&& loadPos : cashes) {
		auto loadedVal = apiSource.getInterestRate(loadPos.ticker);
		if (!loadedVal) {
			cout << "Error, failed to load interest rate\n";
			return false;
		}
		loadPos.yield = *loadedVal;
	}
	return true;
}

bool RTPortfolio::loadActiveYields() {
	if (loadActiveYieldsStocks() && loadActiveYieldsCash()) {	// if even one is false, whole loading was false
		return true;
	}
	else {
		return false;
	}
}

bool RTPortfolio::saveSnapshot() const {
	Snapshoter snp(storagePath);
	if (snp.writeSnapshot(*this)) {
		return true;
	}
	return false;
}

bool RTPortfolio::loadSnapshot(chrono::year_month_day date) {
	Snapshoter snp(storagePath);
	optional<RTPortfolio> loaded = snp.readSnapshot(date);
	if (loaded == nullopt) {
		return false;
	}
	stocks = loaded.value().stocks;
	cashes = loaded.value().cashes;
	cryptos = loaded.value().cryptos;
	totalDeposited = loaded.value().totalDeposited;
	totalWithdrawn = loaded.value().totalWithdrawn;
	return true;
}