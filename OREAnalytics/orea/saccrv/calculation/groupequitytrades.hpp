/*! 
    \file /OREAnalytics/orea/saccrv/calculation/groupequitytrades.hpp
    \brief Header file for the GroupEquityTrades class, which groups equity trades and manages their calculations within the SACCR framework.
    \ingroup saccrv
*/

#include <orea/saccrv/saccrvcalculationmanager.hpp>
#include <algorithm>

namespace ore {
namespace analytics {

class GroupEquityTrades {
public:
    GroupEquityTrades();

    void groupEquityTrades(const std::vector<SaccrvTrades>& groupTrades, const std::string& nettingSetId, const std::string& counterpartyId,const std::string& hedgingSetName = "");

private:
    std::map<std::string, std::vector<SaccrvTrades>> groupTradesByRefEntities(
    const std::vector<SaccrvTrades>& trades
    );
    std::string constructHedgingSetName(
    const std::string& baseName,
    const std::string& refEntity
    );

};

} // namespace analytics
} // namespace ore