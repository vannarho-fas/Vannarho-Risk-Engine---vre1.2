/*
 Copyright (C) 2017 Quaternion Risk Management Ltd
 Copyright (C) 2024 Growth Mindset Pty Ltd
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

#include <qle/pricingengines/mclgmswaptionengine.hpp>

#include <ql/exercise.hpp>

namespace QuantExt {

void McLgmSwaptionEngine::calculate() const {
    leg_ = arguments_.legs;
    currency_ = std::vector<Currency>(leg_.size(), model_->irlgm1f(0)->currency());
    payer_.resize(arguments_.payer.size());
    for (Size i = 0; i < arguments_.payer.size(); ++i) {
        payer_[i] = QuantLib::close_enough(arguments_.payer[i], -1.0);
    }
    exercise_ = arguments_.exercise;
    optionSettlement_ = arguments_.settlementType;
    McMultiLegBaseEngine::calculate();
    results_.value = resultValue_;
    results_.additionalResults["underlyingNpv"] = resultUnderlyingNpv_;
    results_.additionalResults["amcCalculator"] = amcCalculator();
} // McLgmSwaptionEngine::calculate

void McLgmNonstandardSwaptionEngine::calculate() const {
    leg_ = arguments_.legs;
    currency_ = std::vector<Currency>(leg_.size(), model_->irlgm1f(0)->currency());
    payer_.resize(arguments_.payer.size());
    for (Size i = 0; i < arguments_.payer.size(); ++i) {
        payer_[i] = QuantLib::close_enough(arguments_.payer[i], -1.0);
    }
    exercise_ = arguments_.exercise;
    optionSettlement_ = arguments_.settlementType;
    McMultiLegBaseEngine::calculate();
    results_.value = resultValue_;
    results_.additionalResults["underlyingNpv"] = resultUnderlyingNpv_;
    results_.additionalResults["amcCalculator"] = amcCalculator();
} // McLgmSwaptionEngine::calculate

} // namespace QuantExt
