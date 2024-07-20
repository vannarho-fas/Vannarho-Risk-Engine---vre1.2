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

/*! \file /OREAnalytics/orea/saccrv/tradeparsers/utils.hpp
    \brief SACCRV parser utilities
    \ingroup saccrv/tradeparsers
*/

#pragma once

#include <string>

#include <ql/time/date.hpp>
#include <ql/time/period.hpp>

// #include <orea/saccrv/saccrvtrades.hpp>
#include <orea/saccrv/saccrvinputmanager.hpp> 
#include <orea/saccrv/saccrvcsa.hpp>


namespace ore {
namespace analytics {

double parseTenor(const std::string& tenor);
double dateToFloatYears(const QuantLib::Date& asOfDate, const QuantLib::Date& targetDate);
int periodToDays(const QuantLib::Period& period);
std::pair<float, float> calculateDuration(const QuantLib::Date& asOfDate, 
                                          const std::string& startDateStr, 
                                          const std::string& endDateStr);
// Overload the << operator to print SaccrvTrades struct
std::ostream& operator<<(std::ostream& os, const SaccrvTrades& trade);
// Overload the << operator to print SaccrvCsa struct
std::ostream& operator<<(std::ostream& os, const SaccrvCsa& csa);

} // namespace analytics
} // namespace ore
