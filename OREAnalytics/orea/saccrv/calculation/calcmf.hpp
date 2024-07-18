
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