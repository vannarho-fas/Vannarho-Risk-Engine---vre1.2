/*! 
    \file /OREAnalytics/orea/saccrv/calculation/groupcredittrades.hpp
    \brief SACCRV header declares the credit risk manager class for dealing with credit trades in SA-CCR calculations
    \ingroup saccrv
*/
#pragma once

#include <orea/saccrv/saccrvcalculationmanager.hpp>
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace ore {
namespace analytics {

class GroupCreditTrades {
public:
    GroupCreditTrades();
    void groupCreditTrades(
        const std::vector<SaccrvTrades>& groupTrades,
        const std::string& nettingSetId,
        const std::string& counterpartyId,
        const std::string& hedgingSetName = ""
    );
private:
    std::string constructHedgingSetName(
        const std::string& baseName,
        const std::string& refEntity
    );
};

} // namespace analytics
} // namespace ore