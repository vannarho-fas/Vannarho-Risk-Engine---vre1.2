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

/*! \file blackvariancecurve3.hpp
    \brief Black volatility curve modeled as variance curve
    \ingroup termstructures
*/

#ifndef quantext_black_variance_curve_3_hpp
#define quantext_black_variance_curve_3_hpp

#include <ql/math/interpolation.hpp>
#include <ql/patterns/lazyobject.hpp>
#include <ql/quote.hpp>
#include <ql/termstructures/volatility/equityfx/blackvoltermstructure.hpp>

namespace QuantExt {
using namespace QuantLib;

//! Black volatility curve modeled as variance curve
/*! This class calculates time-dependent Black volatilities using
    as input a vector of (ATM) Black volatilities observed in the
    market.

    The calculation is performed interpolating on the variance curve.
    Linear interpolation is used.

    \todo check time extrapolation

    \ingroup termstructures
*/
class BlackVarianceCurve3 : public LazyObject, public BlackVarianceTermStructure {
public:
    // If requireMonotoneVariance is true, we check for monotone variance in performCalculations and throw if not
    BlackVarianceCurve3(Natural settlementDays, const Calendar& cal, BusinessDayConvention bdc, const DayCounter& dc,
                        const std::vector<Time>& times, const std::vector<Handle<Quote> >& blackVolCurve,
                        bool requireMonotoneVariance = true);
    //! \name TermStructure interface
    //@{
    Date maxDate() const override;
    //@}
    //! \name VolatilityTermStructure interface
    //@{
    Real minStrike() const override;
    Real maxStrike() const override;
    //@}
    //! \name Observer interface
    //@{
    void update() override;
    //@}
    //! \name LazyObject interface
    //@{
    void performCalculations() const override;
    //@}
    //! \name Visitability
    //@{
    virtual void accept(AcyclicVisitor&) override;
    //@}
protected:
    virtual Real blackVarianceImpl(Time t, Real) const override;

private:
    std::vector<Time> times_;
    std::vector<Handle<Quote> > quotes_;
    mutable std::vector<Real> variances_;
    mutable Interpolation varianceCurve_;
    bool requireMonotoneVariance_;
};

// inline definitions

inline Date BlackVarianceCurve3::maxDate() const { return Date::maxDate(); }

inline Real BlackVarianceCurve3::minStrike() const { return QL_MIN_REAL; }

inline Real BlackVarianceCurve3::maxStrike() const { return QL_MAX_REAL; }

inline void BlackVarianceCurve3::accept(AcyclicVisitor& v) {
    Visitor<BlackVarianceCurve3>* v1 = dynamic_cast<Visitor<BlackVarianceCurve3>*>(&v);
    if (v1 != 0)
        v1->visit(*this);
    else
        BlackVarianceTermStructure::accept(v);
}
} // namespace QuantExt

#endif
