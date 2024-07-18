/*
 Copyright (C) 2024 Vannarho Pty Ltd
 All rights reserved.

 This file is part of VRE, a free-software/open-source library
 for transparent pricing and risk analysis

 VRE is free software: you can redistribute it and/or modify it
 under the terms of the Modified BSD License.  You should have received a
 copy of the license along with this program.


 This program is distributed on the basis that it will form a useful
 contribution to risk analytics and model standardisation, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
*/

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
