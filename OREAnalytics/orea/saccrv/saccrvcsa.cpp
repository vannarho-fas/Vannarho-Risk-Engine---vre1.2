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

/*! \file /OREAnalytics/orea/saccrv/csaInstance.cpp
    \brief SACCRV extracts details from NettingSetManager and populates the csaInstance struct in preparation for calculation of EAD
    \ingroup saccrv
*/


#include <orea/saccrv/saccrvcsa.hpp>
#include <ored/portfolio/nettingsetmanager.hpp>
#include <ored/portfolio/nettingsetdetails.hpp>
#include <iostream>
#include <ored/portfolio/portfolio.hpp>
#include <ored/portfolio/trade.hpp>
#include <orea/saccrv/tradeparsers/utils.hpp>

namespace ore {
namespace analytics {

// VTODO - add new fields to the nettingset with active CSA including tradeGroups, hasIlliquidCollateral, applyMarginCallDisputes, greaterThan5000Transactions

std::vector<SaccrvCsa> extractCsaData(const boost::shared_ptr<data::NettingSetManager>& nettingSetManager, const boost::shared_ptr<ore::data::Portfolio>& portfolio) {
    SaccrvCsa csaInstance;

    std::vector<SaccrvCsa> csaData;

    if (!nettingSetManager) {
        LOG("SACCRV: csaInstance.hpp - NettingSetManager pointer is null");
        return csaData;
    } 

    //find the unique nettingset keys in portfolio
    std::map<std::string, std::string> nettingSetMap =  portfolio->nettingSetMap();
    removeEmptyNettingSets(nettingSetMap);

    std::vector<std::string> nettingSetIds;
    for(std::map<std::string, std::string>::iterator it = nettingSetMap.begin(); it != nettingSetMap.end(); ++it)
        nettingSetIds.push_back(it->second);
    //unique nettingset keys
    sort(nettingSetIds.begin(), nettingSetIds.end());
    nettingSetIds.erase(unique( nettingSetIds.begin(), nettingSetIds.end() ), nettingSetIds.end());
    // populate if netting-set has an active CSA in place
    for (auto const& id : nettingSetIds) {
        LOG("For netting-set " << id << " CSA flag is " << nettingSetManager->get(id)->activeCsaFlag());
        // if (nettingSetManager->get(id)->activeCsaFlag()) {
        boost::shared_ptr<ore::data::CSA> csaDetails = nettingSetManager->get(id)->csaDetails();
        if (csaDetails) {
            csaInstance.id = id;
            csaInstance.active = nettingSetManager->get(id)->activeCsaFlag();
            csaInstance.thresCpty = csaDetails->thresholdPay();
            csaInstance.thresPO = csaDetails->thresholdRcv();
            // csaInstance.imCpty = csaDetails->initialMarginCounterparty();
            // csaInstance.imPO = csaDetails->initialMarginOwnParty();
            csaInstance.imCpty = 0.0;
            csaInstance.imPO = 0.0;
            csaInstance.mtaCpty = csaDetails->mtaRcv();
            csaInstance.mtaPO = csaDetails->mtaPay();
            csaInstance.mporDays = ore::analytics::periodToDays(csaDetails->marginPeriodOfRisk()); // parse 1m to days for nettingSet->marginPexriodOfRisk();
            csaInstance.remarginFreq = ore::analytics::periodToDays(csaDetails->marginCallFrequency());
            csaInstance.counterparty = id;
            csaInstance.currency = csaDetails->csaCurrency();
            // csaInstance.tradeGroups = "IRD"; // also Commodity, FX, Credit, Equity
            csaInstance.valuesType = "Actual"; // or Percentage
            csaInstance.hasIlliquidCollateralOrOTCNotEasilyReplaced = csaDetails->hasIlliquidCollateralOrOTCNotEasilyReplaced();
            csaInstance.applyMarginCallDisputes = csaDetails->applyMarginCallDisputes();
            csaInstance.greaterThan5000TransactionsNotCentrallyCleared = csaDetails->greaterThan5000TransactionsNotCentrallyCleared();
            
            LOG("SACCRV: saccrvcsa.cpp - CSA details for netting-set \n" << csaInstance );
            csaData.push_back(csaInstance);
        } else {
            csaInstance.id = id;
            csaInstance.active = nettingSetManager->get(id)->activeCsaFlag();
            csaInstance.counterparty = id;
            csaInstance.tradeGroups = "IRD"; // also Commodity, FX, Credit, Equity
            LOG("SACCRV: saccrvcsa.cpp - CSA details for netting-set \n" << csaInstance );
            csaData.push_back(csaInstance);
        }
        // }
    }


    return csaData;
}


void removeEmptyNettingSets(std::map<std::string, std::string>& nettingSetMap) {
    for(auto it = nettingSetMap.begin(); it != nettingSetMap.end(); ) {
        if(it->second.empty()) {
            it = nettingSetMap.erase(it);
        } else {
            ++it;
        }
    }
}



} // namespace analytics
} // namespace ore
