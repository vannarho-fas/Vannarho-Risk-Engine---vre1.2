/*
 Copyright (C) 2021 Quaternion Risk Management Ltd
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

/*! \file qle/termstructures/iborfallbackcurve.hpp
    \brief projection curve for ibor fallback indices
*/

#include <qle/termstructures/overnightfallbackcurve.hpp>

#include <qle/cashflows/overnightindexedcoupon.hpp>

namespace QuantExt {

OvernightFallbackCurve::OvernightFallbackCurve(const QuantLib::ext::shared_ptr<OvernightIndex>& originalIndex,
                                     const QuantLib::ext::shared_ptr<OvernightIndex>& rfrIndex, const Real spread,
                                     const Date& switchDate)
    : YieldTermStructure(originalIndex->forwardingTermStructure()->dayCounter()), originalIndex_(originalIndex),
      rfrIndex_(rfrIndex), spread_(spread), switchDate_(switchDate) {
    registerWith(originalIndex->forwardingTermStructure());
    registerWith(rfrIndex->forwardingTermStructure());
    // Always enable extrapolation: The original and rfr index forwarding curves might have different settings
    // so we do not want to make things overly complicated here.
    enableExtrapolation();
}

QuantLib::ext::shared_ptr<OvernightIndex> OvernightFallbackCurve::originalIndex() const { return originalIndex_; }

QuantLib::ext::shared_ptr<OvernightIndex> OvernightFallbackCurve::rfrIndex() const { return rfrIndex_; }

Real OvernightFallbackCurve::spread() const { return spread_; }

const Date& OvernightFallbackCurve::switchDate() const { return switchDate_; }

const Date& OvernightFallbackCurve::referenceDate() const {
    return originalIndex_->forwardingTermStructure()->referenceDate();
}

Date OvernightFallbackCurve::maxDate() const { return originalIndex_->forwardingTermStructure()->maxDate(); }

Calendar OvernightFallbackCurve::calendar() const { return originalIndex_->forwardingTermStructure()->calendar(); }

Natural OvernightFallbackCurve::settlementDays() const {
    return originalIndex_->forwardingTermStructure()->settlementDays();
}

Real OvernightFallbackCurve::discountImpl(QuantLib::Time t) const {
    Date today = Settings::instance().evaluationDate();
    if (today < switchDate_) {
        return originalIndex_->forwardingTermStructure()->discount(t);
    }
    Date endDate = today + originalIndex_->tenor();
    Real couponTime = rfrIndex_->dayCounter().yearFraction(today, endDate);
    Real curveTime = timeFromReference(endDate);
    Real s = std::log(1.0 + couponTime * spread_) / curveTime;
    return rfrIndex_->forwardingTermStructure()->discount(t) * std::exp(-s * t);
}

} // namespace QuantExt
