/*
 Copyright (C) 2024 Vannarho Pty Ltd
 All rights reserved.

 This file is part of VRE, a free-software/open-source library
 for transparent pricing and risk analysis

 VRE is free software: you can redistribute it and/or modify it
 under the terms of the Modified BSD License.  You should have received a
 copy of the license along with this program.


 This program is distributed on the basis that it will form a useful
 contribution to risk analytics and model standardisation, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
*/


/*! \file /OREAnalytics/orea/saccrv/calculation/groupcommtrades.cpp
    \brief SACCRV impl file to define the interface for grouping trades by hedging sets and commodity types.
    \ingroup saccrv
*/


#include <orea/saccrv/calculation/groupcommtrades.hpp>
#include <orea/saccrv/saccrvcalculationmanager.hpp>
#include <string>
#include <set>

namespace ore {
namespace analytics {

GroupCommodityTrades::GroupCommodityTrades() {}

void GroupCommodityTrades::groupCommodityTrades(const std::vector<SaccrvTrades>& groupTrades,  std::string nettingSetId, const std::string& counterpartyId,const std::string& hedgingSetName) {

    // Get the singleton instance of SaccrvCalculationManager
    LOG("SACCRV: groupCommTrades called with nettingSetId: " << nettingSetId << " and hedgingSetName: " << hedgingSetName);
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

    auto& commodityData = nettingSet->assetClasses.commodity;


    // Extract unique hedging sets and commodity types
    std::vector<std::string> hedgingSets = getUniqueSubClasses(groupTrades);
    std::vector<std::string> commodityTypes = getUniqueCommodityTypes(groupTrades);

    for (auto& originalHedgingSet : hedgingSets) {
        std::string modifiedHedgingSet = originalHedgingSet;

        // Apply special treatment for "Vol_" or "Basis_" prefixes
        if (originalHedgingSet.substr(0, 4) == "Vol_" || originalHedgingSet.substr(0, 6) == "Basis_") {
            modifiedHedgingSet = "_" + originalHedgingSet;
        }

        // Create a new HedgingSet
        HedgingSet hedgingSet;
        // Set the hedgingSet field to the modifiedHedgingSet
        hedgingSet.hedgingSet = modifiedHedgingSet;

        // Add the hedgingSet to the commodityData
        commodityData.hedgingSets.push_back(hedgingSet);

        // Group trades by commodity type under the current hedging set
        for (const auto& commodityType : commodityTypes) {

            // Create a new CommodityType
            CommodityType commodityT_;
        
            // Set the commodity field to the commodityType
            commodityT_.commodity = commodityType;

            std::string subclass;

            for (const auto& trade : groupTrades) {
                if (trade.commodityType == commodityType) {

                    // Create a new TradeData
                    TradeData tradeData;

                    // Add the trade to the tradeData
                    tradeData.trades = trade;

                    
                    if (!subclass.empty() && subclass != trade.subClass) {
                        // Handle the error - different subclasses for the same reference entity
                        WLOG("Different subclasses found for the same commodity type entity: " << trade.commodityType);
                    }
                    subclass = trade.subClass; // Set the subclass for the hedgingset

                    // add tradeData to the commodityT_
                    commodityT_.tradeData.push_back(tradeData);
                }
            }

            // Set the subclass for the commodityT_ after confirming it's consistent across all trades
            commodityT_.subClass = subclass;

            // Add the commodityType to the hedgingSet
            hedgingSet.commodityTypes.push_back(commodityT_);
        }
    }
}

std::vector<std::string> GroupCommodityTrades::getUniqueSubClasses(const std::vector<SaccrvTrades>& trades) {
    std::set<std::string> uniqueSubClasses;
    for (const auto& trade : trades) {
        uniqueSubClasses.insert(trade.subClass);
    }
    return std::vector<std::string>(uniqueSubClasses.begin(), uniqueSubClasses.end());
}

std::vector<std::string> GroupCommodityTrades::getUniqueCommodityTypes(const std::vector<SaccrvTrades>& trades) {
    std::set<std::string> uniqueCommodityTypes;
    for (const auto& trade : trades) {
        uniqueCommodityTypes.insert(trade.commodityType);
    }
    return std::vector<std::string>(uniqueCommodityTypes.begin(), uniqueCommodityTypes.end());
}

} // namespace analytics
} // namespace ore
