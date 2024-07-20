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

/*! \file /OREAnalytics/orea/saccrv/saccrvtradedatafactory.cpp
    \brief SACCRV factory for loading parse trades into SaccrvTrades objects
    \ingroup saccrv
*/

#include <regex>
#include <sstream>
#include <set>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include <ored/portfolio/portfolio.hpp>
#include <ored/portfolio/trade.hpp>

#include <orea/saccrv/saccrvtradedatafactory.hpp>
#include <orea/saccrv/tradeparsers/swapparser.hpp>
#include <orea/saccrv/tradeparsers/swaptionparser.hpp>



namespace ore {
namespace analytics {

std::map<std::string, boost::shared_ptr<ISaccrvTradeParser>> SaccrvTradeDataFactory::parserMap;

SaccrvTradeDataFactory::SaccrvTradeDataFactory() {
    // Instantiate a parser for each trade type and add it to the map.
    parserMap["Swap"] = boost::make_shared<SwapParser>();
    parserMap["Swaption"] = boost::make_shared<SwaptionParser>();
    // ... and so on for other trade types.
    LOG("SACCRV: Instantiate parsers");

    }


SaccrvTrades SaccrvTradeDataFactory::buildTradeData(const boost::shared_ptr<Trade>& trade) {
    std::string tradeType = trade->tradeType();
    LOG("SACCRV: Processing trade for tradeType: " << tradeType);
    
    if (parserMap.find(tradeType) != parserMap.end()) {
        return parserMap[tradeType]->parse(trade);
    } else {
        LOG("SACCRV: Parser not found for tradeType: " << tradeType);
        return SaccrvTrades(); // Return an empty or default-initialized struct
    }
}




} // namespace analytics
} // namespace ore

