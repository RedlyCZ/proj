#include "runtimePortfolio.hpp"
#include "apiMiddleman.hpp"
#include <vector>
#include <string>
#include <array>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;


int RTPortfolio::findTickerIndex(const string& newTicker, vector<instrumentPosition>& container) {
	for (int i = 0; i < container.size(); i++) {
		if (container[i].ticker == newTicker) {
			return i;
		}
	}
	return -1;
}

double RTPortfolio::buyInstrument(instrumentType type, const string& newTicker, double newQuantity) {
	double activePrice;
	vector<instrumentPosition>* selectedContainer = nullptr;
	switch(type) {
	case instrumentType::STOCK: {
		FinnHubChannel apiSource;
		activePrice = apiSource.getActivePrice(newTicker);
		selectedContainer = &stocks;
		break;
	}
	case instrumentType::CASH: {
		FrankfurterChannel apiSourceTwo;
		activePrice = apiSourceTwo.conversionRate(newTicker);
		selectedContainer = &cashes;
		break;
	}
	case instrumentType::CRYPTO: {
		CoinGeckoChannel apiSourceThree;
		activePrice = apiSourceThree.getActivePrice(newTicker);
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
		FinnHubChannel apiSource;
		activePrice = apiSource.getActivePrice(newTicker);
		selectedContainer = &stocks;
		break;
	}
	case instrumentType::CASH: {
		FrankfurterChannel apiSourceTwo;
		activePrice = apiSourceTwo.conversionRate(newTicker);
		selectedContainer = &cashes;
		break;
	}
	case instrumentType::CRYPTO: {
		CoinGeckoChannel apiSourceThree;
		activePrice = apiSourceThree.getActivePrice(newTicker);
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
	FinnHubChannel apiSource;
	for (size_t i = 0; i < stocks.size(); i++) {
		instrumentPosition& loadPos = stocks[i];
		loadPos.activePrice = apiSource.getActivePrice(loadPos.ticker);
	}
}

void RTPortfolio::loadActivePricesCash() {
	FrankfurterChannel apiSource;
	for (size_t i = 0; i < cashes.size(); i++) {
		instrumentPosition& loadPos = cashes[i];
		loadPos.activePrice = apiSource.conversionRate(loadPos.ticker);
	}
}

void RTPortfolio::loadActivePricesCrypto() {
	CoinGeckoChannel apiSource;
	for (size_t i = 0; i < cashes.size(); i++) {
		instrumentPosition& loadPos = cryptos[i];
		loadPos.activePrice = apiSource.conversionRate(loadPos.ticker);
	}
}

void RTPortfolio::loadActivePrices() {
	loadActivePricesStocks();
	loadActivePricesCash();
	loadActivePricesCrypto();
}


