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
    \file /OREAnalytics/orea/saccrv/calculation/groupfxtrades.hpp
    \brief Header file for the GroupIRDTrades class, which groups IRD trades and manages their calculations within the SACCR framework.
    \ingroup saccrv
*/

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <orea/saccrv/saccrvinputmanager.hpp>
#include <orea/saccrv/saccrvcalculationmanager.hpp>

namespace ore {
namespace analytics {

class GroupFXTrades {
public:
    GroupFXTrades();
    void groupFXTrades(const std::vector<SaccrvTrades>& groupTrades, const std::string& nettingSetId, const std::string& counterpartyId,const std::string& hedgingSetName = "");

private:
    void setFXDynamic(SaccrvTrades& trade);
    std::string constructHedgingSetName(const std::string& baseName, const std::string& ccyPair);
    std::map<std::string, std::vector<SaccrvTrades>> groupTradesByCurrencyPair(const std::vector<SaccrvTrades>& trades, const std::string& hedgingSetName);
    // Additional member variables as needed
};

} // namespace analytics
} // namespace ore