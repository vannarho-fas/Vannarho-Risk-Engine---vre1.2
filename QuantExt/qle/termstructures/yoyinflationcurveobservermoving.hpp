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

/*! \file qle/termstructures/yoyinflationcurveobservermoving.hpp
    \brief Observable inflation term structure with floating reference date
           based on the interpolation of zero rate quotes.
    \ingroup termstructures
*/

#ifndef quantext_yoy_inflation_curve_observer_moving_hpp
#define quantext_yoy_inflation_curve_observer_moving_hpp

#include <ql/math/comparison.hpp>
#include <ql/math/interpolations/linearinterpolation.hpp>
#include <ql/patterns/lazyobject.hpp>
#include <ql/termstructures/inflationtermstructure.hpp>
#include <ql/termstructures/interpolatedcurve.hpp>

namespace QuantExt {
using namespace QuantLib;

//! Inflation term structure based on the interpolation of zero rates.
/*! \ingroup inflationtermstructures */
template <class Interpolator>
class YoYInflationCurveObserverMoving : public YoYInflationTermStructure,
                                        protected InterpolatedCurve<Interpolator>,
                                        public LazyObject {
public:
    YoYInflationCurveObserverMoving(
        Natural settlementDays, const Calendar& calendar, const DayCounter& dayCounter, const Period& lag,
        Frequency frequency, bool indexIsInterpolated, const std::vector<Time>& times,
        const std::vector<Handle<Quote>>& rates,
        const QuantLib::ext::shared_ptr<Seasonality>& seasonality = QuantLib::ext::shared_ptr<Seasonality>(),
        const Interpolator& interpolator = Interpolator());

    //! \name InflationTermStructure interface
    //@{
    Date baseDate() const override;
    Time maxTime() const override;
    Date maxDate() const override;
    //@}

    //! \name Inspectors
    //@{
    const std::vector<Time>& times() const;
    const std::vector<Real>& data() const;
    const std::vector<Rate>& rates() const;
    // std::vector<std::pair<Date, Rate> > nodes() const;
    const std::vector<Handle<Quote> >& quotes() const { return quotes_; };
    //@}

    //! \name Observer interface
    //@{
    void update() override;
    //@}

private:
    //! \name LazyObject interface
    //@{
    void performCalculations() const override;
    //@}

protected:
    //! \name YoYInflationTermStructure Interface
    //@{
    Rate yoyRateImpl(Time t) const override;
    //@}
    std::vector<Handle<Quote> > quotes_;
    bool indexIsInterpolated_;
    mutable Date baseDate_;
};

// template definitions

template <class Interpolator>
YoYInflationCurveObserverMoving<Interpolator>::YoYInflationCurveObserverMoving(
    Natural settlementDays, const Calendar& calendar, const DayCounter& dayCounter, const Period& lag,
    Frequency frequency, bool indexIsInterpolated, const std::vector<Time>& times,
    const std::vector<Handle<Quote>>& rates, const QuantLib::ext::shared_ptr<Seasonality>& seasonality,
    const Interpolator& interpolator)
    : YoYInflationTermStructure(settlementDays, calendar, dayCounter, rates[0]->value(), lag, frequency, indexIsInterpolated, seasonality),
      InterpolatedCurve<Interpolator>(std::vector<Time>(), std::vector<Real>(), interpolator), quotes_(rates), indexIsInterpolated_(indexIsInterpolated) {

    QL_REQUIRE(times.size() > 1, "too few times: " << times.size());
    this->times_.resize(times.size());
    this->times_[0] = times[0];
    for (Size i = 1; i < times.size(); i++) {
        QL_REQUIRE(times[i] > times[i - 1], "times not sorted");
        this->times_[i] = times[i];
    }

    QL_REQUIRE(this->quotes_.size() == this->times_.size(),
               "quotes/times count mismatch: " << this->quotes_.size() << " vs " << this->times_.size());

    // initialise data vector, values are copied from quotes in performCalculations()
    this->data_.resize(this->times_.size());
    for (Size i = 0; i < this->times_.size(); i++)
        this->data_[0] = 0.0;

    this->interpolation_ =
        this->interpolator_.interpolate(this->times_.begin(), this->times_.end(), this->data_.begin());
    this->interpolation_.update();

    // register with each of the quotes
    for (Size i = 0; i < this->quotes_.size(); i++)
        registerWith(this->quotes_[i]);
}

template <class T> Date YoYInflationCurveObserverMoving<T>::baseDate() const {
    // if indexIsInterpolated we fixed the dates in the constructor
    calculate();
    return baseDate_;
}

template <class T> Time YoYInflationCurveObserverMoving<T>::maxTime() const { return this->times_.back(); }

template <class T> Date YoYInflationCurveObserverMoving<T>::maxDate() const { return this->maxDate_; }

template <class T> inline Rate YoYInflationCurveObserverMoving<T>::yoyRateImpl(Time t) const {
    calculate();
    return this->interpolation_(t, true);
}

template <class T> inline const std::vector<Time>& YoYInflationCurveObserverMoving<T>::times() const {
    return this->times_;
}

template <class T> inline const std::vector<Rate>& YoYInflationCurveObserverMoving<T>::rates() const {
    calculate();
    return this->data_;
}

template <class T> inline const std::vector<Real>& YoYInflationCurveObserverMoving<T>::data() const {
    calculate();
    return this->data_;
}

template <class T> inline void YoYInflationCurveObserverMoving<T>::update() {
    LazyObject::update();
    YoYInflationTermStructure::update();
}

template <class T> inline void YoYInflationCurveObserverMoving<T>::performCalculations() const {
    Date d = Settings::instance().evaluationDate();
    Date d0 = d - this->observationLag();
    if (!indexIsInterpolated_) {
        baseDate_ = inflationPeriod(d0, this->frequency_).first;
    } else {
        baseDate_ = d0;
    }

    for (Size i = 0; i < this->times_.size(); ++i)
        this->data_[i] = quotes_[i]->value();
    this->interpolation_ =
        this->interpolator_.interpolate(this->times_.begin(), this->times_.end(), this->data_.begin());
    this->interpolation_.update();
}
} // namespace QuantExt

#endif
