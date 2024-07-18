/*! \file /OREAnalytics/orea/saccrv/calculation/calcrc.cpp
    \brief SACCRV header for SA-CCR calculation functions such as Replacement Cost (RC) and Potential Future Exposure (PFE).
    \ingroup saccrv
*/


#include <orea/saccrv/calculation/calcrc.hpp>

namespace ore {
namespace analytics {

std::tuple<double, double, double> SaccrvRCCalculation::computeReplacementCost(const std::vector<double>& mtms,
                                                                             const SaccrvCsa* csa,
                                                                             const std::vector<SaccrvCollateral>& collaterals,
                                                                             bool simplified, bool ignore_margin) {
    double V = std::accumulate(mtms.begin(), mtms.end(), 0.0);
    double V_C, RC;

    if (!csa) {
        V_C = V;
        RC = std::max(V_C, 0.0);
    } else {
        double imCpty = csa->imCpty;
        double mtaCpty = csa->mtaCpty;
        double thresCpty = csa->thresCpty;
        double current_collateral = 0.0;

        if (!collaterals.empty()) {
            std::vector<SaccrvCollateral> collateral_match;
            std::copy_if(collaterals.begin(), collaterals.end(), std::back_inserter(collateral_match),
                         [&csa](const SaccrvCollateral& c) { return c.getNettingSetID() == csa->id; });

            for (const auto& col : collateral_match) {
                if (col.type == "ICA") {
                    current_collateral += col.amount;
                    imCpty = col.amount;
                } else if (col.type == "VariationMargin") {
                    current_collateral += col.amount;
                }
            }
        }

        if (simplified) {
            V_C = V;
            RC = std::max({0.0, V_C, thresCpty + mtaCpty});
            if (ignore_margin) {
                RC = std::max(0.0, V_C);
            }
        } else {
            V_C = V - current_collateral;
            RC = std::max({0.0, V_C, thresCpty + mtaCpty - imCpty});
            if (ignore_margin) {
                RC = std::max(0.0, V_C);
            }
        }
    }

    return std::make_tuple(V_C, RC, V);
}

} // namespace analytics
} // namespace ore