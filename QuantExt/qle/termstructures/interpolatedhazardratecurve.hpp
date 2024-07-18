/*
 Copyright (C) 2016 Quaternion Risk Management Ltd
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
 Copyright (C) 2008 Jose Aparicio
 Copyright (C) 2008 Chris Kenyon
 Copyright (C) 2008 Roland Lichters
 Copyright (C) 2008, 2009 StatPro Italia srl

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

/*! \file interpolatedhazardratecurve.hpp
    \brief interpolated hazard-rate term structure (with the option to disable
           the negative rates check)
*/

#ifndef quantext_interpolated_hazard_rate_curve_hpp
#define quantext_interpolated_hazard_rate_curve_hpp

#include <ql/termstructures/credit/hazardratestructure.hpp>
#include <ql/termstructures/interpolatedcurve.hpp>
#include <utility>


namespace QuantExt {
using namespace QuantLib;

    //! DefaultProbabilityTermStructure based on interpolation of hazard rates
    /*! This is an exact copy of the QuantLib class, but with the option to
        disabled the check for non negative input hazard rates */
    /*! \ingroup defaultprobabilitytermstructures */
    template <class Interpolator>
    class InterpolatedHazardRateCurve
        : public HazardRateStructure,
          protected InterpolatedCurve<Interpolator> {
      public:
        InterpolatedHazardRateCurve(
            const std::vector<Date>& dates,
            const std::vector<Rate>& hazardRates,
            const DayCounter& dayCounter,
            const Calendar& cal = Calendar(),
            const std::vector<Handle<Quote> >& jumps =
                                                std::vector<Handle<Quote> >(),
            const std::vector<Date>& jumpDates = std::vector<Date>(),
            const Interpolator& interpolator = Interpolator(),
            const bool allowNegativeRates = false);
        InterpolatedHazardRateCurve(
            const std::vector<Date>& dates,
            const std::vector<Rate>& hazardRates,
            const DayCounter& dayCounter,
            const Calendar& calendar,
            const Interpolator& interpolator,
            const bool allowNegativeRates = false);
        InterpolatedHazardRateCurve(
            const std::vector<Date>& dates,
            const std::vector<Rate>& hazardRates,
            const DayCounter& dayCounter,
            const Interpolator& interpolator,
            const bool allowNegativeRates = false);
        //! \name TermStructure interface
        //@{
        Date maxDate() const override;
        //@}
        //! \name other inspectors
        //@{
        const std::vector<Time>& times() const;
        const std::vector<Date>& dates() const;
        const std::vector<Real>& data() const;
        const std::vector<Rate>& hazardRates() const;
        std::vector<std::pair<Date, Real> > nodes() const;
        //@}
      protected:
        InterpolatedHazardRateCurve(
            const DayCounter&,
            const std::vector<Handle<Quote> >& jumps = std::vector<Handle<Quote> >(),
            const std::vector<Date>& jumpDates = std::vector<Date>(),
            const Interpolator& interpolator = Interpolator(),
            const bool allowNegativeRates = false);
        InterpolatedHazardRateCurve(
            const Date& referenceDate,
            const DayCounter&,
            const std::vector<Handle<Quote> >& jumps = std::vector<Handle<Quote> >(),
            const std::vector<Date>& jumpDates = std::vector<Date>(),
            const Interpolator& interpolator = Interpolator(),
            const bool allowNegativeRates = false);
        InterpolatedHazardRateCurve(
            Natural settlementDays,
            const Calendar&,
            const DayCounter&,
            const std::vector<Handle<Quote> >& jumps = std::vector<Handle<Quote> >(),
            const std::vector<Date>& jumpDates = std::vector<Date>(),
            const Interpolator& interpolator = Interpolator(),
            const bool allowNegativeRates = false);
        //! \name DefaultProbabilityTermStructure implementation
        //@{
        Real hazardRateImpl(Time) const override;
        Probability survivalProbabilityImpl(Time) const override;
        //@}
        mutable std::vector<Date> dates_;
      private:
        void initialize();
        bool allowNegativeRates_;
    };


    // inline definitions

    template <class T>
    inline Date InterpolatedHazardRateCurve<T>::maxDate() const {
        return dates_.back();
    }

    template <class T>
    inline const std::vector<Time>&
    InterpolatedHazardRateCurve<T>::times() const {
        return this->times_;
    }

    template <class T>
    inline const std::vector<Date>&
    InterpolatedHazardRateCurve<T>::dates() const {
        return dates_;
    }

    template <class T>
    inline const std::vector<Real>&
    InterpolatedHazardRateCurve<T>::data() const {
        return this->data_;
    }

    template <class T>
    inline const std::vector<Rate>&
    InterpolatedHazardRateCurve<T>::hazardRates() const {
        return this->data_;
    }

    template <class T>
    inline std::vector<std::pair<Date, Real> >
    InterpolatedHazardRateCurve<T>::nodes() const {
        std::vector<std::pair<Date, Real> > results(dates_.size());
        for (Size i=0; i<dates_.size(); ++i)
            results[i] = std::make_pair(dates_[i], this->data_[i]);
        return results;
    }

    #ifndef __DOXYGEN__

    // template definitions

    template <class T>
    Real InterpolatedHazardRateCurve<T>::hazardRateImpl(Time t) const {
        if (t <= this->times_.back())
            return this->interpolation_(t, true);

        // flat hazard rate extrapolation
        return this->data_.back();
    }

