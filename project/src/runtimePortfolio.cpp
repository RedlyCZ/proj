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


int RTPortfolio::findTickerIndex(const string& newTicker, const vector<instrumentPosition>& container) {
	for (int i = 0; i < container.size(); i++) {
		if (container[i].ticker == newTicker) {
			return i;
		}
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

vector<instrumentPosition>* RTPortfolio::getContainer(instrumentType type) {
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

double RTPortfolio::getActivePrice(instrumentType type, const string& ticker) {
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
	return -1.0;
}


double RTPortfolio::buyInstrument(instrumentType type, const string& newTicker, double newQuantity) {
	double activePrice = getActivePrice(type, newTicker);
	vector<instrumentPosition>* selectedContainer = getContainer(type);

	if (activePrice < 0) {
		cout << "Error 3, failed to load price, buying\n";
		return 0;
	}
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
		selectedContainer->push_back(newPos);
	}
	else {
		instrumentPosition& editPos = (*selectedContainer)[posIndex];
		double totalQuantity = editPos.quantity + newQuantity;
		double totalInvested = (editPos.quantity * editPos.averageBuyPrice) + totalCost;
		editPos.averageBuyPrice = totalInvested / totalQuantity;
		editPos.quantity = totalQuantity;
	}

	return totalCost;
}

double RTPortfolio::sellInstrument(instrumentType type, const string& newTicker, double newQuantity, bool closePosition) {
	double activePrice = getActivePrice(type, newTicker);
	vector<instrumentPosition>* selectedContainer = getContainer(type);

	if (activePrice < 0) {
		cout << "Error 3, failed to load price, selling\n";
		return 0;
	}
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
	for (size_t i = 0; i < stocks.size(); i++) {
		instrumentPosition& loadPos = stocks[i];
		double loadedVal = apiSource.getActivePrice(loadPos.ticker);
		if (loadedVal < 0) {
			cout << "Error, failed to load stock price\n";
			return false;
		}
		else {
			loadPos.activePrice = loadedVal;
		}
	}
	return true;
}

bool RTPortfolio::loadActivePricesCash() {
	CashDataChannel apiSource;
	for (size_t i = 0; i < cashes.size(); i++) {
		instrumentPosition& loadPos = cashes[i];
		double loadedVal = apiSource.getConversionRate(loadPos.ticker);
		if (loadedVal < 0) {
			cout << "Error, failed to load cash price\n";
			return false;
		}
		else {
			loadPos.activePrice = loadedVal;
		}
	}
	return true;
}

bool RTPortfolio::loadActivePricesCrypto() {
	CryptoDataChannel apiSource;
	for (size_t i = 0; i < cryptos.size(); i++) {
		instrumentPosition& loadPos = cryptos[i];
		double loadedVal = apiSource.getActivePrice(loadPos.ticker);
		if (loadedVal < 0) {
			cout << "Error, failed to load cash price\n";
			return false;
		}
		else {
			loadPos.activePrice = loadedVal;
		}
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
	for (size_t i = 0; i < stocks.size(); i++) {
		instrumentPosition& loadPos = stocks[i];
		double loadedVal = apiSource.getActiveDividend(loadPos.ticker) / apiSource.getActivePrice(loadPos.ticker); //annualized divi/price = divirate
		if (loadedVal < 0) {
			cout << "Error, failed to load stock yield\n";
			return false;
		}
		else {
			loadPos.yield = loadedVal;
		}
	}
	return true;
}

bool RTPortfolio::loadActiveYieldsCash() {
	CashDataChannel apiSource;
	for (size_t i = 0; i < cashes.size(); i++) {
		instrumentPosition& loadPos = cashes[i];
		double loadedVal = apiSource.getInterestRate(loadPos.ticker);
		if (loadedVal < 0) {
			cout << "Error, failed to load interest rate\n";
			return false;
		}
		else {
			loadPos.yield = loadedVal;
		}
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

bool RTPortfolio::saveSnapshot() {
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