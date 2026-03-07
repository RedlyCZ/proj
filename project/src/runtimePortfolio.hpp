#ifndef RUNTIME_PORTFOLIO
#define RUNTIME_PORTFOLIO

#include <vector>
#include <string>
#include <array>

//definition of active types

constexpr int supportedTypesCount = 3;

enum class instrumentType {
	STOCK,
	CASH,
	CRYPTO
};

struct instrumentPosition {
public:
	instrumentType positionType{ instrumentType::STOCK };
	std::string ticker = "";
	double quantity = 0.0;
	double activePrice = 0.0;
	double yield = 0.0;
	double averageBuyPrice = 0.0;
};

class RTPortfolio {
private:
	std::array<int, supportedTypesCount> valueByClasses{};
	double totalValue = 0;
	int findTickerIndex(const std::string& newTicker, const std::vector<instrumentPosition>& container);
	bool loadActivePricesStocks(); //return false if api failed
	bool loadActivePricesCash();
	bool loadActivePricesCrypto();
	bool loadActiveYieldsStocks();
	bool loadActiveYieldsCash();

public:
	std::vector<instrumentPosition> stocks;
	std::vector<instrumentPosition> cashes;
	std::vector<instrumentPosition> cryptos;
	double buyInstrument(instrumentType type, const std::string& newTicker, double newQuantity);							//returns price spent
	double sellInstrument(instrumentType type, const std::string& newTicker, double newQuantity, bool closePosition);		//returns price recieved
	bool loadActivePrices();	//return false if loading failed
	bool loadActiveYields();
	void printAllPositions();
};


#endif