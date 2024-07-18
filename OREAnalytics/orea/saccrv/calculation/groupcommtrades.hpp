/*! \file /OREAnalytics/orea/saccrv/calculation/groupcommtrades.hpp
    \brief SACCRV header to define the interface for grouping trades by hedging sets and commodity types.
    \ingroup saccrv
*/

#pragma once

#include <vector>
#include <string>
#include <memory>
#include <orea/saccrv/saccrvcalculationmanager.hpp>

namespace ore {
namespace analytics {

class GroupCommodityTrades {
public:
    GroupCommodityTrades(); // Constructor
    void groupCommodityTrades(const std::vector<SaccrvTrades>& groupTrades, std::string nettingSetId, const std::string& counterpartyId,const std::string& hedgingSetName = "");

private:
    std::vector<std::string> getUniqueSubClasses(const std::vector<SaccrvTrades>& trades);
    std::vector<std::string> getUniqueCommodityTypes(const std::vector<SaccrvTrades>& trades);
};

} // namespace analytics
} // namespace ore
