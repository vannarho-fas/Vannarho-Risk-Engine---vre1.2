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

/**
 * \file /OREAnalytics/orea/saccrv/calculation/calcpfe.hpp
 * \brief SACCRV header for calculating the Projected Future Exposure (PFE).     // Calculates the Projected Future Exposure (PFE) after applying the relevant multiplier.
    // @param V_C The difference between the sum of the MtMs and the collateral.
    // @param addonAggregate The aggregate amount of the addOn.
    // @param simplified When true, the multiplier will be set to 1 as per the simplified & OEM approach.
    // @param V The sum of MVs of the trades, defaults to 0.
    // @return The Projected Future Exposure (PFE).
 * \ingroup saccrv
 */

#pragma once

#include <algorithm>
#include <cmath>

namespace ore {
namespace analytics {

class SaccrvExposureCalculator {
public:

    static double calcPFE(double V_C, double addonAggregate, bool simplified, double V = 0.0);
};

} // namespace analytics
} // namespace ore