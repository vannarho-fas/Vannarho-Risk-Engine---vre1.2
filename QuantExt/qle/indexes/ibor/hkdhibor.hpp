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

/*! \file hkdhibor.hpp
    \brief HKD-HIBOR index
    \ingroup indexes
*/

#ifndef quantext_hkd_hibor_hpp
#define quantext_hkd_hibor_hpp

#include <ql/currencies/asia.hpp>
#include <ql/indexes/iborindex.hpp>
#include <ql/time/calendars/hongkong.hpp>
#include <ql/time/daycounters/actual365fixed.hpp>

namespace QuantExt {
using namespace QuantLib;

//! HKD-HIBOR index
/*! HKD-HIBOR rate overseen by The Hong Kong Association of Banks.

    See <http://www.hkab.org.hk>.

    \warning Check roll convention and EOM.

            \ingroup indexes
*/
class HKDHibor : public IborIndex {
public:
    HKDHibor(const Period& tenor, const Handle<YieldTermStructure>& h = Handle<YieldTermStructure>())
        : IborIndex("HKD-HIBOR", tenor, 0, HKDCurrency(), HongKong(), ModifiedFollowing, false, Actual365Fixed(), h) {}
};
} // namespace QuantExt

#endif
