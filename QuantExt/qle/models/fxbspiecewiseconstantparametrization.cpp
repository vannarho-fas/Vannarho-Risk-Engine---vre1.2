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

#include <qle/models/fxbspiecewiseconstantparametrization.hpp>

namespace QuantExt {

FxBsPiecewiseConstantParametrization::FxBsPiecewiseConstantParametrization(const Currency& currency,
                                                                           const Handle<Quote>& fxSpotToday,
                                                                           const Array& times, const Array& sigma,
    const QuantLib::ext::shared_ptr<Constraint>& sigmaConstraint)
    : FxBsParametrization(currency, fxSpotToday),
      PiecewiseConstantHelper1(times, sigmaConstraint) {
    initialize(sigma);
}

FxBsPiecewiseConstantParametrization::FxBsPiecewiseConstantParametrization(
    const Currency& currency, const Handle<Quote>& fxSpotToday, const std::vector<Date>& dates, const Array& sigma,
    const Handle<YieldTermStructure>& domesticTermStructure,
    const QuantLib::ext::shared_ptr<Constraint>& sigmaConstraint)
    : FxBsParametrization(currency, fxSpotToday),
      PiecewiseConstantHelper1(dates, domesticTermStructure, sigmaConstraint) {
    initialize(sigma);
}

void FxBsPiecewiseConstantParametrization::initialize(const Array& sigma) {
    QL_REQUIRE(PiecewiseConstantHelper1::t().size() + 1 == sigma.size(),
               "alpha size (" << sigma.size() << ") inconsistent to times size ("
                              << PiecewiseConstantHelper1::t().size() << ")");

    // store raw parameter values
    for (Size i = 0; i < PiecewiseConstantHelper1::y_->size(); ++i) {
        PiecewiseConstantHelper1::y_->setParam(i, inverse(0, sigma[i]));
    }
    update();
}

} // namespace QuantExt
