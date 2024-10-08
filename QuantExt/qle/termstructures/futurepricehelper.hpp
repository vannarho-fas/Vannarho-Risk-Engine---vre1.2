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

/*! \file qle/termstructures/futurepricehelper.hpp
    \brief Future price helper
    \ingroup termstructures
*/

#ifndef quantext_futurepricehelper_hpp
#define quantext_futurepricehelper_hpp

#include <qle/termstructures/pricetermstructure.hpp>
#include <ql/termstructures/bootstraphelper.hpp>

namespace QuantExt {

typedef QuantLib::BootstrapHelper<PriceTermStructure> PriceHelper;

//! Helper for bootstrapping over future prices
//! \ingroup termstructures
class FuturePriceHelper : public PriceHelper {
public:
    //! \name Constructors
    //@{
    FuturePriceHelper(const QuantLib::Handle<QuantLib::Quote>& price,
        const QuantLib::Date& expiryDate);

    FuturePriceHelper(QuantLib::Real price, const QuantLib::Date& expiryDate);
    //@}

    //! \name PriceHelper interface
    //@{
    QuantLib::Real impliedQuote() const override;
    //@}

    //! \name Visitability
    //@{
    void accept(QuantLib::AcyclicVisitor& v) override;
    //@}
};

}

#endif
