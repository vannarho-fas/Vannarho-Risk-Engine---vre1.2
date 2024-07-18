
/*! \file OREAnalytics/orea/saccrv/calculation/determineccrmethodology.hpp
    \brief Provides the function to determine the Counterparty Credit Risk (CCR) methodology.
    \ingroup saccrv
*/


#include <string>

namespace ore {
namespace analytics {

/**
 * Determine the CCR methodology that the institution is eligible to utilize.
 * 
 * @param total_assets       The total assets of the institution in million EUR.
 * @return                   A string specifying the CCR methodology the institution is eligible to utilize.
 */

std::string determineCCRMethodology(double total_assets);

} // namespace analytics
} // namespace ore

