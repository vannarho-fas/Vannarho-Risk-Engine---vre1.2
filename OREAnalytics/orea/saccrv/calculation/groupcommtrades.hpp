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


/*! \file /OREAnalytics/orea/saccrv/calculation/groupcommtrades.hpp
    \brief SACCRV header to define the interface for grouping trades by hedging sets and commodity types.
    \ingroup saccrv
*/

#pragma once

#include <vector>
#include <string>
#include <memory>
#include <orea/saccrv/saccrvcalculationmanager.hpp>

namespace ore {
namespace analytics {

class GroupCommodityTrades {
public:
    GroupCommodityTrades(); // Constructor
    void groupCommodityTrades(const std::vector<SaccrvTrades>& groupTrades, std::string nettingSetId, const std::string& counterpartyId,const std::string& hedgingSetName = "");

private:
    std::vector<std::string> getUniqueSubClasses(const std::vector<SaccrvTrades>& trades);
    std::vector<std::string> getUniqueCommodityTypes(const std::vector<SaccrvTrades>& trades);
};

} // namespace analytics
} // namespace ore
