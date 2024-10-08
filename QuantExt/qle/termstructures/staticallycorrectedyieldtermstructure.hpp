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

/*! \file staticallycorrectedyieldtermstructure.hpp
    \brief Statically corrected yield term structure
    \ingroup termstructures
*/

#ifndef quantext_statically_corrected_yts_hpp
#define quantext_statically_corrected_yts_hpp

#include <qle/termstructures/dynamicstype.hpp>

#include <ql/termstructures/yieldtermstructure.hpp>

#include <boost/unordered_map.hpp>

namespace QuantExt {
using namespace QuantLib;

//! Statically Corrected Yield Term Structure
/*! This termstructure takes a floating reference date term structure
    and two fixed reference date term structures, applying a static
    correction to the floating ts implied by the two fixed ones.
    Usually the floating term structure will coincide with
    the first fixed at construction time. Also, the two fixed
    termstructures should have the same reference date and all three
    termstructures should have the same day counter.

    \ingroup termstructures
 */
class StaticallyCorrectedYieldTermStructure : public YieldTermStructure {
public:
    StaticallyCorrectedYieldTermStructure(const Handle<YieldTermStructure>& floatingTermStructure,
                                          const Handle<YieldTermStructure>& fixedSourceTermStructure,
                                          const Handle<YieldTermStructure>& fixedTargetTermStructure,
                                          const YieldCurveRollDown& rollDown = ForwardForward)
        : YieldTermStructure(floatingTermStructure->dayCounter()), x_(floatingTermStructure),
          source_(fixedSourceTermStructure), target_(fixedTargetTermStructure), rollDown_(rollDown) {
        registerWith(floatingTermStructure);
        registerWith(fixedSourceTermStructure);
        registerWith(fixedTargetTermStructure);
    }

    Date maxDate() const override { return x_->maxDate(); }
    void update() override {}
    const Date& referenceDate() const override { return x_->referenceDate(); }

    Calendar calendar() const override { return x_->calendar(); }
    Natural settlementDays() const override { return x_->settlementDays(); }

    void flushCache() { cache_c_.clear(); }

protected:
    Real discountImpl(Time t) const override;

private:
    // FIXME: remove cache
    // cache for source and target forwards
    struct cache_key {
        double t0, t;
        bool operator==(const cache_key& o) const { return (t0 == o.t0) && (t == o.t); }
    };
    struct cache_hasher {
        std::size_t operator()(cache_key const& x) const {
            std::size_t seed = 0;
            boost::hash_combine(seed, x.t0);
            boost::hash_combine(seed, x.t);
            return seed;
        }
    };
    mutable boost::unordered_map<cache_key, Real, cache_hasher> cache_c_;
    // end cache
    const Handle<YieldTermStructure> x_, source_, target_;
    const YieldCurveRollDown rollDown_;
};

// inline

inline Real StaticallyCorrectedYieldTermStructure::discountImpl(Time t) const {
    Real c = 1.0;
    if (rollDown_ == ForwardForward) {
        Real t0 = source_->timeFromReference(referenceDate());
        // roll down = ForwardForward
        // cache lookup
        cache_key k = { t0, t };
        boost::unordered_map<cache_key, Real>::const_iterator i = cache_c_.find(k);
        if (i == cache_c_.end()) {
            c = source_->discount(t0) / source_->discount(t0 + t) * target_->discount(t0 + t) / target_->discount(t0);
            cache_c_.insert(std::make_pair(k, c));
        } else {
            c = i->second;
        }
    } else {
        // roll down = ConstantDiscount
        // cache lookup
        cache_key k = { 0.0, t };
        boost::unordered_map<cache_key, Real>::const_iterator i = cache_c_.find(k);
        if (i == cache_c_.end()) {
            c = target_->discount(t) / source_->discount(t);
            cache_c_.insert(std::make_pair(k, c));
        } else {
            c = i->second;
        }
    }
    return x_->discount(t) * c;
}

} // namespace QuantExt

#endif
