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

/*! \file interpolateddiscountcurve.hpp
    \brief interpolated discount term structure
    \ingroup termstructures
*/

#ifndef quantext_interpolated_discount_curve_hpp
#define quantext_interpolated_discount_curve_hpp

#include <boost/make_shared.hpp>
#include <ql/termstructures/yieldtermstructure.hpp>
#include <qle/quotes/logquote.hpp>

namespace QuantExt {
using namespace QuantLib;

//! InterpolatedDiscountCurve based on loglinear interpolation of DiscountFactors
/*! InterpolatedDiscountCurve based on loglinear interpolation of DiscountFactors,
    flat fwd extrapolation is always enabled, the term structure has always a
    floating reference date

        \ingroup termstructures
    */
class InterpolatedDiscountCurve : public YieldTermStructure {
public:
    enum class Interpolation { logLinear, linearZero };
    enum class Extrapolation { flatFwd, flatZero };
    //! \name Constructors
    //@{
    //! default constructor
    InterpolatedDiscountCurve(const std::vector<Time>& times, const std::vector<Handle<Quote>>& quotes,
                              const Natural settlementDays, const Calendar& cal, const DayCounter& dc,
                              const Interpolation interpolation = Interpolation::logLinear,
                              const Extrapolation extrapolation = Extrapolation::flatFwd)
        : YieldTermStructure(settlementDays, cal, dc), times_(times), interpolation_(interpolation),
          extrapolation_(extrapolation) {
        initalise(quotes);
    }

    //! constructor that takes a vector of dates
    InterpolatedDiscountCurve(const std::vector<Date>& dates, const std::vector<Handle<Quote>>& quotes,
                              const Natural settlementDays, const Calendar& cal, const DayCounter& dc,
                              const Interpolation interpolation = Interpolation::logLinear,
                              const Extrapolation extrapolation = Extrapolation::flatFwd)
        : YieldTermStructure(settlementDays, cal, dc), times_(dates.size()), interpolation_(interpolation),
          extrapolation_(extrapolation) {
        for (Size i = 0; i < dates.size(); ++i)
            times_[i] = timeFromReference(dates[i]);
        initalise(quotes);
    }
    //@}

private:
    void initalise(const std::vector<Handle<Quote>>& quotes) {
        QL_REQUIRE(times_.size() > 1, "at least two times required");
        QL_REQUIRE(times_[0] == 0.0, "First time must be 0, got " << times_[0]); // or date=asof
        QL_REQUIRE(times_.size() == quotes.size(), "size of time and quote vectors do not match");
        for (Size i = 0; i < quotes.size(); ++i) {
            quotes_.push_back(QuantLib::ext::make_shared<LogQuote>(quotes[i]));
        }
        for (Size i = 0; i < times_.size() - 1; ++i)
            timeDiffs_.push_back(times_[i + 1] - times_[i]);
    }

    //! \name TermStructure interface
    //@{
    Date maxDate() const override { return Date::maxDate(); } // flat fwd extrapolation
    //@}

protected:
    DiscountFactor discountImpl(Time t) const override {
        if (t > this->times_.back() && extrapolation_ == Extrapolation::flatZero) {
            Real tMax = this->times_.back();
            Real dMax = std::exp(quotes_.back()->value());
            return std::pow(dMax, t / tMax);
        }
        std::vector<Time>::const_iterator it = std::upper_bound(times_.begin(), times_.end(), t);
        Size i = std::min<Size>(it - times_.begin(), times_.size() - 1);
        Real weight = (times_[i] - t) / timeDiffs_[i - 1];
        if (interpolation_ == Interpolation::logLinear || t > this->times_.back()) {
            // this handles flat fwd extrapolation (t > times.back()) as well
            Real value = (1.0 - weight) * quotes_[i]->value() + weight * quotes_[i - 1]->value();
            return ::exp(value);
        } else {
            Real value =
                (1.0 - weight) * quotes_[i]->value() / times_[i] + weight * quotes_[i - 1]->value() / times_[i - 1];
            return ::exp(t * value);
        }
    }

private:
    std::vector<Time> times_;
    std::vector<Time> timeDiffs_;
    std::vector<QuantLib::ext::shared_ptr<Quote>> quotes_;
    Interpolation interpolation_;
    Extrapolation extrapolation_;
};

} // namespace QuantExt

#endif
