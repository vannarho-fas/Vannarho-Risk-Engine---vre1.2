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

#include <ql/math/comparison.hpp>
#include <qle/models/eqbsconstantparametrization.hpp>

namespace QuantExt {

EqBsConstantParametrization::EqBsConstantParametrization(const Currency& currency, const std::string& eqName,
                                                         const Handle<Quote>& eqSpotToday,
                                                         const Handle<Quote>& fxSpotToday, const Real sigma,
                                                         const Handle<YieldTermStructure>& eqIrCurveToday,
                                                         const Handle<YieldTermStructure>& eqDivYieldCurveToday)
    : EqBsParametrization(currency, eqName, eqSpotToday, fxSpotToday, eqIrCurveToday, eqDivYieldCurveToday),
      sigma_(QuantLib::ext::make_shared<PseudoParameter>(1)) {
    sigma_->setParam(0, inverse(0, sigma));
}

} // namespace QuantExt
