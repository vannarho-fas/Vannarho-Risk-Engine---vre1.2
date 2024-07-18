/*! \file OREAnalytics/orea/saccrv/calculation/calculatefactormult.hpp
    \brief Utility functions for Standardised Approach for Counterparty Credit Risk calculations
    \ingroup saccrv
*/

#include <orea/saccrv/calculation/calculatefactormult.hpp>
#include <string>

namespace ore {
namespace analytics {

double FactorMultiplierCalculator::CalculateFactorMult(const std::string& hedgingSetName) {
    double factorMult = 1.0;
    if (hedgingSetName.substr(0, 4) == "Vol_") {
        factorMult = 5.0;
    } else if (hedgingSetName.substr(0, 6) == "Basis_") {
        factorMult = 0.5;
    } // else branch is not required because factorMult is initialized to 1.0
    return factorMult;
}

} // namespace analytics
} // namespace ore