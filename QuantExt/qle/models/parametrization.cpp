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

#include <qle/models/parametrization.hpp>

namespace QuantExt {

Parametrization::Parametrization(const Currency& currency, const std::string& name)
    : h_(1.0E-6), h2_(1.0E-4), currency_(currency), name_(name), emptyParameter_(QuantLib::ext::make_shared<NullParameter>()) {}

} // namespace QuantExt
