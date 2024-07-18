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

