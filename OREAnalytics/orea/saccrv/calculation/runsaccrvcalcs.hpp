
/*! \file /OREAnalytics/orea/saccrv/calculation/runsaccrvcalcs.hpp
    \brief SACCR runsaccrvcalcs header file that defines the RunSaccrvCalculations class.
    \ingroup saccrv
*/

#pragma once

#include <orea/saccrv/saccrvinputmanager.hpp>
#include <orea/saccrv/saccrvcalculationmanager.hpp>
#include <memory>
#include <boost/config.hpp>
#include <orea/saccrv/saccrvcalculationmanager.hpp>

namespace ore {
namespace analytics {

class RunSaccrvCalculations {

public:
    RunSaccrvCalculations();
    void runCalcs();
    double calculateEAD(SaccrvCalculationContext context);
    std::vector<std::string> getUniqueCounterpartiesFromTrades(const std::vector<SaccrvTrades>& trades);
    std::vector<SaccrvTrades> filterTradesByCounterparty(const std::vector<SaccrvTrades>& trades, const std::string& counterpartyId);
    std::vector<SaccrvTrades> filterTradesWithNoNettingSet(const std::vector<SaccrvTrades>& trades);
    bool counterpartyIsAssociatedWithCsa(const std::string& counterpartyId, const SaccrvCsa& csa);
    bool allTradesAreSoldOptions(const std::vector<SaccrvTrades>& trades);
    SaccrvCalculationContext createContext(const std::string& counterpartyId, const std::string& nettingsetId, const std::vector<SaccrvTrades>& trades, bool isMargined, const SaccrvCsa& csa, bool simplified, const std::vector<SaccrvCollateral>& collaterals, bool OEM, bool ignoreMargin);

private:
    SaccrvCalculationManager* calculationManager_; 
    std::vector<double> getMtmsForNettingSet(std::string counterpartyId, std::string nettingsetId);
    std::vector<SaccrvCollateral> filterCollateralsByCsa(const std::vector<SaccrvCollateral>& collaterals, const SaccrvCsa& csa);
    std::vector<SaccrvTrades> filterTradesByCsa(const std::vector<SaccrvTrades>& trades, const SaccrvCsa& csa);
    std::vector<SaccrvTrades> filterTradesWithNoNettingSets(const std::vector<SaccrvTrades>& trades);
    std::vector<std::string> getCounterpartiesFromTrades(const std::vector<SaccrvTrades>& trades);
    // Additional private helper functions as needed
};

} // namespace analytics
} // namespace ore

