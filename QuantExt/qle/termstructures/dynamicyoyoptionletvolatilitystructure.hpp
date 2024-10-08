/*
Copyright (C) 2019 Quaternion Risk Management Ltd
 Copyright (C) 2024 Growth Mindset Pty Ltd
All rights reserved.

This file is part of ORE, a free-software/open-source library
for transparent pricing and risk analysis - http://opensourcerisk.org

ORE is free software: you can redistribute it and/or modify it
under the terms of the Modified BSD License.  You should have received a
copy of the license along with this program.
The license is also available online at <http://opensourcerisk.org>

This program is distributed on the basis that it will form a useful
contribution to risk analytics and model standardisation, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
*/

/*! \file qle/termstructures/dynamicyoyoptionletvolatilitystructure.hpp
\brief dynamic yoy inflation optionlet volatility structure
\ingroup termstructures
*/

#pragma once

#include <boost/make_shared.hpp>
#include <ql/termstructures/volatility/inflation/yoyinflationoptionletvolatilitystructure.hpp>
#include <ql/termstructures/volatility/smilesection.hpp>
#include <qle/termstructures/dynamicstype.hpp>

namespace QuantExt {
using namespace QuantLib;

//! Converts YoYOptionletVolatilitySurface with fixed reference date into a floating reference date term structure.
/*! Different ways of reacting to time decay can be specified.

\warning No checks are performed that the supplied YoYOptionletVolatilitySurface has a fixed reference date

\ingroup termstructures
*/

class DynamicYoYOptionletVolatilitySurface : public YoYOptionletVolatilitySurface {
public:
    DynamicYoYOptionletVolatilitySurface(const QuantLib::ext::shared_ptr<YoYOptionletVolatilitySurface>& source,
                                         ReactionToTimeDecay decayMode = ConstantVariance);

protected:
    Volatility volatilityImpl(Time optionTime, Rate strike) const override;
    Rate minStrike() const override;
    Rate maxStrike() const override;
    Date maxDate() const override;

private:
    const QuantLib::ext::shared_ptr<YoYOptionletVolatilitySurface> source_;
    ReactionToTimeDecay decayMode_;
    const Date originalReferenceDate_;
};

} // namespace QuantExt
