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

/*
 Copyright (C) 2006 Ferdinando Ametrano
 Copyright (C) 2006 Katiuscia Manzoni
 Copyright (C) 2015 Peter Caspers

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <qle/math/flatextrapolation.hpp>
#include <qle/termstructures/swaptionvolcube2.hpp>

#include <ql/math/interpolations/bilinearinterpolation.hpp>
#include <ql/math/interpolations/flatextrapolation2d.hpp>
#include <ql/termstructures/volatility/interpolatedsmilesection.hpp>

#include <ql/indexes/swapindex.hpp>
#include <ql/math/rounding.hpp>

namespace QuantExt {

SwaptionVolCube2::SwaptionVolCube2(const Handle<SwaptionVolatilityStructure>& atmVolStructure,
                                   const std::vector<Period>& optionTenors, const std::vector<Period>& swapTenors,
                                   const std::vector<Spread>& strikeSpreads,
                                   const std::vector<std::vector<Handle<Quote> > >& volSpreads,
                                   const QuantLib::ext::shared_ptr<SwapIndex>& swapIndexBase,
                                   const QuantLib::ext::shared_ptr<SwapIndex>& shortSwapIndexBase, bool vegaWeightedSmileFit,
                                   bool flatExtrapolation, bool volsAreSpreads)
    : SwaptionVolatilityCube(atmVolStructure, optionTenors, swapTenors, strikeSpreads, volSpreads, swapIndexBase,
                             shortSwapIndexBase, vegaWeightedSmileFit),
      flatExtrapolation_(flatExtrapolation), volsAreSpreads_(volsAreSpreads), volSpreadsInterpolator_(nStrikes_),
      volSpreadsMatrix_(nStrikes_, Matrix(optionTenors.size(), swapTenors.size(), 0.0)) {
    QL_REQUIRE(std::find(strikeSpreads_.begin(), strikeSpreads_.end(), 0.0) != strikeSpreads_.end(),
               "SwaptionVolCube2: strikeSpreads must contain 0.0 for atm vols");
}

void SwaptionVolCube2::performCalculations() const {

    SwaptionVolatilityCube::performCalculations();
    //! set volSpreadsMatrix_ by volSpreads_ quotes
    for (Size i = 0; i < nStrikes_; i++)
        for (Size j = 0; j < nOptionTenors_; j++)
            for (Size k = 0; k < nSwapTenors_; k++) {
                volSpreadsMatrix_[i][j][k] = volSpreads_[j * nSwapTenors_ + k][i]->value();
            }
    //! create volSpreadsInterpolator_
    for (Size i = 0; i < nStrikes_; i++) {
        if (!flatExtrapolation_)
            volSpreadsInterpolator_[i] =
                BilinearInterpolation(swapLengths_.begin(), swapLengths_.end(), optionTimes_.begin(),
                                      optionTimes_.end(), volSpreadsMatrix_[i]);
        else
            volSpreadsInterpolator_[i] = FlatExtrapolator2D(QuantLib::ext::make_shared<BilinearInterpolation>(
                swapLengths_.begin(), swapLengths_.end(), optionTimes_.begin(), optionTimes_.end(),
                volSpreadsMatrix_[i]));
        volSpreadsInterpolator_[i].enableExtrapolation();
    }
}

QuantLib::ext::shared_ptr<SmileSection> SwaptionVolCube2::smileSectionImpl(Time optionTime, Time swapLength) const {

    calculate();
    Date optionDate = optionDateFromTime(optionTime);
    Rounding rounder(0);
    Period swapTenor(static_cast<Integer>(rounder(swapLength * 12.0)), Months);
    // ensure that option date is valid fixing date
    optionDate = swapTenor > shortSwapIndexBase_->tenor()
                     ? swapIndexBase_->fixingCalendar().adjust(optionDate, Following)
                     : shortSwapIndexBase_->fixingCalendar().adjust(optionDate, Following);
    return smileSectionImpl(optionDate, swapTenor);
}

QuantLib::ext::shared_ptr<SmileSection> SwaptionVolCube2::smileSectionImpl(const Date& optionDate,
                                                                   const Period& swapTenor) const {
    calculate();
    Rate atmForward = atmStrike(optionDate, swapTenor);
    Volatility referenceVol = volsAreSpreads_ ? atmVol_->volatility(optionDate, swapTenor, atmForward) : 0.0;
    Time optionTime = std::max(1E-6, timeFromReference(optionDate));
    Real exerciseTimeSqrt = std::sqrt(optionTime);
    std::vector<Real> strikes, stdDevs;
    strikes.reserve(nStrikes_);
    stdDevs.reserve(nStrikes_);
    Time length = swapLength(swapTenor);
    for (Size i = 0; i < nStrikes_; ++i) {
        strikes.push_back(atmForward + strikeSpreads_[i]);
        stdDevs.push_back(exerciseTimeSqrt * (referenceVol + volSpreadsInterpolator_[i](length, optionTime)));
    }
    Real shift = atmVol_->shift(optionTime, length);
    if (!flatExtrapolation_)
        return QuantLib::ext::shared_ptr<SmileSection>(new InterpolatedSmileSection<Linear>(
            optionTime, strikes, stdDevs, atmForward, Linear(), Actual365Fixed(), volatilityType(), shift));
    else
        return QuantLib::ext::shared_ptr<SmileSection>(new InterpolatedSmileSection<LinearFlat>(
            optionTime, strikes, stdDevs, atmForward, LinearFlat(), Actual365Fixed(), volatilityType(), shift));
}
} // namespace QuantExt
