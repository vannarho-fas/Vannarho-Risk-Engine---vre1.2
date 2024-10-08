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

/*! \file logquote.cpp
    \brief implementation file for logquote
    \ingroup
*/

#include <qle/quotes/logquote.hpp>

namespace QuantExt {

Real LogQuote::quote() const { return q_->value(); }

Real LogQuote::value() const { return logValue_; }

bool LogQuote::isValid() const { return q_->isValid(); }

void LogQuote::update() {
    Real v = q_->value();
    QL_REQUIRE(v > 0.0, "Invalid quote, cannot take log of non-positive number");
    logValue_ = std::log(v);
}
} // namespace QuantExt
