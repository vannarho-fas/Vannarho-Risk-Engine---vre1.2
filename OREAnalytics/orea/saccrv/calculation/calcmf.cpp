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

/*! \file /OREAnalytics/orea/saccrv/calculation/calcmf.hpp
    \brief This implementation file contains declarations for SA-CCR calculation of MF.
    \ingroup saccrv
*/

#include <orea/saccrv/calculation/calcmf.hpp>
#include <algorithm>
#include <cmath>

namespace ore {
namespace analytics {

double calculateMaturityFactor(const SaccrvTrades& trade) {
    double Ei = trade.getEi().value_or(1.0); // Default to 1 if Ei is not available
    double Si = trade.getSi().value_or(0.0); // Default to 0 if Si is not available
    LOG("Ei: " << Ei << " Si: " << Si);

    // Calculate the effective maturity as the maximum of (Ei - Si, 10 days), expressed in years
    double effectiveMaturity = std::max((Ei - Si), 10.0 / 365.0);

    // Calculate the maturity factor using the SA-CCR formula
    double maturityFactor = std::sqrt(std::min(effectiveMaturity, 1.0));

    return maturityFactor;
}

} // namespace analytics
} // namespace ore





// /*! \file /OREAnalytics/orea/saccrv/calculation/calcmf.hpp
//     \brief This implementation file contains declarations for SA-CCR calculation of MF.
//     \ingroup saccrv
// */

// #include <orea/saccrv/calculation/calcmf.hpp>

// namespace ore {
// namespace analytics {

// double calculateMaturityFactor(const SaccrvTrades& trade) {
//     double Mi = trade.getEi().value_or(1.0); // Default to 1 if Ei is not available
//     std::string delType = trade.getDeliveryType();

//     if (delType == "Physical" || delType.empty()) {
//         Mi = trade.getSi().value_or(Mi);
//     }

//     double maturityFactor = (Mi < 1) ? std::sqrt(Mi) : 1.0;

//     if (trade.getTradeType() == "EquityFutureOption") {
//         maturityFactor = 10.0 / 252.0;
//     }

//     return std::max(10.0 / 252.0, maturityFactor);
// }
// } // namespace analytics
// } // namespace ore
