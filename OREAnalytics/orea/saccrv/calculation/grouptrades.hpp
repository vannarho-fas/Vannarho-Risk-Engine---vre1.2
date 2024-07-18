
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

/*! \file /oreanalytics/orea/saccrv/calculation/grouptrades.hpp
    \brief Manages the grouping of trades for SA-CCR calculations. Provides functionality to group trades according to given rules and modify the trade classes tree structure.
    \ingroup saccrv
*/

#pragma once

#include <vector>
#include <functional>
#include <string>
#include <memory>
#include <map>
#include <orea/saccrv/saccrvinputmanager.hpp>

namespace ore {
namespace analytics {

class GroupTradesManager {
public:
    // GroupingFunc used to direct the grouping of trades to specific asset class grouping functions
    // using GroupingFuncType = void(*)(const std::vector<SaccrvTrades>&, const std::string&, const std::string&);
    using GroupingFuncType = std::function<void(const std::vector<SaccrvTrades>&, const std::string&, const std::string&, const std::string&)>;

    GroupTradesManager();
    void groupTrades(const std::vector<SaccrvTrades>& group_trades, GroupingFuncType groupingFunc, std::string nettingSetId, std::string counterpartyId);

private:
    std::vector<std::string> determineHedgingSuperSet(const std::vector<SaccrvTrades>& results);
};

} // namespace analytics
} // namespace ore