    template <class T>
    Probability
    InterpolatedHazardRateCurve<T>::survivalProbabilityImpl(Time t) const {
        if (t == 0.0)
            return 1.0;

        Real integral;
        if (t <= this->times_.back()) {
            integral = this->interpolation_.primitive(t, true);
        } else {
            // flat hazard rate extrapolation
            integral = this->interpolation_.primitive(this->times_.back(), true)
                     + this->data_.back()*(t - this->times_.back());
        }
        return std::exp(-integral);
    }

    template <class T>
    InterpolatedHazardRateCurve<T>::InterpolatedHazardRateCurve(
                                    const DayCounter& dayCounter,
                                    const std::vector<Handle<Quote> >& jumps,
                                    const std::vector<Date>& jumpDates,
                                    const T& interpolator,
                                    const bool allowNegativeRates)
    : HazardRateStructure(dayCounter, jumps, jumpDates),
      InterpolatedCurve<T>(interpolator), allowNegativeRates_(allowNegativeRates) {}

    template <class T>
    InterpolatedHazardRateCurve<T>::InterpolatedHazardRateCurve(
                                    const Date& referenceDate,
                                    const DayCounter& dayCounter,
                                    const std::vector<Handle<Quote> >& jumps,
                                    const std::vector<Date>& jumpDates,
                                    const T& interpolator,
                                    const bool allowNegativeRates)
    : HazardRateStructure(referenceDate, Calendar(), dayCounter, jumps, jumpDates),
      InterpolatedCurve<T>(interpolator), allowNegativeRates_(allowNegativeRates) {}

    template <class T>
    InterpolatedHazardRateCurve<T>::InterpolatedHazardRateCurve(
                                    Natural settlementDays,
                                    const Calendar& calendar,
                                    const DayCounter& dayCounter,
                                    const std::vector<Handle<Quote> >& jumps,
                                    const std::vector<Date>& jumpDates,
                                    const T& interpolator,
                                    const bool allowNegativeRates)
    : HazardRateStructure(settlementDays, calendar, dayCounter, jumps, jumpDates),
      InterpolatedCurve<T>(interpolator), allowNegativeRates_(allowNegativeRates) {}

    template <class T>
    InterpolatedHazardRateCurve<T>::InterpolatedHazardRateCurve(
                                    const std::vector<Date>& dates,
                                    const std::vector<Rate>& hazardRates,
                                    const DayCounter& dayCounter,
                                    const Calendar& calendar,
                                    const std::vector<Handle<Quote> >& jumps,
                                    const std::vector<Date>& jumpDates,
                                    const T& interpolator,
                                    const bool allowNegativeRates)
    : HazardRateStructure(dates.at(0), calendar, dayCounter, jumps, jumpDates),
      InterpolatedCurve<T>(std::vector<Time>(), hazardRates, interpolator),
      dates_(dates), allowNegativeRates_(allowNegativeRates)
    {
        initialize();
    }

    template <class T>
    InterpolatedHazardRateCurve<T>::InterpolatedHazardRateCurve(
            const std::vector<Date>& dates,
            const std::vector<Rate>& hazardRates,
            const DayCounter& dayCounter,
            const Calendar& calendar,
            const T& interpolator,
            const bool allowNegativeRates)
    : HazardRateStructure(dates.at(0), calendar, dayCounter),
      InterpolatedCurve<T>(std::vector<Time>(), hazardRates, interpolator),
      dates_(dates), allowNegativeRates_(allowNegativeRates)
    {
        initialize();
    }

    template <class T>
    InterpolatedHazardRateCurve<T>::InterpolatedHazardRateCurve(
            const std::vector<Date>& dates,
            const std::vector<Rate>& hazardRates,
            const DayCounter& dayCounter,
            const T& interpolator,
            const bool allowNegativeRates)
    : HazardRateStructure(dates.at(0), Calendar(), dayCounter),
      InterpolatedCurve<T>(std::vector<Time>(), hazardRates, interpolator),
      dates_(dates), allowNegativeRates_(allowNegativeRates)
    {
        initialize();
    }

    #endif

    template <class T>
    void InterpolatedHazardRateCurve<T>::initialize()
    {
        QL_REQUIRE(dates_.size() >= T::requiredPoints,
                   "not enough input dates given");
        QL_REQUIRE(this->data_.size() == dates_.size(),
                   "dates/data count mismatch");

        this->times_.resize(dates_.size());
        this->times_[0] = 0.0;
        for (Size i=1; i<dates_.size(); ++i) {
            QL_REQUIRE(dates_[i] > dates_[i-1],
                       "invalid date (" << dates_[i] << ", vs "
                       << dates_[i-1] << ")");
            this->times_[i] = dayCounter().yearFraction(dates_[0], dates_[i]);
            QL_REQUIRE(!close(this->times_[i], this->times_[i-1]),
                       "two dates correspond to the same time "
                       "under this curve's day count convention");
            QL_REQUIRE(allowNegativeRates_ || this->data_[i] >= 0.0, "negative hazard rate");
        }

        this->interpolation_ =
            this->interpolator_.interpolate(this->times_.begin(),
                                            this->times_.end(),
                                            this->data_.begin());
        this->interpolation_.update();
    }

}

#endif
