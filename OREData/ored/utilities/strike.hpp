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

/*! \file ored/utilities/strike.hpp
    \brief strike description
    \ingroup utilities
*/

#pragma once

#include <ql/types.hpp>
#include <string>

namespace ore {
namespace data {

struct Strike {
    enum class Type {
        ATM,
        ATMF,
        ATM_Offset,
        Absolute,
        Delta,
        DeltaCall,
        DeltaPut,
        BF,
        RR,
        ATM_Moneyness,
        ATMF_Moneyness
    };
    Type type;
    QuantLib::Real value;
};

//! Convert text to Strike
/*!
\ingroup utilities
*/
Strike parseStrike(const std::string& s);

//! Convert Strike to text
/*!
\ingroup utilities
*/
std::ostream& operator<<(std::ostream& out, const Strike& s);

//! Convert Strike to text
/*!
\ingroup utilities
*/
std::ostream& operator<<(std::ostream& out, const Strike& s);

//! Logical comparison of strikes
/*!
\ingroup utilities
*/
bool operator==(const Strike& s1, const Strike& s2);

//! Convenience function that computes an absolute strike
/*!
\ingroup utilities
*/
QuantLib::Real computeAbsoluteStrike(const Strike& s, const QuantLib::Real atm, const QuantLib::Real atmf);

//! Utility class for handling delta strings ATM, 10P, 25C, ... used e.g. for FX Surfaces
class DeltaString {
public:
    DeltaString(const std::string& s);
    bool isAtm() const { return isAtm_; }
    bool isPut() const { return isPut_; }
    bool isCall() const { return isCall_; }
    Real delta() const { return delta_; }

private:
    bool isAtm_, isPut_, isCall_;
    Real delta_;
};

} // namespace data
} // namespace ore
