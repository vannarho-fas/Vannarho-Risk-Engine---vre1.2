
/*! \file /OREAnalytics/orea/saccrv/calculation/calcead.hpp
    \brief This header file contains declarations for SA-CCR calculation of EAD.
    \ingroup saccrv
*/

#pragma once

namespace ore {
namespace analytics {
    // Declaration for the function to calculate Exposure at Default
    double calcEAD(double RC, double PFE);
} // namespace analytics
} // namespace ore