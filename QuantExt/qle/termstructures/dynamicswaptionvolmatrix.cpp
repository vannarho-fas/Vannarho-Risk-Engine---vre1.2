/*
 Copyright (C) 2016 Quaternion Risk Management Ltd
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

#include <qle/termstructures/dynamicswaptionvolmatrix.hpp>

#include <ql/termstructures/volatility/flatsmilesection.hpp>

namespace QuantExt {

DynamicSwaptionVolatilityMatrix::DynamicSwaptionVolatilityMatrix(
    const QuantLib::ext::shared_ptr<SwaptionVolatilityStructure>& source, Natural settlementDays, const Calendar& calendar,
    ReactionToTimeDecay decayMode)
    : SwaptionVolatilityStructure(settlementDays, calendar, source->businessDayConvention(), source->dayCounter()),
      source_(source), decayMode_(decayMode), originalReferenceDate_(source->referenceDate()),
      volatilityType_(source->volatilityType()) {}

const Period& DynamicSwaptionVolatilityMatrix::maxSwapTenor() const { return source_->maxSwapTenor(); }

QuantLib::ext::shared_ptr<SmileSection> DynamicSwaptionVolatilityMatrix::smileSectionImpl(Time optionTime,
                                                                                  Time swapLength) const {
    // null strike to indicate ATM
    return QuantLib::ext::make_shared<FlatSmileSection>(optionTime, volatilityImpl(optionTime, swapLength, Null<Real>()),
                                                source_->dayCounter(), Null<Real>(), source_->volatilityType(),
                                                shiftImpl(optionTime, swapLength));
}

Volatility DynamicSwaptionVolatilityMatrix::volatilityImpl(Time optionTime, Time swapLength, Rate strike) const {
    if (decayMode_ == ForwardForwardVariance) {
        Real tf = source_->timeFromReference(referenceDate());
        if (source_->volatilityType() == ShiftedLognormal) {
            QL_REQUIRE(close_enough(source_->shift(tf + optionTime, swapLength), source_->shift(tf, swapLength)),
                       "DynamicSwaptionVolatilityMatrix: Shift must be constant in option time direction");
        }
        Real realisedVariance =
            source_->blackVariance(tf + optionTime, swapLength, strike) -
            (tf > 0.0 && !close_enough(tf, 0.0) ? source_->blackVariance(tf, swapLength, strike) : 0.0);
        return std::sqrt(std::max(realisedVariance / optionTime, 1E-6));
    }
    if (decayMode_ == ConstantVariance) {
        return source_->volatility(optionTime, swapLength, strike);
    }
    QL_FAIL("unexpected decay mode (" << decayMode_ << ")");
}

Real DynamicSwaptionVolatilityMatrix::shiftImpl(Time optionTime, Time swapLength) const {
    if (source_->volatilityType() == Normal) {
        return 0.0;
    }
    if (decayMode_ == ForwardForwardVariance) {
        Real tf = source_->timeFromReference(referenceDate());
        return source_->shift(tf + optionTime, swapLength);
    }
    if (decayMode_ == ConstantVariance) {
        return source_->shift(optionTime, swapLength);
    }
    QL_FAIL("unexpected decay mode (" << decayMode_ << ")");
}

Real DynamicSwaptionVolatilityMatrix::minStrike() const { return source_->minStrike(); }

Real DynamicSwaptionVolatilityMatrix::maxStrike() const { return source_->maxStrike(); }

Date DynamicSwaptionVolatilityMatrix::maxDate() const {
    if (decayMode_ == ForwardForwardVariance) {
        return source_->maxDate();
    }
    if (decayMode_ == ConstantVariance) {
        return Date(std::min(Date::maxDate().serialNumber(), referenceDate().serialNumber() -
                                                                 originalReferenceDate_.serialNumber() +
                                                                 source_->maxDate().serialNumber()));
    }
    QL_FAIL("unexpected decay mode (" << decayMode_ << ")");
}

void DynamicSwaptionVolatilityMatrix::update() { SwaptionVolatilityStructure::update(); }

} // namespace QuantExt
