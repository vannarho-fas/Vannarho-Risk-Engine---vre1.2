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
    \file  /OREAnalytics/orea/saccrv/calculation/groupcredittrades.cpp
    \brief SACCRV header declares the credit risk manager class for dealing with credit trades in SA-CCR calculations
    \ingroup saccrv
*/

#include <orea/saccrv/calculation/groupcredittrades.hpp>
#include <algorithm> // for std::find
#include <functional> // for std::function

namespace ore {
namespace analytics {

GroupCreditTrades::GroupCreditTrades() {}

void GroupCreditTrades::groupCreditTrades(const std::vector<SaccrvTrades>& groupTrades, const std::string& nettingSetId, const std::string& counterpartyId,const std::string& hedgingSetName) {

    // Get the singleton instance of SaccrvCalculationManager
    LOG("SACCRV: groupCreditTrades called with nettingSetId: " << nettingSetId << " and hedgingSetName: " << hedgingSetName);
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

    auto& creditData = nettingSet->assetClasses.credit;

    // Extract unique reference entities
    std::vector<std::string> refEntities;
    std::transform(groupTrades.begin(), groupTrades.end(), std::back_inserter(refEntities),
        [](const SaccrvTrades& trade){ return trade.getRefEntity(); });

    // Remove duplicates
    std::sort(refEntities.begin(), refEntities.end());
    refEntities.erase(std::unique(refEntities.begin(), refEntities.end()), refEntities.end());

    for(const auto& refEntity : refEntities) {
        // Create a new ReferenceEntity
        ReferenceEntity referenceEntity;

        // Construct the hedging set full name
        std::string hedgingSetFullName = hedgingSetName.empty() ? refEntity : constructHedgingSetName(hedgingSetName, refEntity);

        // Set the referenceEntity field to the hedgingSetFullName
        referenceEntity.referenceEntity = hedgingSetFullName;

        std::string subclass;

        for (const auto& trade : groupTrades) {
            if (trade.getRefEntity() == refEntity) {
                // Create a new TradeData
                TradeData tradeData;
                // Set the trade field to the trade
                tradeData.trades = trade;

                if (!subclass.empty() && subclass != trade.subClass) {
                // Handle the error - different subclasses for the same reference entity
                    WLOG("Different subclasses found for the same reference entity: " + refEntity);
                }
                subclass = trade.subClass; // Set the subclass for the ReferenceEntity

                // Add tradeData to the referenceEntity
                referenceEntity.tradeData.push_back(tradeData);
            }
        }

        // Set the subclass for the ReferenceEntity after confirming it's consistent across all trades
        referenceEntity.subClass = subclass;

        // Add the referenceEntity to the creditData
        creditData.referenceEntities.push_back(referenceEntity);
    }
}

std::string GroupCreditTrades::constructHedgingSetName(
    const std::string& baseName,
    const std::string& refEntity
) {
    if(baseName.compare(0, 4, "Vol_") == 0 || baseName.compare(0, 6, "Basis_") == 0) {
        return baseName + "_" + refEntity;
    } else {
        return refEntity;
    }
}

} // namespace analytics
} // namespace ore