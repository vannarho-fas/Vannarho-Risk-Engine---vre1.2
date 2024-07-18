/*! \file /OREAnalytics/orea/saccrv/calculation/calcrc.hpp
    \brief SACCRV header for SA-CCR calculation functions such as Replacement Cost (RC) and Potential Future Exposure (PFE).
    \ingroup saccrv
*/

#pragma once

#include <vector>
#include <algorithm>
#include <orea/saccrv/saccrvinputmanager.hpp> 


namespace ore {
namespace analytics {

class SaccrvRCCalculation {
public:
    static std::tuple<double, double, double> computeReplacementCost(const std::vector<double>& mtms, const SaccrvCsa* csa,
                                                                     const std::vector<SaccrvCollateral>& collaterals,
                                                                     bool simplified, bool ignore_margin);
};

} // namespace analytics
} // namespace ore