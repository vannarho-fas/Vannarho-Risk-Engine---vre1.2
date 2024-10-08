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

/*! \file averageonindexedcouponpricer.hpp
    \brief Pricer for average overnight indexed coupons

        \ingroup cashflows
*/

#ifndef quantext_average_on_indexed_coupon_pricer_hpp
#define quantext_average_on_indexed_coupon_pricer_hpp

#include <qle/cashflows/averageonindexedcoupon.hpp>

#include <ql/cashflows/couponpricer.hpp>

namespace QuantExt {
using namespace QuantLib;

//! Pricer for average overnight indexed coupons
/*! \ingroup cashflows
 */
class AverageONIndexedCouponPricer : public FloatingRateCouponPricer {
public:
    enum Approximation { Takada, None };

    AverageONIndexedCouponPricer(Approximation approxType = Takada) : approximationType_(approxType) {}

    void initialize(const FloatingRateCoupon& coupon) override;
    Rate swapletRate() const override;

    Real swapletPrice() const override { QL_FAIL("swapletPrice not available"); }
    Real capletPrice(Rate) const override { QL_FAIL("capletPrice not available"); }
    Rate capletRate(Rate) const override { QL_FAIL("capletRate not available"); }
    Real floorletPrice(Rate) const override { QL_FAIL("floorletPrice not available"); }
    Rate floorletRate(Rate) const override { QL_FAIL("floorletRate not available"); }

protected:
    Approximation approximationType_;
    Real gearing_;
    Spread spread_;
    Time accrualPeriod_;
    QuantLib::ext::shared_ptr<OvernightIndex> overnightIndex_;

    const AverageONIndexedCoupon* coupon_;
};
} // namespace QuantExt

#endif
