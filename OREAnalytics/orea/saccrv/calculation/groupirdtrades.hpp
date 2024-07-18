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
    \file /OREAnalytics/orea/saccrv/calculation/groupirdtrades.hpp
    \brief GroupIRDTrades header declares functions for grouping interest rate derivative trades by currencies and time buckets.
    \ingroup saccrv
*/

#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <orea/saccrv/saccrvinputmanager.hpp>
#include <orea/saccrv/saccrvcalculationmanager.hpp>

namespace ore {
namespace analytics {

/*! The IRDTradeGrouping class handles grouping of interest rate derivative trades by currency and time buckets. */
class GroupIRDTrades {
public:

    GroupIRDTrades();
    void groupIRDTrades(const std::vector<SaccrvTrades>& groupTrades, const std::string& nettingSetId, const std::string& counterpartyId, const std::string& hedgingSetName = "");

private:
    std::vector<std::string> getUniqueCurrencies(const std::vector<SaccrvTrades>& trades);
    std::map<int, std::vector<SaccrvTrades>> groupTradesByTimeBucket(const std::vector<SaccrvTrades>& trades, const std::string& currency);
    int calcTimeBucket(double Ei);
};

} // namespace analytics
} // namespace ore