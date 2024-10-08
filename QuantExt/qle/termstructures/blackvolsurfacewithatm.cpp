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

#include <qle/termstructures/blackvolsurfacewithatm.hpp>

namespace QuantExt {

BlackVolatilityWithATM::BlackVolatilityWithATM(const QuantLib::ext::shared_ptr<BlackVolTermStructure>& surface,
                                               const Handle<Quote>& spot, const Handle<YieldTermStructure>& yield1,
                                               const Handle<YieldTermStructure>& yield2)
    : BlackVolatilityTermStructure(0, surface->calendar(), surface->businessDayConvention(), surface->dayCounter()),
      surface_(surface), spot_(spot), yield1_(yield1), yield2_(yield2) {

    QL_REQUIRE(!spot.empty(), "No spot handle provided");

    if (surface->allowsExtrapolation())
        this->enableExtrapolation();

    registerWith(surface);
    registerWith(spot);
    registerWith(yield1);
    registerWith(yield2);
}

Volatility BlackVolatilityWithATM::blackVolImpl(Time t, Real strike) const {
    if (strike == Null<Real>() || strike == 0) {
        // calculate fwd(t)
        strike = spot_->value();
        if (!yield1_.empty() && !yield2_.empty())
            strike *= yield2_->discount(t, true) / yield1_->discount(t, true);
    }
    return surface_->blackVol(t, strike);
}

} // namespace QuantExt
