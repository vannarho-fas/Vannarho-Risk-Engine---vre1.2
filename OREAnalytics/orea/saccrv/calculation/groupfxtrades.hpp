/*! 
    \file /OREAnalytics/orea/saccrv/calculation/groupfxtrades.hpp
    \brief Header file for the GroupIRDTrades class, which groups IRD trades and manages their calculations within the SACCR framework.
    \ingroup saccrv
*/

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <orea/saccrv/saccrvinputmanager.hpp>
#include <orea/saccrv/saccrvcalculationmanager.hpp>

namespace ore {
namespace analytics {

class GroupFXTrades {
public:
    GroupFXTrades();
    void groupFXTrades(const std::vector<SaccrvTrades>& groupTrades, const std::string& nettingSetId, const std::string& counterpartyId,const std::string& hedgingSetName = "");

private:
    void setFXDynamic(SaccrvTrades& trade);
    std::string constructHedgingSetName(const std::string& baseName, const std::string& ccyPair);
    std::map<std::string, std::vector<SaccrvTrades>> groupTradesByCurrencyPair(const std::vector<SaccrvTrades>& trades, const std::string& hedgingSetName);
    // Additional member variables as needed
};

} // namespace analytics
} // namespace ore