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
    \file /OREAnalytics/orea/saccrv/calculation/groupothertrades.hpp
    \brief Header file for the GroupOtherTrades functionality for SACCR as part of ORE's architecture.
    \ingroup saccrv
*/

#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <orea/saccrv/saccrvcalculationmanager.hpp>

namespace ore {
namespace analytics {

class GroupOtherTrades {
public:
    GroupOtherTrades();
    // groupOtherTrades groups the given trades by their sub class and adds them to the counterparty credit risk exposure tree
    void groupOtherTrades(const std::vector<SaccrvTrades>& groupTrades, const std::string& nettingSetId,const std::string& counterpartyId, const std::string& hedgingSetName = "");

private:
    // getUniqueSubClasses returns a vector of unique sub classes from the given vector of trades
    std::vector<std::string> getUniqueSubClasses(const std::vector<SaccrvTrades>& trades);
};

} // namespace analytics
} // namespace ore