/*
 Copyright (C) 2020 Quaternion Risk Management Ltd
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

/*! \file ored/portfolio/builders/capflooredovernightindexedcouponleg.hpp
    \brief builder that returns an engine to price capped floored ibor legs
    \ingroup builders
*/

#pragma once

#include <ored/portfolio/builders/cachingenginebuilder.hpp>
#include <ored/portfolio/enginefactory.hpp>

#include <ql/cashflows/couponpricer.hpp>

namespace ore {
namespace data {

//! CouponPricer Builder for CapFlooredOvernightIndexedCouponLeg
/*! The coupon pricers are cached by index / rate comp period
 \ingroup builders
 */
class CapFlooredOvernightIndexedCouponLegEngineBuilder
    : public CachingCouponPricerBuilder<string, const std::string&, const QuantLib::Period&> {
public:
    CapFlooredOvernightIndexedCouponLegEngineBuilder()
        : CachingEngineBuilder("BlackOrBachelier", "BlackOvernightIndexedCouponPricer",
                               {"CapFlooredOvernightIndexedCouponLeg"}) {}

protected:
    string keyImpl(const string& index, const QuantLib::Period& rateComputationPeriod) override;
    QuantLib::ext::shared_ptr<FloatingRateCouponPricer> engineImpl(const string& index,
                                                           const QuantLib::Period& rateComputationPeriod) override;
};
} // namespace data
} // namespace ore
