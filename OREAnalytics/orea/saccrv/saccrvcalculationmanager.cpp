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

/*! \file /OREAnalytics/orea/saccrv/saccrvcalculationmanager.cpp
    \brief SACCRV implementation manage the calculation results in a tree-like structure. This manager will provide methods for populating and retrieving data efficiently
    \ingroup saccrv
*/

#include <algorithm>

#include <orea/saccrv/saccrvcalculationmanager.hpp>
#include <orea/saccrv/calculation/runsaccrvcalcs.hpp>
#include <orea/saccrv/calculation/groupothertrades.hpp>
#include <orea/saccrv/calculation/groupirdtrades.hpp>
#include <orea/saccrv/calculation/groupfxtrades.hpp>
#include <orea/saccrv/calculation/groupcredittrades.hpp>
#include <orea/saccrv/calculation/groupcommtrades.hpp>
#include <orea/saccrv/calculation/grouptrades.hpp>


namespace ore {
namespace analytics {

SaccrvCalculationManager* SaccrvCalculationManager::instance_ = nullptr;

std::mutex SaccrvCalculationManager::mutex_;

SaccrvCalculationManager* SaccrvCalculationManager::getInstance() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (instance_ == nullptr) {
        instance_ = new SaccrvCalculationManager();
    }
    return instance_;
}

void SaccrvCalculationManager::initialiseCounterpartyTree(const std::vector<Counterparty>& counterparties) {
    // Clear the existing data
    data_->counterparties.clear();

    // Populate the CounterpartyCreditRiskExposureTree with the input counterparties
    for (const auto& counterparty : counterparties) {
        data_->counterparties.push_back(counterparty);
    }
}


void SaccrvCalculationManager::createSaccrvNettingSetTree(const std::string& counterpartyId, const std::string& nettingSetId, const std::vector<SaccrvTrades>& trades, const bool isMargined) {
    LOG("SACCRV: Entering createSaccrvNettingSetTree");
    if (trades.empty()) {
        LOG("SACCRV: No trades provided for creating a saccrvData.");
        throw std::runtime_error("No trades provided for creating a saccrvData.");
    }
    LOG("SACCRV: Creating netting set tree for counterparty: " << counterpartyId << " netting set: " << nettingSetId);

    // Find the counterparty
    auto counterpartyIt = std::find_if(data_->counterparties.begin(), data_->counterparties.end(),
                                       [&counterpartyId](const Counterparty& c) { return c.counterpartyId == counterpartyId; });
    if (counterpartyIt == data_->counterparties.end()) {
        LOG("SACCRV: Counterparty not found.");
        throw std::runtime_error("Counterparty not found.");
    }


    // Create a new netting set
    NettingSet nettingSet;
    nettingSet.id = nettingSetId;
    nettingSet.counterpartyId = counterpartyId;
    nettingSet.isMargined = isMargined;

    // // add the netting set to the counterparty
    counterpartyIt->nettingSets.push_back(nettingSet);

    GroupTradesManager groupTradesManager;
    LOG("SACCRV: GroupTradesManager created");

    // Create a map to hold trades grouped by asset class
    std::map<std::string, std::vector<SaccrvTrades>> tradesByAssetClass;

    // Populate the map
    for (const auto& trade : trades) {
        tradesByAssetClass[trade.getTradeGroup()].push_back(trade);
    }

    // Now iterate over the map and call groupTrades for each asset class
    for (const auto& pair : tradesByAssetClass) {
        const std::string& tradeGroup = pair.first;
        const std::vector<SaccrvTrades>& tradesForGroup = pair.second;

        GroupTradesManager::GroupingFuncType groupingFunc;
        LOG("SACCRV: Grouping trades by their type");

        // This lambda function captures the groupFXTrades instance by reference and calls the groupFXTrades method on it. It can be assigned to groupingFunc because it matches the signature of GroupingFuncType
        if (tradeGroup == "FX") {
            GroupFXTrades groupFXTrades;
            std::vector<SaccrvTrades> tradesCopy = tradesForGroup;
            groupingFunc = [&groupFXTrades](const std::vector<SaccrvTrades>& tradesCopy, const std::string& nettingSetId, const std::string& counterpartyId, const std::string& hedgingSetName) mutable {
                groupFXTrades.groupFXTrades(tradesCopy, nettingSetId, counterpartyId,hedgingSetName);
            };
            LOG("SACCRV: Grouping FX trades");
        } else if (tradeGroup == "IRD") {
            GroupIRDTrades groupIRDTrades;
            std::vector<SaccrvTrades> tradesCopy = tradesForGroup;
            groupingFunc = [&groupIRDTrades](const std::vector<SaccrvTrades>& tradesCopy, const std::string& nettingSetId,  const std::string& counterpartyId,const std::string& hedgingSetName) mutable {
                groupIRDTrades.groupIRDTrades(tradesCopy, nettingSetId, counterpartyId,hedgingSetName);
            };
            LOG("SACCRV: Grouping IRD trades");
        } else if (tradeGroup == "CreditSingle" || tradeGroup == "CreditIndex") {
            GroupCreditTrades groupCreditTrades;
            std::vector<SaccrvTrades> tradesCopy = tradesForGroup;
            groupingFunc = [&groupCreditTrades](const std::vector<SaccrvTrades>& tradesCopy, const std::string& nettingSetId,  const std::string& counterpartyId,const std::string& hedgingSetName) mutable {
                groupCreditTrades.groupCreditTrades(tradesCopy, nettingSetId, counterpartyId,hedgingSetName);
            };
            LOG("SACCRV: Grouping Credit trades");
        } else if (tradeGroup == "Commodity") {
            GroupCommodityTrades groupCommodityTrades;
            std::vector<SaccrvTrades> tradesCopy = tradesForGroup;
            groupingFunc = [&groupCommodityTrades](const std::vector<SaccrvTrades>& tradesCopy, const std::string& nettingSetId, const std::string& counterpartyId,const std::string& hedgingSetName) mutable {
                groupCommodityTrades.groupCommodityTrades(tradesCopy, nettingSetId, counterpartyId, hedgingSetName);
            };
            LOG("SACCRV: Grouping Commodity trades");
        } else {
            GroupOtherTrades groupOtherTrades;
            std::vector<SaccrvTrades> tradesCopy = tradesForGroup;
            groupingFunc = [&groupOtherTrades](const std::vector<SaccrvTrades>& tradesCopy, const std::string& nettingSetId,  const std::string& counterpartyId,const std::string& hedgingSetName) mutable {
                groupOtherTrades.groupOtherTrades(tradesCopy, nettingSetId, counterpartyId, hedgingSetName);
            };
            LOG("SACCRV: Grouping Other trades");
        }

        groupTradesManager.groupTrades({tradesForGroup}, groupingFunc,nettingSetId,counterpartyId);
        LOG("SACCRV: Grouped trades");
    }

}

void SaccrvCalculationManager::printIRD(const IRDData& ird) {
        std::cout << "IRD Addon: " << ird.addon << std::endl;
        // Print details for each currency in the IRD
        for (const auto& currencyData : ird.currencies) {
            std::cout << "Currency: " << currencyData.currency << std::endl;
        }
    }

void SaccrvCalculationManager::printCommodity(const CommodityData& commodity) {
        std::cout << "Commodity Addon: " << commodity.addon << std::endl;
        // Print details for each hedging set in the Commodity
        for (const auto& hedgingSet : commodity.hedgingSets) {
            std::cout << "Hedging Set: " << hedgingSet.hedgingSet << std::endl;
        }
    }

void SaccrvCalculationManager::printCredit(const CreditData& credit) {
        std::cout << "Credit Addon: " << credit.addon << std::endl;
        // Print details for each reference entity in the Credit
        for (const auto& referenceEntity : credit.referenceEntities) {
            std::cout << "Reference Entity: " << referenceEntity.referenceEntity << std::endl;
        }
    }

void SaccrvCalculationManager::printFX(const FXData& fx) {
        std::cout << "FX Addon: " << fx.addon << std::endl;
        // Print details for each currency pair in the FX
        for (const auto& currencyPair : fx.currencyPairs) {
            std::cout << "Currency Pair: " << currencyPair.currencyPair << std::endl;
        }
    }

void SaccrvCalculationManager::printEquity(const EquityData& equity) {
        std::cout << "Equity Addon: " << equity.addon << std::endl;
        // Print details for each reference entity in the Equity
        for (const auto& referenceEntity : equity.referenceEntities) {
            std::cout << "Reference Entity: " << referenceEntity.referenceEntity << std::endl;
        }
    }

void SaccrvCalculationManager::printOtherExposure(const OtherExposureData& otherExposure) {
        std::cout << "Other Exposure Addon: " << otherExposure.addon << std::endl;
        // Print details for each other subclass in the Other Exposure
        for (const auto& otherSubClass : otherExposure.otherSubClasses) {
            std::cout << "Other SubClass: " << otherSubClass.otherSubClass << std::endl;
        }
    }

void SaccrvCalculationManager::printSaccrvData() {
        for (const auto& counterparty : data_->counterparties) {
            std::cout << "Counterparty ID: " << counterparty.counterpartyId << std::endl;

            // Print details for each netting set
            for (const auto& nettingSet : counterparty.nettingSets) {
                std::cout << "Netting Set ID: " << nettingSet.id << std::endl;

                // Print details for each asset class
                printIRD(nettingSet.assetClasses.ird);
                printCommodity(nettingSet.assetClasses.commodity);
                printCredit(nettingSet.assetClasses.credit);
                printFX(nettingSet.assetClasses.fx);
                printEquity(nettingSet.assetClasses.equity);
                printOtherExposure(nettingSet.assetClasses.otherExposure);
            }
        }
}

void SaccrvCalculationManager::updateReplacementCost(const std::string& counterpartyId, const std::string& nettingSetId, double V_C, double RC, double V) {

    auto counterparty = getCounterparty(counterpartyId);
    if (!counterparty) {
        WLOG("Invalid counterparty ID provided for Replacement Cost update.");
        return;
    }

    // Find the netting set object
    auto nettingSet = std::find_if(counterparty->nettingSets.begin(), counterparty->nettingSets.end(),
                                   [&nettingSetId](const NettingSet& ns) { return ns.id == nettingSetId; });

    if (nettingSet == counterparty->nettingSets.end()) {
        WLOG("Netting set not found in the counterparty data.");
        return;
    }

    // Update the Replacement Cost in the netting set data
    nettingSet->replacementCost.v_c = V_C;
    nettingSet->replacementCost.rc = RC;
    nettingSet->replacementCost.v = V;
}

void SaccrvCalculationManager::updatePFE(const std::string& counterpartyId, const std::string& nettingSetId, double PFE) {

    auto counterparty = getCounterparty(counterpartyId);
    if (!counterparty) {
        WLOG("Invalid counterparty ID provided for PFE update.");
        return;
    }

    // Find the netting set object
    auto nettingSet = std::find_if(counterparty->nettingSets.begin(), counterparty->nettingSets.end(),
                                   [&nettingSetId](const NettingSet& ns) { return ns.id == nettingSetId; });

    if (nettingSet == counterparty->nettingSets.end()) {
        WLOG("Netting set not found in the counterparty data.");
        return;
    }

    // Update the PFE in the netting set data
    nettingSet->pfe = PFE;
}

void SaccrvCalculationManager::updateEAD(const std::string& counterpartyId, const std::string& nettingsetId, double EAD) {

    NettingSet* nettingSet = getNettingSet(counterpartyId, nettingsetId);

    if (nettingSet == nullptr) {
        WLOG("SACCRV: Nullptr - Netting set not found in the counterparty data." << counterpartyId << " " << nettingsetId);
        return;
    }

    // Update the EAD in the netting set data
    nettingSet->ead = EAD;
}



void SaccrvCalculationManager::sumNettingSetValuesToCounterparty() {
    for (auto& counterparty : data_->counterparties) {
        for (const auto& nettingSet : counterparty.nettingSets) {
            counterparty.addOn += nettingSet.addOn;
            counterparty.ead += nettingSet.ead;
            counterparty.pfe += nettingSet.pfe;
            counterparty.replacementCost.v_c += nettingSet.replacementCost.v_c;
            counterparty.replacementCost.rc += nettingSet.replacementCost.rc;
            counterparty.replacementCost.v += nettingSet.replacementCost.v;
        }
    }
}

int SaccrvCalculationManager::calculateTimeBucketForTrade(const TradeData& tradeData) {
    // Example logic for calculating time bucket based on trade maturity
    double maturity = tradeData.exposureDetails.maturityFactor; // Assuming maturityFactor is a field in exposureDetails

    if (maturity <= 1) {
        return 1; // Bucket 1 for trades with maturity <= 1 year
    } else if (maturity <= 5) {
        return 2; // Bucket 2 for trades with maturity > 1 year and <= 5 years
    } else {
        return 3; // Bucket 3 for trades with maturity > 5 years
    }
}

Counterparty* SaccrvCalculationManager::getCounterparty(const std::string& counterpartyId) {
    // Iterate over the list of counterparties
    for (auto& counterparty : data_->counterparties) {
        // If the ID of the current counterparty matches the input ID, return a pointer to this counterparty
        if (counterparty.counterpartyId == counterpartyId) {
            return &counterparty;
        }
    }
    // If no counterparty with the input ID is found, return nullptr
    return nullptr;
}

void SaccrvCalculationManager::addCounterparty(Counterparty& counterparty) {
    data_->counterparties.push_back(counterparty);
}

NettingSet* SaccrvCalculationManager::getNettingSet(const std::string& counterpartyId, const std::string& nettingsetId) {
    // Get the counterparty
    Counterparty* counterparty = getCounterparty(counterpartyId);
    if (counterparty == nullptr) {
        WLOG("getNettingSet - Counterparty not found:" << counterpartyId);
        return nullptr; // Counterparty not found
    }

    // Search for the netting set within the counterparty's netting sets
    for (NettingSet& nettingSet : counterparty->nettingSets) {
        if (nettingSet.id == nettingsetId) {
            LOG("getNettingSet - Nettingset found:" << nettingsetId);
            return &nettingSet; // Netting set found
        }
    }
    LOG("getNettingSet - Nettingset NOT found:" << nettingsetId);
    return nullptr; //
}
void SaccrvCalculationManager::deleteNettingSet(const std::string& counterpartyId, const std::string& nettingsetId) {
    // Get the counterparty
    Counterparty* counterparty = getCounterparty(counterpartyId);
    if (counterparty == nullptr) {
        WLOG("deleteNettingSet - Counterparty not found:" << counterpartyId);
        return; // Counterparty not found
    }

    // Find the netting set within the counterparty's netting sets
    auto it = std::find_if(counterparty->nettingSets.begin(), counterparty->nettingSets.end(),
                           [&nettingsetId](const NettingSet& ns) { return ns.id == nettingsetId; });

    if (it != counterparty->nettingSets.end()) {
        // If the netting set is found, erase it
        counterparty->nettingSets.erase(it);
        LOG("deleteNettingSet - Nettingset deleted:" << nettingsetId);
    } else {
        LOG("deleteNettingSet - Nettingset NOT found:" << nettingsetId);
    }
}


double SaccrvCalculationManager::getEAD(const std::string& counterpartyId) {
    auto counterparty = getCounterparty(counterpartyId);
    if (!counterparty) {
        WLOG("Invalid counterparty ID provided for EAD update.");
        return 0.0;
    }
    return counterparty->ead;
}

void SaccrvCalculationManager::printIRDTree(const IRDData& ird) {
    LOG("    IRD Asset Class:");
    LOG("      IRD Addon: " << ird.addon);
    for (const auto& currencyData : ird.currencies) {
        LOG("        Currency: " << currencyData.currency);
        LOG("        Currency Addon: " << currencyData.addon);
        LOG("        Currency Supervisory Factor: " << currencyData.supervisoryFactor);
        for (const auto& timeBucket : currencyData.timeBuckets) {
            LOG("          Time Bucket: " << timeBucket.timeBucketNum);
            LOG("          Effective Notional: " << timeBucket.effectiveNotional);
            for (const auto& tradeData : timeBucket.tradeData) {
                LOG("            Trade ID: " << tradeData.id);
                LOG("              Adjusted Notional: " << tradeData.exposureDetails.adjNotional);
                LOG("              Maturity Factor: " << tradeData.exposureDetails.maturityFactor);
                LOG("              Supervisory Delta: " << tradeData.exposureDetails.supervDelta);
                LOG("              Effective Notional: " << tradeData.exposureDetails.effectiveNotional);
                if (tradeData.trades.notional.has_value()) {
                    LOG("              Trade Notional: " << tradeData.trades.notional.value());
                } else {
                    LOG("              Trade Notional: N/A");
                }
                LOG("              Trade Type: " << tradeData.trades.tradeType);
            }
        }
    }
}

// void SaccrvCalculationManager::printFXTree(const FXData& fx) {
//     LOG("    FX Asset Class:");
//     LOG("      FX Addon: " << fx.addon);
//     for (const auto& currencyPair : fx.currencyPairs) {
//         LOG("        Currency Pair: " << currencyPair.currencyPair);
//         LOG("        Currency Pair Addon: " << currencyPair.addon);
//         LOG("        Currency Pair Supervisory Factor: " << currencyPair.supervisoryFactor);
//         for (const auto& tradeData : currencyPair.tradeData) {
//             LOG("            Trade ID: " << tradeData.id);
//             LOG("              Adjusted Notional: " << tradeData.exposureDetails.adjNotional);
//                        LOG("              Maturity Factor: " << tradeData.exposureDetails.maturityFactor);
//                        LOG("              Supervisory Delta: " << tradeData.exposureDetails.supervDelta);
//                        LOG("              Effective Notional: " << tradeData.exposureDetails.effectiveNotional);
//             LOG("              Trade Notional: " << tradeData.trades.notional);
//             LOG("              Trade Type: " << tradeData.trades.tradeType);
//         }
//     }
// }

// void SaccrvCalculationManager::printCreditTree(const CreditData& credit) {
//     LOG("    Credit Asset Class:");
//     LOG("      Credit Addon: " << credit.addon);
//     LOG("      Idiosyncratic Component: " << credit.idiosynchraticComponent);
//     LOG("      Systematic Component: " << credit.systematicComponent);
//     for (const auto& referenceEntity : credit.referenceEntities) {
//         LOG("        Reference Entity: " << referenceEntity.referenceEntity);
//         LOG("        Reference Entity Addon: " << referenceEntity.addon);
//         LOG("        Reference Entity Supervisory Factor: " << referenceEntity.supervisoryFactor);
//         LOG("        Reference Entity Supervisory Correlation: " << referenceEntity.supervisoryCorel);
//         for (const auto& tradeData : referenceEntity.tradeData) {
//             LOG("            Trade ID: " << tradeData.id);
//             LOG("              Adjusted Notional: " << tradeData.exposureDetails.adjNotional);
//             LOG("              Maturity Factor: " << tradeData.exposureDetails.maturityFactor);
//             LOG("              Supervisory Delta: " << tradeData.exposureDetails.supervDelta);
//             LOG("              Effective Notional: " << tradeData.exposureDetails.effectiveNotional);
//             LOG("              Trade Notional: " << tradeData.trades.notional);
//             LOG("              Trade Type: " << tradeData.trades.tradeType);
//         }
//     }
// }

// void SaccrvCalculationManager::printCommodityTree(const CommodityData& commodity) {
//     LOG("    Commodity Asset Class:");
//     LOG("      Commodity Addon: " << commodity.addon);
//     for (const auto& hedgingSet : commodity.hedgingSets) {
//         LOG("        Hedging Set: " << hedgingSet.hedgingSet);
//         LOG("        Hedging Set Addon: " << hedgingSet.addon);
//         LOG("        Hedging Set Supervisory Correlation: " << hedgingSet.supervisoryCorel);
//         for (const auto& commodityType : hedgingSet.commodityTypes) {
//             LOG("          Commodity Type: " << commodityType.commodity);
//             LOG("          Commodity Type Addon: " << commodityType.addon);
//             LOG("          Commodity Type Supervisory Factor: " << commodityType.supervisoryFactor);
//             for (const auto& tradeData : commodityType.tradeData) {
//                 LOG("            Trade ID: " << tradeData.id);
//                 LOG("              Adjusted Notional: " << tradeData.exposureDetails.adjNotional);
//                 LOG("              Maturity Factor: " << tradeData.exposureDetails.maturityFactor);
//                 LOG("              Supervisory Delta: " << tradeData.exposureDetails.supervDelta);
//                 LOG("              Effective Notional: " << tradeData.exposureDetails.effectiveNotional);
//                 LOG("              Trade Notional: " << tradeData.trades.notional);
//                 LOG("              Trade Type: " << tradeData.trades.tradeType);
//             }
//         }
//     }
// }

// void SaccrvCalculationManager::printEquityTree(const EquityData& equity) {
//     LOG("    Equity Asset Class:");
//     LOG("      Equity Addon: " << equity.addon);
//     LOG("      Idiosyncratic Component: " << equity.idiosynchraticComponent);
//     LOG("      Systematic Component: " << equity.systematicComponent);
//     for (const auto& referenceEntity : equity.referenceEntities) {
//         LOG("        Reference Entity: " << referenceEntity.referenceEntity);
//         LOG("        Reference Entity Addon: " << referenceEntity.addon);
//         LOG("        Reference Entity Supervisory Factor: " << referenceEntity.supervisoryFactor);
//         LOG("        Reference Entity Supervisory Correlation: " << referenceEntity.supervisoryCorel);
//         for (const auto& tradeData : referenceEntity.tradeData) {
//             LOG("            Trade ID: " << tradeData.id);
//             LOG("              Adjusted Notional: " << tradeData.exposureDetails.adjNotional);
//             LOG("              Maturity Factor: " << tradeData.exposureDetails.maturityFactor);
//             LOG("              Supervisory Delta: " << tradeData.exposureDetails.supervDelta);
//             LOG("              Effective Notional: " << tradeData.exposureDetails.effectiveNotional);
//             LOG("              Trade Notional: " << tradeData.trades.notional);
//             LOG("              Trade Type: " << tradeData.trades.tradeType);
//         }
//     }
// }

// void SaccrvCalculationManager::printOtherExposureTree(const OtherExposureData& otherExposure) {
//     LOG("    Other Exposure Asset Class:");
//     LOG("      Other Exposure Addon: " << otherExposure.addon);
//     for (const auto& otherSubClass : otherExposure.otherSubClasses) {
//         LOG("        Other Sub Class: " << otherSubClass.otherSubClass);
//         LOG("        Other Sub Class Addon: " << otherSubClass.addon);
//         LOG("        Other Sub Class Supervisory Factor: " << otherSubClass.supervisoryFactor);
//         for (const auto& tradeData : otherSubClass.tradeData) {
//             LOG("            Trade ID: " << tradeData.id);
//             LOG("              Adjusted Notional: " << tradeData.exposureDetails.adjNotional);
//             LOG("              Maturity Factor: " << tradeData.exposureDetails.maturityFactor);
//             LOG("              Supervisory Delta: " << tradeData.exposureDetails.supervDelta);
//             LOG("              Effective Notional: " << tradeData.exposureDetails.effectiveNotional);
//             LOG("              Trade Notional: " << tradeData.trades.notional);
//             LOG("              Trade Type: " << tradeData.trades.tradeType);
//         }
//     }
// }

void SaccrvCalculationManager::printCounterpartyCreditRiskExposureTree() {
    LOG("SACCRV: Printing Counterparty Credit Risk Exposure Tree");
    for (const auto& counterparty : data_->counterparties) {
        LOG("Counterparty ID: " << counterparty.counterpartyId);
        LOG("  AddOn: " << counterparty.addOn);
        LOG("  EAD: " << counterparty.ead);
        LOG("  PFE: " << counterparty.pfe);
        LOG("  Replacement Cost V_C: " << counterparty.replacementCost.v_c);
        LOG("  Replacement Cost RC: " << counterparty.replacementCost.rc);
        LOG("  Replacement Cost V: " << counterparty.replacementCost.v);
        for (const auto& nettingSet : counterparty.nettingSets) {
            LOG("  Netting Set ID: " << nettingSet.id);
            LOG("    Is Margined: " << (nettingSet.isMargined ? "Yes" : "No"));
            LOG("    AddOn: " << nettingSet.addOn);
            LOG("    EAD: " << nettingSet.ead);
            LOG("    PFE: " << nettingSet.pfe);
            LOG("    Replacement Cost V_C: " << nettingSet.replacementCost.v_c);
            LOG("    Replacement Cost RC: " << nettingSet.replacementCost.rc);
            LOG("    Replacement Cost V: " << nettingSet.replacementCost.v);
            printIRDTree(nettingSet.assetClasses.ird);
        }
    }
}


} // namespace analytics
} // namespace ore
