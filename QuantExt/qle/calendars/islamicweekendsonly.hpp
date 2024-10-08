/*
 Copyright (C) 2021 Quaternion Risk Management Ltd
 Copyright (C) 2024 Growth Mindset Pty Ltd

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

/*! \file islamicweekendsonly.hpp
    \brief Islamic weekends-only calendar
*/

#ifndef quantext_target_islamicweekends_only_hpp
#define quantext_target_islamicweekends_only_hpp

#include <ql/time/calendar.hpp>

namespace QuantExt {
using namespace QuantLib;

    //! Islamic Weekends-only calendar
    /*! This calendar has no bank holidays except for Islamic weekends
        (Fridays and Saturdays).

        \ingroup calendars
    */
    class IslamicWeekendsOnly : public Calendar {
      private:
        class Impl : public Calendar::Impl {
          public:
            std::string name() const override { return "Islamic weekends only"; }
            bool isWeekend(Weekday) const override;
            bool isBusinessDay(const Date&) const override;
        };
      public:
        IslamicWeekendsOnly();
    };

}


#endif
