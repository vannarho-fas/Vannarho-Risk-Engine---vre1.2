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

/*! \file /oreanalytics/orea/saccrv/alculation/singletradeaddon.cpp
    \brief SACCRV SingleTradeAddon manages calculations for addOn information for individual trades.
    \ingroup saccrv
*/


#include <orea/saccrv/calculation/singletradeaddon.hpp>
#include <orea/saccrv/saccrvinputmanager.hpp>
#include <orea/saccrv/saccrvcalculationmanager.hpp>
#include <orea/saccrv/calculation/calcmf.hpp>
#include <stdexcept>

namespace ore {
namespace analytics {

SingleTradeAddon::SingleTradeAddon(const SaccrvTrades& trade, double maturityFactor, bool simplified)
    : trade_(trade), maturityFactor_(maturityFactor), simplified_(simplified) {
}

void SingleTradeAddon::calculateAddon() {
    LOG("SACCRV: Calculating AddOn for trade: " << trade_.getId());
    calculateAdjustedNotional();
    calculateSupervDelta();
    calculateEffectiveNotional();
}

void SingleTradeAddon::calculateAdjustedNotional() {
    // Logic to calculate adjusted notional based on trade details - CRE52.34

    if (trade_.getTradeGroup() == "IRD" || trade_.getTradeGroup() == "Credit") {
        double Si = trade_.getSi().value_or(0.0);
        double Ei = trade_.getEi().value_or(1.0);

        // If the start date has occurred, set Si to zero
        if (Si < 0) {
            Si = 0;
        }

        if (simplified_) {
            adjustedNotional_ = Ei - Si;
        } else {
            // Calculate adjusted notional by multiplying the notional by the supervisory duration
            // TODO - convert Notional to domestic currency
            adjustedNotional_ = trade_.getNotional().value_or(0.0) * ((exp(-0.05 * Si) - exp(-0.05 * Ei)) / 0.05);
        }
    } else if (trade_.getTradeGroup() == "FX") {
        // Adjusted notional for FX trades
        // Assume trade_.getNotional() returns the notional value of the foreign currency leg of the derivative contract
        // and trade_.getFxRate() returns the current exchange rate
        // TODO - convert Notional to domestic currency
        // adjustedNotional_ = trade_.getNotional().value_or(0.0) * trade_.getFxRate().value_or(1.0);
    } else if (trade_.getTradeGroup() == "Equity" || trade_.getTradeGroup() == "Commodity") {
        // Adjusted notional for Equity and Commodity trades
        // Assume trade_.getCurrentPrice() returns the current price of the relevant share or unit of commodity
        // and trade_.getQuantity() returns the number of shares/units that the derivative references
        //TODO - implement getCurrentPrice()
        if (trade_.getUnderlyingPrice().has_value())  {
            adjustedNotional_ = trade_.getUnderlyingPrice().value() * trade_.getQuantity();
        } else {
            // Handle the case where either trade_.getUnderlyingPrice() or trade_.getQuantity() does not have a value
            // For example, you could set adjustedNotional_ to a default value
            adjustedNotional_ = 0.0;
        }
    }
}


void SingleTradeAddon::calculateSupervDelta() {
    auto configManager = SaccrvConfigManager::getInstance();
    auto supervisoryFactors = configManager->getConfigurations().getSupervisoryFactors();
    std::string assetClass = trade_.getTradeGroup();
    std::string subClass = trade_.getSubClass();
    std::string buySell = trade_.getBuySell();
    std::string optionType = trade_.getOptionType();
    double underlyingPrice = trade_.getUnderlyingPrice().value_or(1.0);
    double strikePrice = trade_.getStrikePrice().value_or(1.0);
    double Si = trade_.getSi().value_or(0.0);
    double Ei = trade_.getEi().value_or(1.0);
    double supervVol = supervisoryFactors[assetClass][subClass].supervisory_option_volatility;

    if (simplified_) {
        supervDelta_ = (buySell == "Buy") ? 1.0 : -1.0;
        return;
    }

    double optionMat;
    if (trade_.getTradeType().find("Option") != std::string::npos || trade_.getTradeType() == "Swap") {
        optionMat = Si;
    } else {
        optionMat = Ei;
    }

    if (trade_.getTradeType() == "Option") {
        double lambda = std::max(0.001 - std::min(underlyingPrice, strikePrice), 0.0);
        double temp = 0.0;

        if (underlyingPrice * strikePrice < 0) {
            temp = (std::log((underlyingPrice + lambda) / (strikePrice + lambda)) + 0.5 * supervVol * supervVol * optionMat) / (supervVol * std::sqrt(optionMat));
        } else {
            temp = (std::log(underlyingPrice / strikePrice) + 0.5 * supervVol * supervVol * optionMat) / (supervVol * std::sqrt(optionMat));
        }

        // Black-Scholes Delta Calculation
        if (buySell == "Buy") {
            if (optionType == "Call") supervDelta_ = normalCDF(temp);
            else if (optionType == "Put") supervDelta_ = -normalCDF(-temp);
        } else if (buySell == "Sell") {
            if (optionType == "Call") supervDelta_ = -normalCDF(temp);
            else if (optionType == "Put") supervDelta_ = normalCDF(-temp);
        }
    } else {
        // Default case for non-option trades
        supervDelta_ = (buySell == "Buy") ? 1.0 : -1.0;
    }
}

// TODO NEW check - need to determine who to check for option trades, and how to get Ti (which should be Ei???)

// void SingleTradeAddon::calculateSupervDelta() {
//     auto configManager = SaccrvConfigManager::getInstance();
//     auto supervisoryFactors = configManager->getConfigurations().getSupervisoryFactors();
//     std::string assetClass = trade_.getTradeGroup();
//     std::string subClass = trade_.getSubClass();
//     std::string buySell = trade_.getBuySell();
//     std::string optionType = trade_.getOptionType();
//     double underlyingPrice = trade_.getUnderlyingPrice().value_or(1.0);
//     double strikePrice = trade_.getStrikePrice().value_or(1.0);
//     double supervVol = supervisoryFactors[assetClass][subClass].supervisory_option_volatility;
//     double Ti = trade_.getTi().value_or(0.0); // Time to maturity

//     // Calculation for Options
//     if (trade_.getTradeType() == "Option") {
//         double lambda = std::max(0.001 - std::min(underlyingPrice, strikePrice), 0.0);
//         double d1 = (std::log((underlyingPrice + lambda) / (strikePrice + lambda)) + 0.5 * supervVol * supervVol * Ti) / (supervVol * std::sqrt(Ti));

//         if (buySell == "Buy") {
//             supervDelta_ = optionType == "Call" ? normalCDF(d1) : -normalCDF(-d1);
//         } else if (buySell == "Sell") {
//             supervDelta_ = optionType == "Call" ? -normalCDF(d1) : normalCDF(-d1);
//         }
//     }
//     // Calculation for CDO tranches and nth-to-default transactions
//     else if (assetClass == "CDO" || assetClass == "Nth-to-Default") {
//         double Ai = trade_.getAi().value_or(1.0);
//         double Di = trade_.getDi().value_or(1.0);
//         supervDelta_ = buySell == "Buy" ? 15 / ((14 * Ai + 1) * (14 * Di + 1)) : -15 / ((14 * Ai + 1) * (14 * Di + 1));
//     }
//     // Default case for non-option, non-CDO trades
//     else {
//         supervDelta_ = buySell == "Buy" ? 1.0 : -1.0;
//     }
// }



double SingleTradeAddon::normalCDF(double value) {
    // Standard Normal Cumulative Distribution Function
    return 0.5 * erfc(-value * M_SQRT1_2);
}


void SingleTradeAddon::calculateEffectiveNotional() {
    effectiveNotional_ = supervDelta_ * adjustedNotional_ * maturityFactor_;
    LOG("SACCRV: Effective notional: " << effectiveNotional_ << " supervdelta: " << supervDelta_ << " adjusted notional: " << adjustedNotional_ << " maturity factor: " << maturityFactor_);
}

// class LongRiskFactorStrategy : public SupervDeltaStrategy {
// public:
//     double calculateSupervDelta(const SaccrvTrades& trade) override {
//         // Implement the calculation for long risk factor
//         // For example:
//         if (trade.getMarketValue() > 0) {
//             return 1.0;
//         } else {
//             return -1.0;
//         }
//     }
// };

// class ShortRiskFactorStrategy : public SupervDeltaStrategy {
// public:
//     double calculateSupervDelta(const SaccrvTrades& trade) override {
//         // Implement the calculation for short risk factor
//         // For example:
//         if (trade.getMarketValue() < 0) {
//             return -1.0;
//         } else {
//             return 1.0;
//         }
//     }
// };

// class OptionStrategy : public SupervDeltaStrategy {
// public:
//     double calculateSupervDelta(const SaccrvTrades& trade) override {
//         // Implement the calculation for options
//         // For example:
//         if (trade.getOptionType() == "Call") {
//             double lambda = std::max(0.001 - std::min(trade.getUnderlyingPrice(), trade.getStrikePrice()), 0.0);
//             double temp = (std::log((trade.getUnderlyingPrice() + lambda) / (trade.getStrikePrice() + lambda)) + 0.5 * trade.getSupervVol() * trade.getOptionMat()) / (trade.getSupervVol() * std::sqrt(trade.getOptionMat()));
//             return normalCDF(temp);
//         } else if (trade.getOptionType() == "Put") {
//             double lambda = std::max(0.001 - std::min(trade.getUnderlyingPrice(), trade.getStrikePrice()), 0.0);
//             double temp = (std::log((trade.getUnderlyingPrice() + lambda) / (trade.getStrikePrice() + lambda)) + 0.5 * trade.getSupervVol() * trade.getOptionMat()) / (trade.getSupervVol() * std::sqrt(trade.getOptionMat()));
//             return -normalCDF(-temp);
//         }
//     }
// };

// class CDOTrancheStrategy : public SupervDeltaStrategy {
// public:
//     double calculateSupervDelta(const SaccrvTrades& trade) override {
//         // Implement the calculation for CDO tranches
//         // For example:
//         double Ai = trade.getAttachmentPoint();
//         double Di = trade.getDetachmentPoint();
//         return 15.0 / ((14.0 * Ai + 1.0) * (14.0 * Di + 1.0));
//     }
// };


} // namespace analytics
} // namespace ore