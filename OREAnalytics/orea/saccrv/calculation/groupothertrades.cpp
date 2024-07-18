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

/*!
    \file /OREAnalytics/orea/saccrv/calculation/groupothertrades.cpp
    \brief Header file for the GroupOtherTrades functionality for SACCR as part of ORE's architecture.
    \ingroup saccrv
*/

#include <orea/saccrv/saccrvcalculationmanager.hpp>
#include <orea/saccrv/calculation/groupothertrades.hpp>

namespace ore {
namespace analytics {

GroupOtherTrades::GroupOtherTrades() {}

void GroupOtherTrades::groupOtherTrades(const std::vector<SaccrvTrades>& groupTrades, const std::string& nettingSetId, const std::string& counterpartyId,const std::string& hedgingSetName) {
    // Get the singleton instance of SaccrvCalculationManager
    auto saccrvCalcManager = SaccrvCalculationManager::getInstance();

    // Access the OtherExposureData object
    auto& nettingSets = saccrvCalcManager->getCurrentCounterparty()->nettingSets;
    auto nettingSetIt = std::find_if(nettingSets.begin(), nettingSets.end(), [&nettingSetId](const NettingSet& ns) {
        return ns.id == nettingSetId;
    });
    if (nettingSetIt == nettingSets.end()) {
        // Handle the error - netting set not found
        WLOG("Netting set not found: " << nettingSetId);
        return;
    }
    auto& otherExposureData = nettingSetIt->assetClasses.otherExposure;

    // Extract unique subclasses
    std::vector<std::string> subClasses = getUniqueSubClasses(groupTrades);

    for(const auto& originalSubClass : subClasses) {
        std::string modifiedSubClass = originalSubClass;

        // Apply special treatment for "Vol_" or "Basis_" prefixes
        if (hedgingSetName.substr(0, 4) == "Vol_" || hedgingSetName.substr(0, 6) == "Basis_") {
            modifiedSubClass = hedgingSetName + "_" + originalSubClass;
        }

        // Create a new OtherSubClass
        OtherSubClass otherSubClass;
        // Set the otherSubClass field to the modifiedSubClass
        otherSubClass.otherSubClass = modifiedSubClass;

        for (const auto& trade : groupTrades) {
            if (trade.getSubClass() == originalSubClass) {
                // Create a new TradeData
                TradeData tradeData;
                // Set the trade field to the trade
                tradeData.trades = trade;

                // Add tradeData to the otherSubClass
                otherSubClass.tradeData.push_back(tradeData);
            }
        }

        // Add the otherSubClass to the otherExposureData
        otherExposureData.otherSubClasses.push_back(otherSubClass);
    }
}

std::vector<std::string> GroupOtherTrades::getUniqueSubClasses(const std::vector<SaccrvTrades>& trades) {
    std::set<std::string> uniqueSubClasses;
    for (const auto& trade : trades) {
        uniqueSubClasses.insert(trade.getSubClass());
    }
    return std::vector<std::string>(uniqueSubClasses.begin(), uniqueSubClasses.end());
}

} // namespace analytics
} // namespace ore