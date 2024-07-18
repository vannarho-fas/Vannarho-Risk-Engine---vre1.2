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


/*! \file /oreanalytics/orea/saccrv/calculation/groupttrades.cpp
    \brief Manages the grouping of trades for SA-CCR calculations. Provides functionality to group trades according to given rules and modify the trade classes tree structure.
    \ingroup saccrv
*/

#include <orea/saccrv/calculation/grouptrades.hpp>
#include <orea/saccrv/saccrvcalculationmanager.hpp>
#include <orea/saccrv/calculation/handlebasisvol.hpp>
#include <algorithm>

namespace ore {
namespace analytics {

GroupTradesManager::GroupTradesManager() {
    // Constructor implementation, if necessary
    LOG("SACCRV: GroupTradesManager constructor called");
}

void GroupTradesManager::groupTrades(const std::vector<SaccrvTrades>& group_trades, GroupingFuncType groupingFunc, std::string nettingSetId, std::string counterpartyId) {
    try {
        LOG("SACCRV: GroupTradesManager::groupTrades called");
        // auto saccrvCalcManager = SaccrvCalculationManager::getInstance();
        auto tradeCategorizer = std::make_unique<TradeCategorizer>();

        std::vector<std::shared_ptr<SaccrvTrades>> group_trades_ptrs;
        for (auto& trade : group_trades) {
            group_trades_ptrs.push_back(std::make_shared<SaccrvTrades>(trade));
        }
        CategorizedTrades results = tradeCategorizer->handleBasisVol(group_trades_ptrs);

        std::vector<SaccrvTrades> trades_for_ids;
        for (const auto& id : results.tradeIdsAll) {
            auto it = std::find_if(group_trades.begin(), group_trades.end(), [&id](const SaccrvTrades& trade) {
                return trade.getId() == id;
            });
            if (it != group_trades.end()) {
                trades_for_ids.push_back(*it);
            }
        }

        std::vector<std::string> hedging_super_set = determineHedgingSuperSet(trades_for_ids); 

        LOG("SACCRV: GroupTradesManager::groupTrades iterating through hedging_super_set: " );
        for (const auto& hedging_set : hedging_super_set) {
            std::vector<SaccrvTrades> temp_trades;

            if (hedging_set == "normal_trades") {
                std::copy_if(group_trades.begin(), group_trades.end(), std::back_inserter(temp_trades), [&results](const SaccrvTrades& trade) {
                    return std::find(results.tradeIdsAll.begin(), results.tradeIdsAll.end(), trade.getId()) == results.tradeIdsAll.end();
                    LOG("SACCRV: GroupTradesManager::groupTrades normal_trades trade id: " << trade.getId());
                });
            }
            else {
                std::copy_if(group_trades.begin(), group_trades.end(), std::back_inserter(temp_trades), [&results, &hedging_set](const SaccrvTrades& trade) {
                    // Calculate the hedging set based on the trade type
                    std::string trade_hedging_set;
                    if (trade.tradeType.rfind("Vol_", 0) == 0) {
                        trade_hedging_set = "Vol_";
                    } else if (trade.tradeType.rfind("Basis_", 0) == 0) {
                        trade_hedging_set = "Basis_";
                    } else {
                        trade_hedging_set = "normal_trades";
                    }

                    return std::find(results.tradeIdsAll.begin(), results.tradeIdsAll.end(), trade.getId()) != results.tradeIdsAll.end() && trade_hedging_set == hedging_set;
                    LOG("SACCRV: GroupTradesManager::groupTrades hedging_set trade id: " << trade.getId());
                });
            } 

            if (!temp_trades.empty()) {
                LOG("SACCRV: GroupTradesManager::groupTrades calling groupingFunc with temp_trades, nettingSetId, and hedging_set: " << temp_trades.size() << ", " << nettingSetId << ", " << hedging_set);
                groupingFunc(temp_trades, nettingSetId, counterpartyId, hedging_set);
            }
        }
    } catch (const std::exception& e) {
        ALOG("SACCRV: GroupTradesManager::groupTrades encountered an error: " << e.what());
    }
}


std::vector<std::string> GroupTradesManager::determineHedgingSuperSet(const std::vector<SaccrvTrades>& results) {
    try {
        LOG("SACCRV: GroupTradesManager::determineHedgingSuperSet called");
        // Start with a basic set containing "normal_trades"
        std::vector<std::string> hedging_super_set = {"normal_trades"};

        // Check if any "Vol_" hedging sets exist in the results
        if (std::any_of(results.begin(), results.end(), [](const SaccrvTrades& trade) {
            return trade.tradeType.rfind("Vol_", 0) == 0; // Replace HedgingSet with tradeType
        })) {
            // Add "Vol_" to the superset if any "Vol_" trades are present
            hedging_super_set.emplace_back("Vol_");
        }

        // Check if any basis hedging sets exist in the results based on their prefix
        if (std::any_of(results.begin(), results.end(), [](const SaccrvTrades& trade) {
            return trade.tradeType.rfind("Basis_", 0) == 0; // Replace HedgingSet with tradeType
        })) {
            // Add identified basis hedging sets to the superset
            std::set<std::string> unique_basis_sets;
            for (const auto& trade : results) {
                if (trade.tradeType.rfind("Basis_", 0) == 0) { // Replace HedgingSet with tradeType
                    unique_basis_sets.insert(trade.tradeType); // Replace HedgingSet with tradeType
                }
            }
            hedging_super_set.insert(hedging_super_set.end(), unique_basis_sets.begin(), unique_basis_sets.end());
        }

        LOG("SACCRV: GroupTradesManager::determineHedgingSuperSet returning hedging_super_set" );
        return hedging_super_set;
    } catch (const std::exception& e) {
        ALOG("SACCRV: GroupTradesManager::determineHedgingSuperSet encountered an error: " << e.what());
        return std::vector<std::string>();
    }
}

} // namespace analytics
} // namespace ore
