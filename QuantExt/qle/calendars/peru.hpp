/*
 Copyright (C) 2018 Quaternion Risk Management Ltd
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

/*! \file peru.hpp
    \brief Peru calendar
*/

#ifndef quantext_peruvian_calendar_hpp
#define quantext_peruvian_calendar_hpp

#include <ql/time/calendar.hpp>

namespace QuantExt {
using namespace QuantLib;

class Peru : public Calendar {
private:
    class LseImpl : public Calendar::WesternImpl {
    public:
        std::string name() const override { return "Lima stock exchange"; }
        bool isBusinessDay(const Date&) const override;
    };

public:
    enum Market {
        LSE // Lima Stock Exchange
    };
    Peru(Market m = LSE);
};

} // namespace QuantExt

#endif
