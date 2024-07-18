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