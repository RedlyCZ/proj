#ifndef RUNTIME_PORTFOLIO
#define RUNTIME_PORTFOLIO

#include <vector>
#include <string>
#include <array>
#include <chrono>
#include <optional>

class Snapshoter;

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
	friend class Snapshoter;

private:
	std::vector<instrumentPosition> stocks;
	std::vector<instrumentPosition> cashes;
	std::vector<instrumentPosition> cryptos;
	std::string storagePath;
	double totalDeposited = 0;
	double totalWithdrawn = 0;

	int findTickerIndex(const std::string& newTicker, const std::vector<instrumentPosition>& container) const noexcept;
	std::vector<instrumentPosition>* getContainer(instrumentType type) noexcept;
	std::optional<double> getActivePrice(instrumentType type, const std::string& ticker) const;
	bool loadActivePricesStocks(); //return false if api failed
	bool loadActivePricesCash();
	bool loadActivePricesCrypto();
	bool loadActiveYieldsStocks();
	bool loadActiveYieldsCash();

public:
	const std::vector<instrumentPosition>& getStocks() const noexcept { return stocks; }
	const std::vector<instrumentPosition>& getCashes() const noexcept { return cashes; }
	const std::vector<instrumentPosition>& getCryptos() const noexcept { return cryptos; }
	double getTotalDeposited() const noexcept { return totalDeposited; }
	double getTotalWithdrawn() const noexcept { return totalWithdrawn; }
	const std::string& getStoragePath() const noexcept { return storagePath; }

	void setStoragePath(const std::string& path) { storagePath = path; }

	bool depositCash(double quantity);			//for simplicity only takes dollars as the base currency
	bool withdrawCash(double quantity);
	double buyInstrument(instrumentType type, const std::string& newTicker, double newQuantity);							//returns price spent
	double sellInstrument(instrumentType type, const std::string& newTicker, double newQuantity, bool closePosition);		//returns price recieved
	bool loadActivePrices();	//return false if loading failed
	bool loadActiveYields();
	bool saveSnapshot() const;
	bool loadSnapshot(std::chrono::year_month_day searchedDate);
};

#endif