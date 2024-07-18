/*! \file /OREAnalytics/orea/saccrv/calculation/groupirdtrades.cpp
    \brief GroupIRDTrades implementation manages the grouping of interest rate derivative trades for SACCR calculation.
    \ingroup saccrv
*/

#include <orea/saccrv/saccrvcalculationmanager.hpp>
#include <orea/saccrv/calculation/groupirdtrades.hpp>
#include <algorithm>

namespace ore {
namespace analytics {

GroupIRDTrades::GroupIRDTrades() {}

void GroupIRDTrades::groupIRDTrades(const std::vector<SaccrvTrades>& groupTrades, const std::string& nettingSetId, const std::string& counterpartyId, const std::string& hedgingSetName) {
    // Get the singleton instance of SaccrvCalculationManager
    LOG("SACCRV: groupIRDTrades called with nettingSetId: " << nettingSetId << " and hedgingSetName: " << hedgingSetName);
    auto saccrvCalcManager = SaccrvCalculationManager::getInstance();
    NettingSet* nettingSet = saccrvCalcManager->getNettingSet(counterpartyId, nettingSetId);

    if (nettingSet != nullptr) {
        LOG("SACCRV: nettingset not null ");
    }
    else {
        // Handle the case where the netting set is not found
        WLOG("Error: Netting set not found for counterpartyId: " << counterpartyId << " and nettingSetId: " << nettingSetId);
        throw std::runtime_error("Netting set not found");
        return;
    }

    auto& irdData = nettingSet->assetClasses.ird;

    // Extract unique currencies
    std::vector<std::string> currencies = getUniqueCurrencies(groupTrades);
    LOG("SACCRV: GroupIRDTrades::getUniqueCurrencies - Number of unique currencies: " << currencies.size());

    for(const auto& originalCurrency : currencies) {
        LOG("SACCRV: GroupIRDTrades::groupIRDTrades originalCurrency: " << originalCurrency);
        std::string modifiedCurrency = originalCurrency;

        // Apply special treatment for "Vol_" or "Basis_" prefixes
        if (hedgingSetName.substr(0, 4) == "Vol_" || hedgingSetName.substr(0, 6) == "Basis_") {
            modifiedCurrency = hedgingSetName + "_" + originalCurrency;
        }

        // Create a new Currency
        LOG("SACCRV: GroupIRDTrades::groupIRDTrades creating new CurrencyData");
        CurrencyData currency;
        // Set the currency field to the modifiedCurrency
        currency.currency = modifiedCurrency;

        // // Add the currency to the irdData
        // irdData.currencies.push_back(currency);

        // Group trades by time buckets under the current currency
        std::map<int, std::vector<SaccrvTrades>> groupedTrades = groupTradesByTimeBucket(groupTrades, originalCurrency);

        for (const auto& pair : groupedTrades) {
            const int& timeBucket = pair.first;
            const std::vector<SaccrvTrades>& tradesForBucket = pair.second;

            // Create a new TimeBucket
            TimeBucket timeBucket_;
            // Set the timeBucket field to the timeBucket
            timeBucket_.timeBucketNum = timeBucket;

            for (const auto& trade : tradesForBucket) {
                // Create a new TradeData
                TradeData tradeData;
                // Set the trade field to the trade
                tradeData.trades = trade;
                // Add tradeData to the timeBucketNode
                timeBucket_.tradeData.push_back(tradeData);
                LOG("SACCRV GroupIRDTrades::groupIRDTrades - Added tradeData to the timeBucketNode ")
            }

            // Add the timeBucketNode to the currency
            currency.timeBuckets.push_back(timeBucket_);
            LOG("SACCRV: GroupIRDTrades::groupIRDTrades currency.timeBuckets added: ");
        }

        // Add the currency to the irdData
        irdData.currencies.push_back(currency);
        LOG("SACCRV: GroupIRDTrades::groupIRDTrades irdData.currencies added ");
    }
}

std::vector<std::string> GroupIRDTrades::getUniqueCurrencies(const std::vector<SaccrvTrades>& trades) {
    std::set<std::string> uniqueCurrencies;
    for (const auto& trade : trades) {
        uniqueCurrencies.insert(trade.getCurrency());
    }
    return std::vector<std::string>(uniqueCurrencies.begin(), uniqueCurrencies.end());
}

std::map<int, std::vector<SaccrvTrades>> GroupIRDTrades::groupTradesByTimeBucket(const std::vector<SaccrvTrades>& trades, const std::string& currency) {
    std::map<int, std::vector<SaccrvTrades>> groupedTrades;
    std::vector<SaccrvTrades> trade_data = trades;
    for (auto& trade : trade_data) {
        if (trade.getCurrency() == currency) {
            // set the timebucket based on the maturity of the trade
            auto ei = trade.getEi();
            if (ei.has_value()) {
                trade.setTimeBucket(calcTimeBucket(ei.value()));
            } else {
                // Handle the case where getEi() does not return a value
                WLOG("Ei value not found for IRD trade: " << trade.getId());
                trade.setTimeBucket(0);
            }
            LOG("SACCRV GroupIRDTrades::groupIRDTrades - trade.timeBucket set: " << trade.getTimeBucket())
            groupedTrades[trade.getTimeBucket()].push_back(trade);
        }
    }
    return groupedTrades;
}


int GroupIRDTrades::calcTimeBucket(double Ei) {
    // sets the time bucket based on the maturity of the trade
    int timebucket = 0;
    if(Ei <= 1) {
        timebucket = 1;
    } else if(Ei > 1 && Ei <= 5) {
        timebucket = 2;
    } else if(Ei > 5) {
        timebucket = 3;
    }
    return timebucket;
}
                     

} // namespace analytics
} // namespace ore