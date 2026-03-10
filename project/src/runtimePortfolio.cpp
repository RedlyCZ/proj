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

double RTPortfolio::buyInstrument(instrumentType type, const string& newTicker, double newQuantity) {
	double activePrice = 0;
	vector<instrumentPosition>* selectedContainer = nullptr;
	switch(type) {
	case instrumentType::STOCK: {
		StockDataChannel stockApi;
		activePrice = stockApi.getActivePrice(newTicker);
		selectedContainer = &stocks;
		break;
	}
	case instrumentType::CASH: {
		CashDataChannel cashApi;
		activePrice = cashApi.getConversionRate(newTicker);
		selectedContainer = &cashes;
		break;
	}
	case instrumentType::CRYPTO: {
		CryptoDataChannel cryptoApi;
		activePrice = cryptoApi.getActivePrice(newTicker);
		selectedContainer = &cryptos;
		break;
	}
	}
	if (activePrice < 0) {
		cout << "Error 3, failed to load price, buying\n";
		return 0;
	}
	if (selectedContainer == nullptr) {
		cout << "Error, invalid instrument type\n";
		return 0;
	}

	int posIndex = findTickerIndex(newTicker, *selectedContainer);

	if (posIndex == -1) {				//means ticker not present in vector
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
		double totalInvested = (editPos.quantity * editPos.averageBuyPrice) + (newQuantity * activePrice);
		editPos.averageBuyPrice = totalInvested / totalQuantity;
		editPos.quantity = totalQuantity;
	}

	return activePrice * newQuantity; //returns cost
}

double RTPortfolio::sellInstrument(instrumentType type, const string& newTicker, double newQuantity, bool closePosition) {
	double activePrice = 0;
	vector<instrumentPosition>* selectedContainer = nullptr;
	switch (type) {
	case instrumentType::STOCK: {
		StockDataChannel stockApi;
		activePrice = stockApi.getActivePrice(newTicker);
		selectedContainer = &stocks;
		break;
	}
	case instrumentType::CASH: {
		CashDataChannel cashApi;
		activePrice = cashApi.getConversionRate(newTicker);
		selectedContainer = &cashes;
		break;
	}
	case instrumentType::CRYPTO: {
		CryptoDataChannel cryptoApi;
		activePrice = cryptoApi.getActivePrice(newTicker);
		selectedContainer = &cryptos;
		break;
	}
	}
	if (activePrice < 0) {
		cout << "Error 3, failed to load price, selling\n";
		return 0;
	}
	if (selectedContainer == nullptr) {
		cout << "Error, invalid instrument type\n";
		return 0;
	}

	int posIndex = findTickerIndex(newTicker, *selectedContainer);

	if (posIndex == -1) {				//means ticker not present in vector
		cout << "Error 8, cant sell unbuyed\n";
		return 0;
	}
	else {
		instrumentPosition& editPos = (*selectedContainer)[posIndex];
		if (closePosition) { //if we are closing, newQuantity is worthless parameter
			double savedQuantity = editPos.quantity;
			(*selectedContainer).erase((*selectedContainer).begin() + posIndex);
			return activePrice * savedQuantity;
		}
		else if (editPos.quantity < newQuantity) {
			cout << "Error 9, cant sell more than you have\n";
			return 0;
		}
		else {
			editPos.quantity = editPos.quantity - newQuantity;
			//selling doesnt affect averageBuyPrice
		}
	}
	return activePrice * newQuantity; //returns money gained from sell
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
	return true;
}


