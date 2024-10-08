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

/*! \file qle/termstructures/spreadedoptionletvolatility.hpp
    \brief Adds floor to QuantLib::SpreadedOptionletVolatility
    \ingroup termstructures
*/

#ifndef quantext_spreaded_optionlet_volatility_h
#define quantext_spreaded_optionlet_volatility_h

#include <ql/termstructures/volatility/optionlet/spreadedoptionletvol.hpp>

namespace QuantExt {
using QuantLib::Date;
using QuantLib::Handle;
using QuantLib::OptionletVolatilityStructure;
using QuantLib::Quote;
using QuantLib::Rate;
using QuantLib::SmileSection;
using QuantLib::Time;
using QuantLib::Volatility;

class SpreadedOptionletVolatility : public QuantLib::SpreadedOptionletVolatility {
public:
    SpreadedOptionletVolatility(const Handle<OptionletVolatilityStructure>& baseVol, const Handle<Quote>& spread);

protected:
    //! \name OptionletVolatilityStructure interface
    //@{
    QuantLib::ext::shared_ptr<SmileSection> smileSectionImpl(const Date& d) const override;
    QuantLib::ext::shared_ptr<SmileSection> smileSectionImpl(Time optionT) const override;
    Volatility volatilityImpl(Time optionTime, Rate strike) const override;
    //@}
};
} // namespace QuantExt

#endif
