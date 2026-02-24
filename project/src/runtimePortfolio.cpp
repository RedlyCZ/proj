#include "runtimePortfolio.hpp"
#include "apiMiddleman.hpp"
#include <vector>
#include <string>
#include <array>
#include <iostream>
#include <nlohmann/json.hpp>

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
	double activePrice;
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


void RTPortfolio::loadActivePricesStocks() {
	StockDataChannel apiSource;
	for (size_t i = 0; i < stocks.size(); i++) {
		instrumentPosition& loadPos = stocks[i];
		loadPos.activePrice = apiSource.getActivePrice(loadPos.ticker);
	}
}

void RTPortfolio::loadActivePricesCash() {
	CashDataChannel apiSource;
	for (size_t i = 0; i < cashes.size(); i++) {
		instrumentPosition& loadPos = cashes[i];
		loadPos.activePrice = apiSource.getConversionRate(loadPos.ticker);
	}
}

void RTPortfolio::loadActivePricesCrypto() {
	CryptoDataChannel apiSource;
	for (size_t i = 0; i < cryptos.size(); i++) {
		instrumentPosition& loadPos = cryptos[i];
		loadPos.activePrice = apiSource.getActivePrice(loadPos.ticker);
	}
}

void RTPortfolio::loadActivePrices() {
	loadActivePricesStocks();
	loadActivePricesCash();
	loadActivePricesCrypto();
}

void RTPortfolio::loadActiveYieldsStocks() {
	StockDataChannel apiSource;
	for (size_t i = 0; i < stocks.size(); i++) {
		instrumentPosition& loadPos = stocks[i];
		loadPos.yield = apiSource.getActiveDividend(loadPos.ticker) / apiSource.getActivePrice(loadPos.ticker); //annualized divi / price = divirate
	}
}

void RTPortfolio::loadActiveYieldsCash() {
	CashDataChannel apiSource;
	for (size_t i = 0; i < cashes.size(); i++) {
		instrumentPosition& loadPos = cashes[i];
		loadPos.yield = apiSource.getInterestRate(loadPos.ticker);
	}
}

void RTPortfolio::loadActiveYields() {
	loadActiveYieldsStocks();
	loadActiveYieldsCash();
}

void RTPortfolio::printAllPositions() {
	cout << "Complete portfolio print\n";
	cout << "Stocks:\n";
	for (size_t i = 0; i < stocks.size(); i++) {
		cout << "Ticker: " << stocks[i].ticker << " Active Price: " << stocks[i].activePrice << " Active Yield: " << stocks[i].yield << " Quantity: " << stocks[i].quantity << "\n";
	}

	cout << "Crypto:\n";
	for (size_t i = 0; i < cryptos.size(); i++) {
		cout << "Name: " << cryptos[i].ticker << " Active Price: " << cryptos[i].activePrice << " Quantity: " << cryptos[i].quantity << "\n";
	}

	cout << "Cashes:\n";
	for (size_t i = 0; i < cashes.size(); i++) {
		cout << "Ticker: " << cashes[i].ticker << " Active Price: " << cashes[i].activePrice << " Active Yield: " << cashes[i].yield << " Quantity: " << cashes[i].quantity << "\n";
	}
}


