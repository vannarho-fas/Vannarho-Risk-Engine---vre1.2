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

/*! \file krwkoribor.hpp
    \brief KRW-KORIBOR index
    \ingroup indexes
*/

#ifndef quantext_twdtaibor_hpp
#define quantext_twdtaibor_hpp

#include <ql/currencies/asia.hpp>
#include <ql/indexes/iborindex.hpp>
#include <ql/time/calendars/taiwan.hpp>
#include <ql/time/daycounters/actual365fixed.hpp>

namespace QuantExt {
using namespace QuantLib;

//! TWD-TAIBOR index
/*! TWD-TAIBOR rate.

\warning Check roll convention and EOM.

\ingroup indexes
*/
class TWDTaibor : public IborIndex {
public:
    TWDTaibor(const Period& tenor, const Handle<YieldTermStructure>& h = Handle<YieldTermStructure>())
        : IborIndex("TWD-TAIBOR", tenor, 2, TWDCurrency(), Taiwan(), ModifiedFollowing, false, Actual365Fixed(), h) {}
};
} // namespace QuantExt

#endif
