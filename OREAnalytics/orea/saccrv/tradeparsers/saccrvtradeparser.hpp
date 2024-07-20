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

/*! \file /OREAnalytics/orea/saccrv/tradeparsers/saccrvtradeparser.hpp
    \brief SACCRV virtual class to orchestrate different trade parsers
    \ingroup saccrv/tradeparsers
*/

#pragma once

#include <boost/shared_ptr.hpp>

// #include <orea/saccrv/saccrvtrades.hpp>
#include <orea/saccrv/saccrvinputmanager.hpp> 


using namespace ore::data;
using namespace boost::filesystem;

namespace ore {
namespace analytics {


class ISaccrvTradeParser {
public:
    virtual ~ISaccrvTradeParser() = default;
    virtual SaccrvTrades parse(const boost::shared_ptr<Trade>& trade) = 0;
    // virtual SaccrvTrades parseAndLog(const boost::shared_ptr<Trade>& trade) = 0;
};



} // namespace analytics
} // namespace ore

