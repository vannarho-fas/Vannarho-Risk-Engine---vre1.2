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