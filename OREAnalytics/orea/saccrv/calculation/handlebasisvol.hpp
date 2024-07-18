
/*! \file /OREAnalytics/orea/saccrv/calculation/handlebasisvol.hpp
    \brief Header for the trade categorization function, classifying trades into basis, volatility or 'normal' transactions.
    \ingroup saccrv
*/

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include <optional>
#include <orea/saccrv/saccrvinputmanager.hpp>

namespace ore {
namespace analytics {

struct CategorizedTrades {
    std::map<std::string, std::vector<std::string>> tradeIds; // The key is the hedging set name
    std::vector<std::string> tradeIdsAll;
    std::set<std::string> hedgingSets;
};

class TradeCategorizer {
public:
    CategorizedTrades handleBasisVol(const std::vector<std::shared_ptr<SaccrvTrades>>& trades);

private:
    bool isBasisSwap(const SaccrvTrades& trade);
};

} // namespace analytics
} // namespace ore
