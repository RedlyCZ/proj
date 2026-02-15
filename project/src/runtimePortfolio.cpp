#include "runtimePortfolio.hpp"
#include "apiMiddleman.hpp"
#include <vector>
#include <string>
#include <array>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;


vector<instrumentPosition>::iterator RTPortfolio::findTicker(string newTicker, vector<instrumentPosition>& container) {
	for (auto it = container.begin(); it != container.end(); it++) {
		if (it->ticker == newTicker) {
			return it;
		}
	}
	return container.end();
}

void RTPortfolio::buyStock(string newTicker, double newQuantity) {
	finnHubChannel apiSource;
	double activePrice = apiSource.getActivePrice(newTicker);
	if (activePrice < 0) {
		cout << "Error 3, failed to load price\n";
		return;
	}
	auto it = findTicker(newTicker, stocks);
	if (it == stocks.end()) {
		instrumentPosition newPos; //create new empty pos
		newPos.positionType = instrumentType::STOCK;
		newPos.ticker = newTicker;
		newPos.quantity = newQuantity;
		newPos.averageBuyPrice = activePrice;
		stocks.push_back(newPos);
	}
	else {
		instrumentPosition& editPos = *it;
		double activePrice = activePrice;
		double totalQuantity = editPos.quantity + newQuantity;
		double totalInvested = (editPos.quantity * editPos.averageBuyPrice) + (newQuantity * activePrice);
		editPos.averageBuyPrice = totalInvested / totalQuantity;
		editPos.quantity = totalQuantity;
	}
}

