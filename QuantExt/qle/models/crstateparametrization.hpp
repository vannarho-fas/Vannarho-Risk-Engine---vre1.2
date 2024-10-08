/*
 Copyright (C) 2017 Quaternion Risk Management Ltd
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

/*! \file crstateparametrization.hpp
    \brief credit state parametrization
    \ingroup models
*/

#pragma once

#include <qle/models/parametrization.hpp>

namespace QuantExt {

//! Credit State Parametrization
/*! \ingroup models */
class CrStateParametrization : public Parametrization {
public:
    CrStateParametrization(const Size id) : Parametrization(Currency()), id_(id) {}
    Size id() const { return id_; }

private:
    const Size id_;
};

} // namespace QuantExt
