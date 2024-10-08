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

/*! \file qle/cashflows/couponpricer.hpp
    \brief Utility functions for setting coupon pricers on legs
    \ingroup cashflows
*/

#ifndef quantext_coupon_pricer_hpp
#define quantext_coupon_pricer_hpp

#include <ql/shared_ptr.hpp>

#include <ql/cashflows/couponpricer.hpp>

namespace QuantExt {
using namespace QuantLib;
/*! \addtogroup cashflows
    @{
*/
//! Set Coupon Pricer
void setCouponPricer(const Leg& leg, const QuantLib::ext::shared_ptr<FloatingRateCouponPricer>&);
//! Set Coupon Pricers
void setCouponPricers(const Leg& leg, const std::vector<QuantLib::ext::shared_ptr<FloatingRateCouponPricer> >&);

// @}
} // namespace QuantExt

#endif
