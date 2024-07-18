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
    \file /OREAnalytics/orea/saccrv/calculation/groupcredittrades.hpp
    \brief SACCRV header declares the credit risk manager class for dealing with credit trades in SA-CCR calculations
    \ingroup saccrv
*/
#pragma once

#include <orea/saccrv/saccrvcalculationmanager.hpp>
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace ore {
namespace analytics {

class GroupCreditTrades {
public:
    GroupCreditTrades();
    void groupCreditTrades(
        const std::vector<SaccrvTrades>& groupTrades,
        const std::string& nettingSetId,
        const std::string& counterpartyId,
        const std::string& hedgingSetName = ""
    );
private:
    std::string constructHedgingSetName(
        const std::string& baseName,
        const std::string& refEntity
    );
};

} // namespace analytics
} // namespace ore