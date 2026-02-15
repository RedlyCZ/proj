#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <string>

#ifndef API_MIDDLEMAN
#define API_MIDDLEMAN

class finnHubChannel {
public:
	double getActivePrice(std::string ticker);
};


#endif