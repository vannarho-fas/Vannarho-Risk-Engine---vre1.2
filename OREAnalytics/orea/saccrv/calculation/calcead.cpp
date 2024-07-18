/*! \file /OREAnalytics/orea/saccrv/calculation/calcead.hpp
    \brief This implementation file contains declarations for SA-CCR calculation of EAD.
    \ingroup saccrv
*/

#include <orea/saccrv/calculation/calcead.hpp>

namespace ore {
namespace analytics {

    // Implementation of the function to calculate Exposure at Default
    double calcEAD(double RC, double PFE) {
        // Multiply the sum of replacement cost and projected future exposure by the alpha factor (1.4)
        double EAD = 1.4 * (RC + PFE);
        return EAD;
    }
    
} // namespace analytics
} // namespace ore
