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

/*! \file /OREAnalytics/orea/saccrv/saccrvcsa.hpp
    \brief SACCRV extracts details from NettingSetManager and populates the SaccrvCsa struct in preparation for calculation of EAD
    \ingroup saccrv
*/

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <boost/shared_ptr.hpp> 
#include <ored/portfolio/portfolio.hpp>
#include <ored/portfolio/nettingsetmanager.hpp>
#include <ored/utilities/log.hpp>


namespace ore {
namespace analytics {

// Struct representing CSA details for SA-CCR
struct SaccrvCsa {
    std::string id;
    bool active;
    double thresCpty;
    double thresPO;
    double imCpty;
    double imPO;
    double mtaCpty;
    double mtaPO;
    int mporDays;
    int remarginFreq;
    std::string counterparty;
    std::string currency;
    std::string tradeGroups;
    std::string valuesType;
    bool hasIlliquidCollateralOrOTCNotEasilyReplaced;
    bool applyMarginCallDisputes;
    bool greaterThan5000TransactionsNotCentrallyCleared;

    // Constructor for convenience
    SaccrvCsa() : id(""), thresCpty(0.0), thresPO(0.0), imCpty(0.0), imPO(0.0),
                  mtaCpty(0.0), mtaPO(0.0), mporDays(0), remarginFreq(0), 
                  counterparty(""), currency(""), tradeGroups(""), valuesType(""), hasIlliquidCollateralOrOTCNotEasilyReplaced(false), applyMarginCallDisputes(false), greaterThan5000TransactionsNotCentrallyCleared(false) {}

    bool isEmpty() const {
        return id.empty() && thresCpty == 0.0 && thresPO == 0.0 && imCpty == 0.0 && imPO == 0.0
            && mtaCpty == 0.0 && mtaPO == 0.0 && mporDays == 0 && remarginFreq == 0
            && counterparty.empty() && currency.empty() && tradeGroups.empty() && valuesType.empty()
            && !hasIlliquidCollateralOrOTCNotEasilyReplaced && !applyMarginCallDisputes && !greaterThan5000TransactionsNotCentrallyCleared;
    }

    friend std::ostream& operator<<(std::ostream& os, const SaccrvCsa& csa);
    const std::string getId() const {
        return id;
    }

    const bool isActive() const {
        return active;
    }
    
    const std::string getCurrency() const {
        return currency;
    }
    const std::string getTradeGroups() const {
        return tradeGroups;
    }
    const std::string getCounterparty() const {
        return counterparty;
    }

    const int getMporDays() const {
        return mporDays;
    }

    int getRemarginFreq() const {
        return remarginFreq;
    }
    
    bool getHasIlliquidCollateral() const {
        return hasIlliquidCollateralOrOTCNotEasilyReplaced;
    }
    bool getApplyMarginCallDisputes() const {
        return applyMarginCallDisputes;
    }

    bool getGreaterThan5000Transactions() const {
        return greaterThan5000TransactionsNotCentrallyCleared;
    }
};

std::vector<SaccrvCsa> extractCsaData(const boost::shared_ptr<data::NettingSetManager>& nettingSetManager, const boost::shared_ptr<ore::data::Portfolio>& portfolio);

void removeEmptyNettingSets(std::map<std::string, std::string>& nettingSetMap);


} // namespace analytics
} // namespace ore
