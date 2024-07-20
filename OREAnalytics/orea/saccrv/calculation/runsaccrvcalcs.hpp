
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

/*! \file /OREAnalytics/orea/saccrv/calculation/runsaccrvcalcs.hpp
    \brief SACCR runsaccrvcalcs header file that defines the RunSaccrvCalculations class.
    \ingroup saccrv
*/

#pragma once

#include <orea/saccrv/saccrvinputmanager.hpp>
#include <orea/saccrv/saccrvcalculationmanager.hpp>
#include <memory>
#include <boost/config.hpp>
#include <orea/saccrv/saccrvcalculationmanager.hpp>

namespace ore {
namespace analytics {

class RunSaccrvCalculations {

public:
    RunSaccrvCalculations();
    void runCalcs();
    double calculateEAD(SaccrvCalculationContext context);
    std::vector<std::string> getUniqueCounterpartiesFromTrades(const std::vector<SaccrvTrades>& trades);
    std::vector<SaccrvTrades> filterTradesByCounterparty(const std::vector<SaccrvTrades>& trades, const std::string& counterpartyId);
    std::vector<SaccrvTrades> filterTradesWithNoNettingSet(const std::vector<SaccrvTrades>& trades);
    bool counterpartyIsAssociatedWithCsa(const std::string& counterpartyId, const SaccrvCsa& csa);
    bool allTradesAreSoldOptions(const std::vector<SaccrvTrades>& trades);
    SaccrvCalculationContext createContext(const std::string& counterpartyId, const std::string& nettingsetId, const std::vector<SaccrvTrades>& trades, bool isMargined, const SaccrvCsa& csa, bool simplified, const std::vector<SaccrvCollateral>& collaterals, bool OEM, bool ignoreMargin);

private:
    SaccrvCalculationManager* calculationManager_; 
    std::vector<double> getMtmsForNettingSet(std::string counterpartyId, std::string nettingsetId);
    std::vector<SaccrvCollateral> filterCollateralsByCsa(const std::vector<SaccrvCollateral>& collaterals, const SaccrvCsa& csa);
    std::vector<SaccrvTrades> filterTradesByCsa(const std::vector<SaccrvTrades>& trades, const SaccrvCsa& csa);
    std::vector<SaccrvTrades> filterTradesWithNoNettingSets(const std::vector<SaccrvTrades>& trades);
    std::vector<std::string> getCounterpartiesFromTrades(const std::vector<SaccrvTrades>& trades);
    // Additional private helper functions as needed
};

} // namespace analytics
} // namespace ore

