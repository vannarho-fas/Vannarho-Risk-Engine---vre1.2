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