
/*! \file /OREAnalytics/orea/saccrv/calculation/calcmf.hpp
    \brief This header file contains declarations for SA-CCR calculation of maturity factor.
    \ingroup saccrv
*/

#pragma once

#include <orea/saccrv/saccrvinputmanager.hpp>

namespace ore {
namespace analytics {
    // Declaration for the function to calculate Exposure at Default
    double calculateMaturityFactor(const SaccrvTrades& trade);
} // namespace analytics
} // namespace ore