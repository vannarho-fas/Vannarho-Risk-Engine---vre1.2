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

/*! \file qle/models/crossassetmodelimpliedfxvoltermstructure.hpp
    \brief dynamic black volatility term structure
    \ingroup crossassetmodel
*/

#ifndef quantext_crossassetmodel_implied_fx_volatility_termstructure_hpp
#define quantext_crossassetmodel_implied_fx_volatility_termstructure_hpp

#include <qle/models/crossassetmodel.hpp>

#include <ql/termstructures/volatility/equityfx/blackvoltermstructure.hpp>

namespace QuantExt {
using namespace QuantLib;

//! Analytic Cross Currency CC LGM FX Option Enfine
/*! \ingroup crossassetmodel
 */
class AnalyticCcLgmFxOptionEngine;

//! Cross Asset Model Implied FX Term Structure
/*! The termstructure as the reference date of the model at construction,
  you can vary this and the relevant state variables using the state() and
  move() methods.

  \ingroup crossassetmodel
*/

/* TODO probably slow, we need to introduce a cache */

class CrossAssetModelImpliedFxVolTermStructure : public BlackVolTermStructure {
public:
    CrossAssetModelImpliedFxVolTermStructure(const QuantLib::ext::shared_ptr<CrossAssetModel>& model,
                                             const Size foreignCurrencyIndex, BusinessDayConvention bdc = Following,
                                             const DayCounter& dc = DayCounter(), const bool purelyTimeBased = false);

    void referenceDate(const Date& d);
    void referenceTime(const Time t);
    void state(const Real domesticIr, const Real foreignIr, const Real logFx);
    void move(const Date& d, const Real domesticIr, const Real foreignIr, const Real logFx);
    void move(const Time t, const Real domesticIr, const Real foreignIr, const Real logFx);

    /* VolatilityTermStructure interface */
    Real minStrike() const override;
    Real maxStrike() const override;
    /* TermStructure interface */
    Date maxDate() const override;
    Time maxTime() const override;
    const Date& referenceDate() const override;
    /* Observer interface */
    void update() override;

    Size fxIndex() const { return fxIndex_; }

protected:
    /* BlackVolTermStructure interface */
    Real blackVarianceImpl(Time t, Real strike) const override;
    Volatility blackVolImpl(Time t, Real strike) const override;

private:
    const QuantLib::ext::shared_ptr<CrossAssetModel> model_;
    const Size fxIndex_;
    const bool purelyTimeBased_;
    const QuantLib::ext::shared_ptr<AnalyticCcLgmFxOptionEngine> engine_;
    Date referenceDate_;
    Real relativeTime_, irDom_, irFor_, fx_;
};

} // namespace QuantExt

#endif
