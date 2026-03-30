#ifndef RUNTIME_PORTFOLIO
#define RUNTIME_PORTFOLIO

#include <vector>
#include <string>
#include <array>
#include <chrono>

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
	int findTickerIndex(const std::string& newTicker, const std::vector<instrumentPosition>& container);
	std::vector<instrumentPosition>* getContainer(instrumentType type);
	double getActivePrice(instrumentType type, const std::string& ticker);
	bool loadActivePricesStocks(); //return false if api failed
	bool loadActivePricesCash();
	bool loadActivePricesCrypto();
	bool loadActiveYieldsStocks();
	bool loadActiveYieldsCash();
	double totalDeposited = 0;
	double totalWithdrawn = 0;
public:
	std::vector<instrumentPosition> stocks;
	std::vector<instrumentPosition> cashes;
	std::vector<instrumentPosition> cryptos;
	std::string storagePath;
	void setStoragePath(const std::string& path) {
		storagePath = path;
	}
	bool depositCash(double quantity);			//for simplicity only takes dollars as the base currency
	bool withdrawCash(double quantity);
	double buyInstrument(instrumentType type, const std::string& newTicker, double newQuantity);							//returns price spent
	double sellInstrument(instrumentType type, const std::string& newTicker, double newQuantity, bool closePosition);		//returns price recieved
	bool loadActivePrices();	//return false if loading failed
	bool loadActiveYields();
	bool saveSnapshot();
	bool loadSnapshot(std::chrono::year_month_day searchedDate);
};


#endif