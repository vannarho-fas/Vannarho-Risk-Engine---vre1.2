/*! 
    \file /OREAnalytics/orea/saccrv/calculation/groupirdtrades.hpp
    \brief GroupIRDTrades header declares functions for grouping interest rate derivative trades by currencies and time buckets.
    \ingroup saccrv
*/

#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <orea/saccrv/saccrvinputmanager.hpp>
#include <orea/saccrv/saccrvcalculationmanager.hpp>

namespace ore {
namespace analytics {

/*! The IRDTradeGrouping class handles grouping of interest rate derivative trades by currency and time buckets. */
class GroupIRDTrades {
public:

    GroupIRDTrades();
    void groupIRDTrades(const std::vector<SaccrvTrades>& groupTrades, const std::string& nettingSetId, const std::string& counterpartyId, const std::string& hedgingSetName = "");

private:
    std::vector<std::string> getUniqueCurrencies(const std::vector<SaccrvTrades>& trades);
    std::map<int, std::vector<SaccrvTrades>> groupTradesByTimeBucket(const std::vector<SaccrvTrades>& trades, const std::string& currency);
    int calcTimeBucket(double Ei);
};

} // namespace analytics
} // namespace ore