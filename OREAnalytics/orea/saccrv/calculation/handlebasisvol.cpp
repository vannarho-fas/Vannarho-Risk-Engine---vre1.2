/*! \file /OREAnalytics/orea/saccrv/calculation/handlebasisvol.cpp
    \brief Header for the trade categorization function, classifying trades into basis, volatility or 'normal' transactions.
    \ingroup saccrv
*/

#include <orea/saccrv/calculation/handlebasisvol.hpp>

namespace ore {
namespace analytics {

CategorizedTrades TradeCategorizer::handleBasisVol(const std::vector<std::shared_ptr<SaccrvTrades>>& trades) {
    CategorizedTrades categorizedTrades;
    std::map<std::string, std::vector<std::string>> basisTradeIds;
    std::set<std::string> basisHedgingSets;

    for (const auto& trade : trades) {
        if (trade->tradeType == "Vol") {
            categorizedTrades.tradeIds["Vol_" + trade->underlyingInstrument].push_back(trade->getId());
        } else if (trade->tradeType == "Swap") {
            if (isBasisSwap(*trade)) {
                std::string setKey = trade->payLegRef + " " + trade->recLegRef;
                basisTradeIds[setKey].push_back(trade->getId());
            }
        }
    }

    for (const auto& kv : basisTradeIds) {
        std::string hedgingSetName = "Basis_" + kv.first;
        categorizedTrades.tradeIds[hedgingSetName] = kv.second;
        basisHedgingSets.insert(hedgingSetName);
        categorizedTrades.tradeIdsAll.insert(categorizedTrades.tradeIdsAll.end(), kv.second.begin(), kv.second.end());
    }

    for (const auto& kv : categorizedTrades.tradeIds) {
        if (kv.first.rfind("Vol_", 0) == 0) { // Check if key starts with "Vol_"
            categorizedTrades.hedgingSets.insert(kv.first);
            categorizedTrades.tradeIdsAll.insert(categorizedTrades.tradeIdsAll.end(), kv.second.begin(), kv.second.end());
        }
    }

    // Convert set to vector for final result's tradeIdsAll
    categorizedTrades.tradeIdsAll.assign(categorizedTrades.tradeIdsAll.begin(), categorizedTrades.tradeIdsAll.end());

    return categorizedTrades;
}

bool TradeCategorizer::isBasisSwap(const SaccrvTrades& trade) {
    // Check if the trade is a Swap
    if (trade.tradeType != "Swap") {
        return false;
    }

    // Check if the pay leg and receive leg reference different indices
    if (trade.payLegRef != trade.recLegRef) {
        return true;
    }

    return false;
}

} // namespace analytics
} // namespace ore
