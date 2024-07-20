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

/*! \file /OREAnalytics/orea/saccrv/saccrvtradedatafactory.hpp
    \brief SACCRV factory header for loading parse trades into SaccrvTrades objects
    \ingroup saccrv
*/

#pragma once

#include <vector>
#include <string>
#include <ored/portfolio/portfolio.hpp>
#include <ored/portfolio/trade.hpp>


#include <orea/saccrv/tradeparsers/saccrvtradeparser.hpp>
// #include <orea/saccrv/saccrvtrades.hpp>
#include <orea/saccrv/saccrvinputmanager.hpp> 

namespace ore {
namespace analytics {



class SaccrvTradeDataFactory {
public:
    SaccrvTradeDataFactory(); //constructor
    static SaccrvTrades buildTradeData(const boost::shared_ptr<Trade>& trade);;

private:
    static std::map<std::string, boost::shared_ptr<ISaccrvTradeParser>> parserMap;
};

} // namespace analytics
} // namespace ore

    // static std::vector<SaccrvTrades> compileTrades(const std::vector<SaccrvTrades>& selectedDerivatives,
    //                                                const boost::shared_ptr<ore::data::Portfolio>& myPortfolio);
    // static std::vector<SaccrvTrades> populateSaccrvTradesFromPortfolio(const std::vector<SaccrvTrades>& saccrvNetTrades, const boost::shared_ptr<ore::data::Portfolio>& myPortfolio);
    // static std::vector<SaccrvTrades> configureTrades(const boost::shared_ptr<ore::data::Portfolio>& myPortfolio);
    // static std::vector<SaccrvTrades> selectDerivatives(const std::vector<SaccrvTrades>& saccrvTrades);