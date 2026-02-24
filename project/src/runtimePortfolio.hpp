#include <vector>
#include <string>
#include <array>


#ifndef RUNTIME_PORTFOLIO
#define RUNTIME_PORTFOLIO

//definition of active types

constexpr int supportedTypesCount = 3;

enum class instrumentType {
	STOCK,
	CASH,
	CRYPTO
};

struct instrumentPosition {
public:
	instrumentType positionType;
	std::string ticker;
	double quantity;
	double activePrice;
	double yield;
	double averageBuyPrice;
};

class RTPortfolio {
private:
	std::vector<instrumentPosition> stocks;
	std::vector<instrumentPosition> cashes;
	std::vector<instrumentPosition> cryptos;
	std::array<int, supportedTypesCount> valueByClasses;
	double totalValue;
	int findTickerIndex(const std::string& newTicker, const std::vector<instrumentPosition>& container);
	void loadActivePricesStocks();
	void loadActivePricesCash();
	void loadActivePricesCrypto();
	void loadActiveYieldsStocks();
	void loadActiveYieldsCash();

public:
	double buyInstrument(instrumentType type, const std::string& newTicker, double newQuantity);							//returns price spent
	double sellInstrument(instrumentType type, const std::string& newTicker, double newQuantity, bool closePosition);		//returns price recieved
	void loadActivePrices();
	void loadActiveYields();
	void printAllPositions();
};


#endif