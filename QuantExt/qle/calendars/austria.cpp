/*
 Copyright (C) 2020 Quaternion Risk Management Ltd
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

#include <qle/calendars/austria.hpp>

using namespace QuantLib;

namespace QuantExt {

Austria::Austria(Market market) {
    // all calendar instances share the same implementation instance
    static QuantLib::ext::shared_ptr<Calendar::Impl> impl(new Austria::SettlementImpl);
    impl_ = impl;
}

bool Austria::SettlementImpl::isBusinessDay(const Date& date) const {
    Weekday w = date.weekday();
    Day d = date.dayOfMonth(), dd = date.dayOfYear();
    Month m = date.month();
    Year y = date.year();
    Day em = easterMonday(y);
    if (isWeekend(w)
        // New Years Day
        || (d == 1 && m == January)
        // Epiphany
        || (d == 6 && m == January)
        // Easter Monday
        || (dd == em)
        // Labour Day
        || (d == 1 && m == May)
        // Ascension Thursday
        || (dd == em + 38)
        // Whit Monday
        || (dd == em + 49)
        // Corpus Christi
        || (dd == em + 59)
        // Assumption Day
        || (d == 15 && m == August)
        // National Day
        || (d == 26 && m == October)
        // All Saints
        || (d == 1 && m == November)
        // Conception Day
        || (d == 8 && m == December)
        // Christmas
        || (d == 25 && m == December)
        // St Stephens Day
        || (d == 26 && m == December))
        return false;
    return true;
}

} // namespace QuantExt
