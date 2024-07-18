/*! \file /OREAnalytics/orea/saccrv/calculation/calcaddon.hpp
    \brief Definition of the SaccrvAddonCalculator class, computes the SA-CCR add-on for a collection of trades.
    \ingroup saccrv
*/

#pragma once

#include <string>
#include <orea/saccrv/saccrvinputmanager.hpp>
#include <orea/saccrv/saccrvcalculationmanager.hpp>
#include <map>
#include <vector>

namespace ore {
namespace analytics {

class SaccrvAddonCalculator {
public:
    SaccrvAddonCalculator(SaccrvCalculationContext context);

    double calculate();
   
private:
    bool simplified_;
    bool oem_;
    std::string counterpartyId_;
    std::string nettingsetId_;
    bool isMargined_;
    int mporDays_;
    bool hasIlliquidCollateral_;
    bool applyMarginCallDisputes_;
    bool greaterThan5000Transactions_;
    int remarginFreq_;
    double maturityFactor_;

    // SaccrvCalculationManager* calcManager_;

    int getAssetClassIndex(const std::string& assetClassName);
    double getMaturityFactor(const SaccrvTrades& trade);
    double calculateFXAddon(FXData& fxData);
    double calculateIRDAddon(IRDData& irdData);
    double calculateCommodityAddon(CommodityData& commodityData);
    double calculateCreditAddon(CreditData& creditData);
    double calculateEquityAddon(EquityData& equityData);
    double calculateOtherExposureAddon(OtherExposureData& otherData);
    double aggregateAddonsWithCorrelation(const std::vector<double>& addons, const std::vector<double>& correlations);

};

} // namespace analytics
} // namespace ore
