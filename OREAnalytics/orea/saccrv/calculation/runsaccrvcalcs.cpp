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

/*! \file /OREAnalytics/orea/saccrv/calculation/runsaccrvcalcs.cpp
    \brief SACCR runsaccrvcalcs implementation that orchestrates out the SA-CCR model calculations.
    \ingroup saccrv
*/


#include <algorithm>
#include <stdexcept>
#include <cmath> 

#include <orea/saccrv/calculation/runsaccrvcalcs.hpp>
#include <orea/saccrv/saccrvcalculationmanager.hpp>
#include <orea/saccrv/saccrvinputmanager.hpp>
#include <orea/saccrv/calculation/calcaddon.hpp>
#include <orea/saccrv/calculation/calcead.hpp>
#include <orea/saccrv/calculation/calcpfe.hpp>
#include <orea/saccrv/calculation/calcrc.hpp>
#include <orea/saccrv/calculation/calcmf.hpp>

namespace ore {
namespace analytics {

RunSaccrvCalculations::RunSaccrvCalculations() {
    calculationManager_ = SaccrvCalculationManager::getInstance();
}

void RunSaccrvCalculations::runCalcs() {

    LOG("SACCRV: Run Calculations");
    // Get singleton instances of managers
    auto tradeInputManager = SaccrvTradesManager::getInstance();
    auto collateralManager = SaccrvCollateralManager::getInstance();
    auto configManager = SaccrvConfigManager::getInstance();
    auto csaManager = SaccrvCsaManager::getInstance();

    // Fetch trade, collateral, CSA, and other necessary input data
    auto trades = tradeInputManager->getTradeVector();
    auto collaterals = collateralManager->getCollateralVector();
    // csa are nettingsets with active CSA flag
    // VTODO - add new fields to the nettingset with active CSA including tradeGroups, hasIlliquidCollateral, applyMarginCallDisputes, greaterThan5000Transactions
    auto csas = csaManager->getCsaVector();
    // auto csas = vector<SaccrvCsa>();
    bool simplified = configManager->isSimplified();
    bool OEM = configManager->isOEM();
    bool ignoreMargin = configManager->shouldIgnoreMargin();
    int mporDays = 0;
    int remarginFreq = 0;
  
    std::string nettingsetId = "";
    std::string unmargined_nettingsetId = "";

    // 1. Check if all trades are sold options
    auto trade_vol = tradeInputManager->getTradeVectorSize();
    LOG("SACCRV: runCalcs - Trade size: " << trade_vol);
    if (allTradesAreSoldOptions(trades)) {
        LOG("SACCRV: All trades are sold options, EAD is zero");
        return; // EAD is zero
    }

    // 2. Get unique counterparties from trades
    auto counterparties = getUniqueCounterpartiesFromTrades(trades);
    // Define filteredTrades and allMarginedTrades which collates all margined trades for a counterparty
    std::vector<SaccrvTrades> filteredTrades;
    // std::vector<SaccrvTrades> allMarginedTrades;

    // 3. Loop over each counterparty
    for (const auto& counterpartyId : counterparties) {
        LOG("SACCRV: Looping over counterparties, current counterparty: " << counterpartyId);
        // create counterparty object
        ore::analytics::Counterparty counterparty;
        counterparty.counterpartyId = counterpartyId;
        // counterparty.nettingSets = {nettingsetId};
        calculationManager_->addCounterparty(counterparty);

 // 4. Select trades associated with current counterparty
        auto tradesCpty = filterTradesByCounterparty(trades, counterpartyId);

        // 5. Loop over each nettingset 
        if (!csas.empty()) {
            for (size_t i = 0; i < csas.size(); ++i) {
                auto& csa = csas[i];
                LOG("SACCRV: Looping over CSAs, current CSA: " << csa.getId());
                // Get the id from the csa
                nettingsetId = csa.getId();

                // get mporDays for branching logic and for maturityFactor floor for margined transactions
                mporDays = csa.getMporDays();
                remarginFreq = csa.getRemarginFreq();
                
                // 6. If current counterparty is associated with current CSA 

                if (counterpartyIsAssociatedWithCsa(counterpartyId, csa) && csa.isActive()) {
                    // Select trades associated with CSA - collateralised
                    LOG("SACCRV: Counterparty " << counterpartyId << " has collateralised nettingSet (active CSA): " << nettingsetId);
                    filteredTrades = filterTradesByCsa(tradesCpty, csa);
                    LOG("SACCRV: Selected trades associated with CSA: " << filteredTrades.size());

                    // define context 
                    SaccrvCalculationContext context_csa = createContext(counterpartyId, nettingsetId, filteredTrades, true, csa, simplified, collaterals, OEM, ignoreMargin);
  
                    // 7a. Calculate EAD for margined trades
                    double EAD_margined = calculateEAD(context_csa);
                    
                    // 7b Calculate EAD as if unmargined trades
                    unmargined_nettingsetId = nettingsetId + "_unmargined";
                    SaccrvCalculationContext context_csa_unmargined_scenario = createContext(counterpartyId, unmargined_nettingsetId, filteredTrades, false, csa, simplified, collaterals, OEM, ignoreMargin);
                    double EAD_unmargined = calculateEAD(context_csa_unmargined_scenario);

                    // Use the minimum of the two EADs and update EAD
                    double minEAD = std::min(EAD_margined, EAD_unmargined);
                    calculationManager_->updateEAD(counterpartyId, nettingsetId, minEAD);
                    // delete unmargined_nettingsetId as only needed for comparison purposes and will otherwise be a duplicate/inflate EAD
                    calculationManager_->deleteNettingSet(counterpartyId, unmargined_nettingsetId);
                    LOG("SACCRV: Updated EAD for Counterparty with min of margined and unmarginned trades: " << counterpartyId << " NettingSet: " << nettingsetId << " EAD: " << minEAD);
                    
                } else if (counterpartyIsAssociatedWithCsa(counterpartyId, csa) && !csa.isActive()) {
                    // Select trades associated with a nettingset with an inactive CSA
                    LOG("SACCRV: Counterparty " << counterpartyId << " has uncollateralised nettingSet (no active CSA): " << nettingsetId);
                    filteredTrades = filterTradesByCsa(tradesCpty, csa);
                    LOG("SACCRV: Selected trades associated with inactive CSA: " << filteredTrades.size());

                    // define context 
                    SaccrvCalculationContext context_no_active_csa = createContext(counterpartyId, nettingsetId, filteredTrades, false, SaccrvCsa(), simplified, collaterals, OEM, ignoreMargin);
  
                    // 7a. Calculate EAD for unmargined trades
                    double EAD_no_active_csa = calculateEAD(context_no_active_csa);
                    calculationManager_->updateEAD(counterpartyId, nettingsetId, EAD_no_active_csa);
                    LOG("SACCRV: Updated EAD for Counterparty with uncollateralised nettingSet (unmargined trades, no active CSA): " << counterpartyId << " NettingSet: " << nettingsetId << " EAD: " << EAD_no_active_csa);
                }
            }

        } 
        
        // then process trades with no nettingset, where needed
        std::vector<SaccrvTrades> filteredTrades = filterTradesWithNoNettingSets(tradesCpty);
        if (!filteredTrades.empty()) {
            // Handle the case where a trade has no nettingset
            LOG("SACCRV: Trades found with no nettingset, processing EAD");
            nettingsetId = "no_nettingSetTrades";
            LOG("SACCRV: Selected trades with no nettingset: " << trades.size());
            nettingsetId = "no_csa";

            // create context
            SaccrvCalculationContext context_no_csa = createContext(counterpartyId, nettingsetId, trades, false, SaccrvCsa(),simplified, collaterals, OEM, ignoreMargin);

            // 7. Create netting set tree and calculate add-on, Calculate RC, PFE, and EAD for each set of trades
            double EAD_no_csa = calculateEAD(context_no_csa);
            calculationManager_->updateEAD(counterpartyId, nettingsetId, EAD_no_csa);
            LOG("SACCRV: Updated EAD for trades with no nettingset: " << counterpartyId << " NettingSet: " << nettingsetId << " EAD: " << EAD_no_csa);
        }

        calculationManager_->sumNettingSetValuesToCounterparty();
        calculationManager_->printCounterpartyCreditRiskExposureTree();
        LOG("SACCRV: Summed values to counterparty");
    }
}


double RunSaccrvCalculations::calculateEAD(SaccrvCalculationContext context) {

    calculationManager_->createSaccrvNettingSetTree(context.counterpartyId, context.nettingsetId, context.trades, context.isMargined);
    LOG("SACCRV: Created netting set tree for Counterparty: " << context.counterpartyId << " NettingSet: " << context.nettingsetId);
    
    SaccrvAddonCalculator addonCalculator(context);
    double counterparty_addon = addonCalculator.calculate();
    LOG("SACCRV: Calculated add-on for Counterparty: " << context.counterpartyId << " NettingSet: " << context.nettingsetId << " Add-on: " << counterparty_addon);

    std::vector<double> mtms = getMtmsForNettingSet(context.counterpartyId, context.nettingsetId);
    double V_C, RC, V;
    std::tie(V_C, RC, V) = SaccrvRCCalculation::computeReplacementCost(mtms, &context.csa, context.collaterals, context.simplified, context.ignoreMargin);
    calculationManager_->updateReplacementCost(context.counterpartyId, context.nettingsetId, V_C, RC, V);
    LOG("SACCRV: Updated replacement cost for Counterparty: " << context.counterpartyId << " NettingSet: " << context.nettingsetId);
    
    double PFE = SaccrvExposureCalculator::calcPFE(V_C, counterparty_addon, context.simplified, V);
    LOG("SACCRV: Calculated PFE for Counterparty: " << context.counterpartyId << " NettingSet: " << context.nettingsetId << " PFE: " << PFE);
    calculationManager_->updatePFE(context.counterpartyId, context.nettingsetId, PFE);
    
    double EAD = ore::analytics::calcEAD(RC, PFE);
    LOG("SACCRV: Calculated EAD for Counterparty: " << context.counterpartyId << " NettingSet: " << context.nettingsetId << " EAD: " << EAD);
    calculationManager_->updateEAD(context.counterpartyId, context.nettingsetId, EAD);
    
    return EAD;
}

std::vector<std::string> RunSaccrvCalculations::getCounterpartiesFromTrades(const std::vector<SaccrvTrades>& trades) {
    std::vector<std::string> counterparties;
    for (const auto& trade : trades) {
        counterparties.push_back(trade.getCounterparty());
    }
    LOG("SACCRV: Got counterparties from trades");
    return counterparties;
}



std::vector<SaccrvTrades> RunSaccrvCalculations::filterTradesByCsa(const std::vector<SaccrvTrades>& trades, const SaccrvCsa& csa) {
    std::vector<SaccrvTrades> filteredTrades;
    for (const auto& trade : trades) {
        if (trade.getNettingSet() == csa.getId()) {
            filteredTrades.push_back(trade);
        }
    }
    LOG("SACCRV: Filtered trades by CSA");
    return filteredTrades;
}

std::vector<SaccrvTrades> RunSaccrvCalculations::filterTradesWithNoNettingSets(const std::vector<SaccrvTrades>& trades) {
    std::vector<SaccrvTrades> filteredTrades;
    for (const auto& trade : trades) {
        if (trade.getNettingSet().empty()) {
            filteredTrades.push_back(trade);
        }
    }
    LOG("SACCRV: Filtered trades with empty netting sets");
    return filteredTrades;
}

std::vector<SaccrvCollateral> RunSaccrvCalculations::filterCollateralsByCsa(const std::vector<SaccrvCollateral>& collaterals, const SaccrvCsa& csa) {
    std::vector<SaccrvCollateral> filteredCollaterals;
    for (const auto& collateral : collaterals) {
        if (collateral.getNettingSetID() == csa.getId()) {
            filteredCollaterals.push_back(collateral);
        }
    }
    LOG("SACCRV: Filtered collaterals by CSA");
    return filteredCollaterals;
}

std::vector<double> RunSaccrvCalculations::getMtmsForNettingSet(std::string counterpartyId, std::string nettingsetId) {
    std::vector<double> mtms;

        // Get the netting set data
    auto saccrvCalcManager = SaccrvCalculationManager::getInstance();
    NettingSet* nettingSet = saccrvCalcManager->getNettingSet(counterpartyId, nettingsetId);

    LOG("SACCRV: Got nettingset from calculation manager: nettingsetId: " << nettingsetId << " counterpartyId: " << counterpartyId);


    // Access the asset classes
    const AssetClasses& assetClasses = nettingSet->assetClasses;

    // Handle IRD asset class
    for (const auto& currencyData : assetClasses.ird.currencies) {
        for (const auto& timeBucket : currencyData.timeBuckets) {
            for (const auto& tradeData : timeBucket.tradeData) {
                if (tradeData.trades.mTm.has_value()) {
                    mtms.push_back(tradeData.trades.mTm.value());
                }
            }
        }
    }

    // Handle Commodity asset class
    for (const auto& hedgingSet : assetClasses.commodity.hedgingSets) {
        for (const auto& commodityType : hedgingSet.commodityTypes) {
            for (const auto& tradeData : commodityType.tradeData) {
                if (tradeData.trades.mTm.has_value()) {
                    mtms.push_back(tradeData.trades.mTm.value());
                }
            }
        }
    }

    // Handle Credit asset class
    for (const auto& referenceEntity : assetClasses.credit.referenceEntities) {
        for (const auto& tradeData : referenceEntity.tradeData) {
            if (tradeData.trades.mTm.has_value()) {
                mtms.push_back(tradeData.trades.mTm.value());
            }
        }
    }

    // Handle FX asset class
    for (const auto& currencyPair : assetClasses.fx.currencyPairs) {
        for (const auto& tradeData : currencyPair.tradeData) {
            if (tradeData.trades.mTm.has_value()) {
                mtms.push_back(tradeData.trades.mTm.value());
            }
        }
    }

    // Handle Equity asset class
    for (const auto& referenceEntity : assetClasses.equity.referenceEntities) {
        for (const auto& tradeData : referenceEntity.tradeData) {
            if (tradeData.trades.mTm.has_value()) {
                mtms.push_back(tradeData.trades.mTm.value());
            }
        }
    }

    // Handle OtherExposure asset class
    for (const auto& otherSubClass : assetClasses.otherExposure.otherSubClasses) {
        for (const auto& tradeData : otherSubClass.tradeData) {
            if (tradeData.trades.mTm.has_value()) {
                mtms.push_back(tradeData.trades.mTm.value());
            }
        }
    }

    return mtms;
}


bool RunSaccrvCalculations::allTradesAreSoldOptions(const std::vector<SaccrvTrades>& trades) {
    for (const auto& trade : trades) {
        if (trade.getTradeType() != "Option" || trade.getBuySell() != "SELL") {
            return false;
        }
    }
    return true;
}

std::vector<std::string> RunSaccrvCalculations::getUniqueCounterpartiesFromTrades(const std::vector<SaccrvTrades>& trades) {
    std::set<std::string> uniqueCounterparties;
    for (const auto& trade : trades) {
        uniqueCounterparties.insert(trade.getCounterparty());
    }
    return std::vector<std::string>(uniqueCounterparties.begin(), uniqueCounterparties.end());
}

std::vector<SaccrvTrades> RunSaccrvCalculations::filterTradesByCounterparty(const std::vector<SaccrvTrades>& trades, const std::string& counterpartyId) {
    std::vector<SaccrvTrades> filteredTrades;
    for (const auto& trade : trades) {
        if (trade.getCounterparty() == counterpartyId) {
            filteredTrades.push_back(trade);
        }
    }
    return filteredTrades;
}

std::vector<SaccrvTrades> RunSaccrvCalculations::filterTradesWithNoNettingSet(const std::vector<SaccrvTrades>& trades) {
    std::vector<SaccrvTrades> filteredTrades;
    for (const auto& trade : trades) {
        if (trade.getNettingSet() == "") {
            filteredTrades.push_back(trade);
        }
    }
    LOG("SACCRV: Filtered trades with no netting set");
    return filteredTrades;
}

bool RunSaccrvCalculations::counterpartyIsAssociatedWithCsa(const std::string& counterpartyId, const SaccrvCsa& csa) {
    // Assuming CSA has a method getCounterparty() that returns the counterparty associated with the CSA
    return csa.getCounterparty() == counterpartyId;
}


SaccrvCalculationContext RunSaccrvCalculations::createContext(const std::string& counterpartyId, const std::string& nettingsetId, const std::vector<SaccrvTrades>& trades, bool isMargined, const SaccrvCsa& csa, bool simplified, const std::vector<SaccrvCollateral>& collaterals, bool OEM, bool ignoreMargin) {
    SaccrvCalculationContext context;
    context.isMargined = isMargined;
    context.counterpartyId = counterpartyId;
    context.nettingsetId = nettingsetId;
    context.trades = trades;
    context.collaterals = collaterals;
    context.simplified = simplified;
    context.OEM = OEM;
    context.collaterals = collaterals;
    context.ignoreMargin = ignoreMargin;

    if (!csa.isEmpty() || csa.isActive()) {
        context.csa = csa;
        context.mporDays = csa.getMporDays();
        context.hasIlliquidCollateralOrOTCNotEasilyReplaced = csa.getHasIlliquidCollateral();
        context.applyMarginCallDisputes = csa.getApplyMarginCallDisputes();
        context.greaterThan5000TransactionsNotCentrallyCleared = csa.getGreaterThan5000Transactions();
        context.remarginFreq = csa.getRemarginFreq();
    }

    return context;
}


} // namespace analytics
} // namespace ore


