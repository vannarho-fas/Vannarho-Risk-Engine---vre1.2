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
 * \brief SACCRV header for calculating the Projected Future Exposure (PFE) and other related metrics.
 * \ingroup saccrv
 */

#include <orea/saccrv/calculation/calcpfe.hpp>

namespace ore {
namespace analytics {

double SaccrvExposureCalculator::calcPFE(double V_C, double addonAggregate, bool simplified, double V) {
    if (V == 0.0)
        V = V_C;

    V_C = std::min(V_C, V);
    double multiplier = 1.0;

    if (!simplified) {
        if (V_C < 0.0) {
            multiplier = std::min(1.0, 0.05 + 0.95 * std::exp(V_C / (1.9 * addonAggregate)));
        }
    }

    double PFE = multiplier * addonAggregate;
    return PFE;
}

} // namespace analytics
} // namespace ore