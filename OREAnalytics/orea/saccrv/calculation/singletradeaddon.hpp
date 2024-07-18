/*! \file /oreanalytics/orea/saccrv/calculation/singletradeaddon.hpp
    \brief SACCRV SingleTradeAddon manages calculations for addOn information for individual trades.
    \ingroup saccrv
*/

#pragma once

#include <string>
#include <memory>
#include <orea/saccrv/saccrvinputmanager.hpp> // Include necessary managers
#include <orea/saccrv/saccrvcalculationmanager.hpp>

namespace ore {
namespace analytics {

class SingleTradeAddon {
public:
    SingleTradeAddon(const SaccrvTrades& trade, double maturityFactor, bool simplified);
    void calculateAddon();

        // Getter methods
    double getAdjustedNotional() const {
        return adjustedNotional_;
    }

    double getSupervDelta() const {
        return supervDelta_;
    }

    double getEffectiveNotional() const {
        return effectiveNotional_;
    }

private:
    SaccrvTrades trade_;
    double adjustedNotional_;
    double maturityFactor_;
    double supervisoryFactor_;
    double supervisoryOptionVolatlity_;
    double supervDelta_;
    double effectiveNotional_;
    bool simplified_;

    void calculateAdjustedNotional();
    void calculateMaturityFactor();
    void calculateSupervDelta();
    double normalCDF(double value);
    double calcSupervDuration();
    void calculateEffectiveNotional();
};

// class SupervDeltaStrategy {
// public:
//     virtual double calculateSupervDelta(const SaccrvTrades& trade) = 0;
// };

// class LongRiskFactorStrategy : public SupervDeltaStrategy {
// public:
//     double calculateSupervDelta(const SaccrvTrades& trade) override;
// };

// class ShortRiskFactorStrategy : public SupervDeltaStrategy {
// public:
//     double calculateSupervDelta(const SaccrvTrades& trade) override;
// };

// class OptionStrategy : public SupervDeltaStrategy {
// public:
//     double calculateSupervDelta(const SaccrvTrades& trade) override;
// };

// class CDOTrancheStrategy : public SupervDeltaStrategy {
// public:
//     double calculateSupervDelta(const SaccrvTrades& trade) override;
// };


} // namespace analytics
} // namespace ore