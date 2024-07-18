
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


/*! \file OREAnalytics/orea/saccrv/calculation/determineccrmethodology.cpp
    \brief Provides the function to determine the Counterparty Credit Risk (CCR) methodology.
    \ingroup saccrv
*/


#include <orea/saccrv/calculation/determineccrmethodology.hpp>
#include <orea/saccrv/saccrvinputmanager.hpp> // Assuming this includes access to trade data

namespace ore {
namespace analytics {

std::string determineCCRMethodology(double total_assets) {

    // Assuming that the derivative's market value can be obtained from SaccrvTradesManager.
    SaccrvTradesManager* saccrvMgr = SaccrvTradesManager::getInstance();
    const std::vector<SaccrvTrades>& trades = saccrvMgr->getTradeVector();
    double derivatives_mv = 0.0;

    // Calculate the sum of the absolute mTm of the trades
    for (const auto& trade : trades) {
        if (trade.mTm.has_value()) {
            derivatives_mv += std::abs(trade.mTm.value());
        }
    }

    std::string methodology;

    if (derivatives_mv / total_assets < 0.05 && derivatives_mv < 100000000) {
        methodology = "Original Exposure Method";
    } else if (derivatives_mv / total_assets < 0.1 && derivatives_mv < 300000000) {
        methodology = "Simplified SA-CCR";
    } else {
        methodology = "SA-CCR";
    }

    return methodology;
}

} // namespace analytics
} // namespace ore
