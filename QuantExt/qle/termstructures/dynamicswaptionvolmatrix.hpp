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

/*! \file termstructures/dynamicswaptionvolmatrix.hpp
    \brief dynamic swaption volatility matrix
    \ingroup termstructures
*/

#ifndef quantext_dynamic_swaption_volatility_termstructure_hpp
#define quantext_dynamic_swaption_volatility_termstructure_hpp

#include <qle/termstructures/dynamicstype.hpp>

#include <ql/termstructures/volatility/swaption/swaptionvolmatrix.hpp>

#include <boost/make_shared.hpp>

namespace QuantExt {
using namespace QuantLib;
//! Takes a SwaptionVolatilityMatrix with fixed reference date and turns it into a floating reference date term
// structure.
/*! This class takes a SwaptionVolatilityMatrix with fixed reference date and turns it into a floating reference date
    term structure. There are different ways of reacting to time decay that can be specified.

    \warning the vols from the source ts are read using strike null (indicating atm)

    \ingroup termstructures
*/

class DynamicSwaptionVolatilityMatrix : public SwaptionVolatilityStructure {
public:
    DynamicSwaptionVolatilityMatrix(const QuantLib::ext::shared_ptr<SwaptionVolatilityStructure>& source,
                                    Natural settlementDays, const Calendar& calendar,
                                    ReactionToTimeDecay decayMode = ConstantVariance);

protected:
    /* SwaptionVolatilityStructure interface */
    const Period& maxSwapTenor() const override;

    QuantLib::ext::shared_ptr<SmileSection> smileSectionImpl(Time optionTime, Time swapLength) const override;

    Volatility volatilityImpl(Time optionTime, Time swapLength, Rate strike) const override;

    Real shiftImpl(Time optionTime, Time swapLength) const override;

    /* VolatilityTermStructure interface */
    Real minStrike() const override;
    Real maxStrike() const override;
    /* TermStructure interface */
    Date maxDate() const override;
    /* Observer interface */
    void update() override;

    VolatilityType volatilityType() const override;

private:
    const QuantLib::ext::shared_ptr<SwaptionVolatilityStructure> source_;
    ReactionToTimeDecay decayMode_;
    const Date originalReferenceDate_;
    VolatilityType volatilityType_;
};

inline VolatilityType DynamicSwaptionVolatilityMatrix::volatilityType() const { return volatilityType_; }

} // namespace QuantExt

#endif
