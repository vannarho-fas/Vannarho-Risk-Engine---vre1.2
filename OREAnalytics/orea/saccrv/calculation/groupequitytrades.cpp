/*! 
    \file /OREAnalytics/orea/saccrv/calculation/groupequitytrades.cpp
    \brief SACCRV impl file to define the interface for grouping trades by hedging sets and equity types.
    \ingroup saccrv
*/

#include <orea/saccrv/calculation/groupequitytrades.hpp>
#include <orea/saccrv/saccrvcalculationmanager.hpp>
#include <string>
#include <set>

namespace ore {
namespace analytics {

GroupEquityTrades::GroupEquityTrades() {
    // Constructor implementation, if needed
}

void GroupEquityTrades::groupEquityTrades(const std::vector<SaccrvTrades>& groupTrades, const std::string& nettingSetId, const std::string& counterpartyId,const std::string& hedgingSetName) {


    // Get the singleton instance of SaccrvCalculationManager
    LOG("SACCRV: groupEquityTrades called with nettingSetId: " << nettingSetId << " and hedgingSetName: " << hedgingSetName);
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

    auto& equityData = nettingSet->assetClasses.equity;

    // Group trades by reference entities (Issuer and Underlying Instrument)
    std::map<std::string, std::vector<SaccrvTrades>> groupedTrades = groupTradesByRefEntities(groupTrades);

    // Create nodes for each reference entity
    for (const auto& pair : groupedTrades) {
        const std::string& refEntity = pair.first;
        const std::vector<SaccrvTrades>& tradesForEntity = pair.second;

        // Create a new ReferenceEntity
        ReferenceEntity referenceEntity;

        // Construct the hedging set full name
        std::string hedgingSetFullName = hedgingSetName.empty() ? refEntity : constructHedgingSetName(hedgingSetName, refEntity);

        // Set the referenceEntity field to the hedgingSetFullName
        referenceEntity.referenceEntity = hedgingSetFullName;

        std::string subclass;

        for (const auto& trade : tradesForEntity) {
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

        // Set the subclass for the ReferenceEntity after confirming it's consistent across all trades
        referenceEntity.subClass = subclass;

        // Add the referenceEntity to the equityData
        equityData.referenceEntities.push_back(referenceEntity);
    }
}

// std::shared_ptr<CounterpartyCreditRiskExposureTree> GroupEquityTrades::groupEquityTrades(
//     std::vector<SaccrvTrades>& groupTrades, 
//     std::shared_ptr<CounterpartyCreditRiskExposureTree> counterpartyCreditRiskExposureTree, 
//     const std::string& hedgingSetName
// ) {
//     // Group trades by reference entities (Issuer and Underlying Instrument)
//     std::map<std::string, std::vector<SaccrvTrades>> groupedTrades = groupTradesByRefEntities(groupTrades);

//     // Create nodes for each reference entity
//     createEquityNodes(groupedTrades, counterpartyCreditRiskExposureTree);

//     return counterpartyCreditRiskExposureTree;
// }

std::map<std::string, std::vector<SaccrvTrades>> GroupEquityTrades::groupTradesByRefEntities(
    const std::vector<SaccrvTrades>& trades
) {
    std::map<std::string, std::vector<SaccrvTrades>> groupedTrades;

    for (const auto& trade : trades) {
        groupedTrades[trade.issuer].push_back(trade);
        groupedTrades[trade.underlyingInstrument].push_back(trade);
    }

    return groupedTrades;
}

std::string GroupEquityTrades::constructHedgingSetName(
    const std::string& baseName,
    const std::string& refEntity
) {
    if(baseName.compare(0, 4, "Vol_") == 0 || baseName.compare(0, 6, "Basis_") == 0) {
        return baseName + "_" + refEntity;
    } else {
        return refEntity;
    }
}


// void GroupEquityTrades::createEquityNodes(
//     const std::map<std::string, std::vector<SaccrvTrades>>& groupedTrades, 
//     std::shared_ptr<CounterpartyCreditRiskExposureTree> counterpartyCreditRiskExposureTree
// ) {
//     for (const auto& pair : groupedTrades) {
//         const std::string& refEntity = pair.first;
//         const std::vector<SaccrvTrades>& tradesForEntity = pair.second;

//         auto refEntityNode = counterpartyCreditRiskExposureTree->getOrCreateChildNode(refEntity);
//         std::string subclass;
//         // Create Trade child nodes under the refEntityNode.
//         for (const auto& trade : tradesForEntity) {
//             TradeData tradeData;
//             tradeData.trade = trade;
//             auto newChildNode = std::make_shared<CounterpartyCreditRiskExposureTree>(tradeData);
//             refEntityNode->addChild(newChildNode);
//             if (!subclass.empty() && subclass != trade.subClass) {
//             // Handle the error - different subclasses for the same reference entity
//                 WLOG("Different subclasses found for the same reference entity: " + refEntity);
//             }
//             subclass = trade.subClass; // Set the subclass for the ReferenceEntity
//         }
//         refEntityNode->subClass = subclass;
//     }
// }

} // namespace analytics
} // namespace ore