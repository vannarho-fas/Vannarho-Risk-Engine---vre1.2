/*
 Copyright (C) 2019 Quaternion Risk Management Ltd
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

/*! \file saibor.hpp
    \brief SAR-SAIBOR index
    \ingroup indexes
*/

#ifndef quantext_saibor_hpp
#define quantext_saibor_hpp

#include <ql/currencies/asia.hpp>
#include <ql/indexes/iborindex.hpp>
#include <ql/time/calendars/saudiarabia.hpp>
#include <ql/time/daycounters/actual360.hpp>

namespace QuantExt {
using namespace QuantLib;

//! SAR-SAIBOR index
/*! SAR-SAIBOR rate published by SAMA

    See <http://www.sama.gov.sa>

    \warning Check roll convention and EOM.

    \ingroup indexes
*/
class SAibor : public IborIndex {
public:
    SAibor(const Period& tenor, const Handle<YieldTermStructure>& h = Handle<YieldTermStructure>())
        : IborIndex("SAR-SAIBOR", tenor, 2, SARCurrency(), SaudiArabia(), ModifiedFollowing, false, Actual360(), h) {}
};
} // namespace QuantExt

#endif
