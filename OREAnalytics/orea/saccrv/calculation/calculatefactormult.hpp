/*! \file OREAnalytics/orea/saccrv/calculation/calculatefactormult.hpp
    \brief Utility functions for Standardised Approach for Counterparty Credit Risk calculations
    \ingroup saccrv
*/

#pragma once

#include <string>

namespace ore {
namespace analytics {

class FactorMultiplierCalculator {
public:
    static double CalculateFactorMult(const std::string& hedgingSetName);
};

} // namespace analytics
} // namespace ore