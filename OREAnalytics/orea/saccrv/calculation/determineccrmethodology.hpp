
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

