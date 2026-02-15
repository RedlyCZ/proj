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

void RTPortfolio::buyInstrument(instrumentType type, const string& newTicker, double newQuantity) {
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
		cout << "Error 3, failed to load price\n";
		return;
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
}

