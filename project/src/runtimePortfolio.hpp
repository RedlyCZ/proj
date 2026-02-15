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
	std::vector<instrumentPosition>::iterator findTicker(std::string newTicker, std::vector<instrumentPosition>& container);

public:
	void buyStock(std::string newTicker, double newQuantity);
	void buyCash(std::string newTicker, double newQuantity);
	void buyCrypto(std::string newTicker, double newQuantity);
	void loadActivePrices();
	void loadActiveYields();
	void printAllPositions();
};


#endif