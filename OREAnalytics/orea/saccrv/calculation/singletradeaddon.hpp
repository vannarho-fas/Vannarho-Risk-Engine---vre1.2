/*
 Copyright (C) 2024 Vannarho Pty Ltd
 All rights reserved.

 This file is part of VRE, a free-software/open-source library
 for transparent pricing and risk analysis

 VRE is free software: you can redistribute it and/or modify it
 under the terms of the Modified BSD License.  You should have received a
 copy of the license along with this program.


 This program is distributed on the basis that it will form a useful
 contribution to risk analytics and model standardisation, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
*/

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