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
    \file /OREAnalytics/orea/saccrv/calculation/groupfxtrades.hpp
    \brief Header file for the GroupFXTrades class, which groups FX trades and manages their calculations within the SACCR framework.
    \ingroup saccrv
*/


#include <orea/saccrv/calculation/groupfxtrades.hpp>
#include <orea/saccrv/saccrvcalculationmanager.hpp>
#include <algorithm>

namespace ore {
namespace analytics {

GroupFXTrades::GroupFXTrades() {
    // Constructor implementation, if needed
}

void GroupFXTrades::groupFXTrades(const std::vector<SaccrvTrades>& groupTrades, const std::string& nettingSetId, const std::string& counterpartyId,const std::string& hedgingSetName) {

    // Get the singleton instance of SaccrvCalculationManager
    LOG("SACCRV: groupFXTrades called with nettingSetId: " << nettingSetId << " and hedgingSetName: " << hedgingSetName);
    auto saccrvCalcManager = SaccrvCalculationManager::getInstance();
    NettingSet* nettingSet = saccrvCalcManager->getNettingSet(counterpartyId, nettingSetId);

    if (nettingSet != nullptr) {
        LOG("SACCRV:  nettingset not null ");

    }
    else {
        // Handle the case where the netting set is not found
        WLOG("Error: Netting set not found for counterpartyId: " << counterpartyId << " and nettingSetId: " << nettingSetId);
        throw std::runtime_error("Netting set not found");
        return;
    }

    auto& fxData = nettingSet->assetClasses.fx;

    std::vector<SaccrvTrades> modifiedTrades = groupTrades;

    // Modify each trade using setFXDynamic
    for (auto& trade : modifiedTrades) {
        setFXDynamic(trade);
    }

    // Group trades by currency pair
    std::map<std::string, std::vector<SaccrvTrades>> groupedTrades = groupTradesByCurrencyPair(modifiedTrades, hedgingSetName);

    // Create nodes for each currency pair
    for (const auto& pair : groupedTrades) {
        const std::string& ccyPair = pair.first;
        const std::vector<SaccrvTrades>& tradesForPair = pair.second;

        // Create a new CurrencyPair
        CurrencyPair currencyPair;

        // Construct the hedging set full name
        std::string hedgingSetFullName = hedgingSetName.empty() ? ccyPair : constructHedgingSetName(hedgingSetName, ccyPair);

        // Set the currencyPair field to the hedgingSetFullName
        currencyPair.currencyPair = hedgingSetFullName;

        for (const auto& trade : tradesForPair) {
            // Create a new TradeData
            TradeData tradeData;
            // Set the trade field to the trade
            tradeData.trades = trade;

            // Add tradeData to the currencyPair
            currencyPair.tradeData.push_back(tradeData);
        }

        // Add the currencyPair to the fxData
        fxData.currencyPairs.push_back(currencyPair);
    }
}

std::string GroupFXTrades::constructHedgingSetName(const std::string& baseName, const std::string& ccyPair) {
    if(baseName.compare(0, 4, "Vol_") == 0 || baseName.compare(0, 6, "Basis_") == 0) {
        return baseName + "_" + ccyPair;
    } else {
        return ccyPair;
    }
}


void GroupFXTrades::setFXDynamic(SaccrvTrades& trade) {
    std::vector<std::string> priorityCurrencies = {"EUR", "JPY", "USD", "CZK", "HRK", "HUF", "BAM", "RSD", "RUB"};
    std::string ccyPaying = trade.getCcyPaying();
    std::string ccyReceiving = trade.getCcyReceiving();

    // Determine the priority currency
    bool isReceivingPriority = std::find(priorityCurrencies.begin(), priorityCurrencies.end(), ccyReceiving) != priorityCurrencies.end();
    bool isPayingPriority = std::find(priorityCurrencies.begin(), priorityCurrencies.end(), ccyPaying) != priorityCurrencies.end();

    if (isReceivingPriority || isPayingPriority) {
        // Set currency pair and Buy/Sell based on priority
        if (isReceivingPriority) {
            trade.setCcyPair(ccyReceiving + "/" + ccyPaying);
            trade.setBuySell("Buy");
        } else {
            trade.setCcyPair(ccyPaying + "/" + ccyReceiving);
            trade.setBuySell("Sell");
        }
    } else {
        trade.setCcyPair(ccyReceiving + "/" + ccyPaying);
        trade.setBuySell("Buy");
    }

    // Determine and set notional
    double amountPaying = trade.getAmountPaying().value_or(0.0);
    double amountReceiving = trade.getAmountReceiving().value_or(0.0);
    trade.setNotional(std::max(amountPaying, amountReceiving));

}

std::map<std::string, std::vector<SaccrvTrades>> GroupFXTrades::groupTradesByCurrencyPair(const std::vector<SaccrvTrades>& trades, const std::string& hedgingSetName) {
    std::map<std::string, std::vector<SaccrvTrades>> groupedTrades;

    for (const auto& trade : trades) {
        std::string ccyPair = trade.getCcyPair();
        if (hedgingSetName.substr(0, 4) == "Vol_" || hedgingSetName.substr(0, 6) == "Basis_") {
            ccyPair = hedgingSetName + "_" + ccyPair;
        }
        groupedTrades[ccyPair].push_back(trade);
    }

    return groupedTrades;
}

} // namespace analytics
} // namespace ore