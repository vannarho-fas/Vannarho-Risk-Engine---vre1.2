#pragma once

#include <boost/shared_ptr.hpp>
#include <ored/portfolio/portfolio.hpp>
#include <ored/portfolio/trade.hpp>
#include <orea/saccrv/tradeparsers/saccrvtradeparser.hpp>
// #include <orea/saccrv/saccrvtrades.hpp>
#include <orea/saccrv/saccrvinputmanager.hpp> 



namespace ore {
namespace analytics {

class SwapParser : public ISaccrvTradeParser {
public:
    virtual SaccrvTrades parse(const boost::shared_ptr<Trade>& saccrvTrade) override;

};

} // namespace analytics
} // namespace ore

