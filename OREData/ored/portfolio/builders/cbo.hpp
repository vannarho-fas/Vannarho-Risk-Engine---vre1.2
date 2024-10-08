/*
 Copyright (C) 2020 Quaternion Risk Management Ltd
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

/*! \file portfolio/builders/cbo.hpp
\brief
\ingroup portfolio
*/

#pragma once

#include <ored/portfolio/enginefactory.hpp>
#include <ql/experimental/credit/pool.hpp>

namespace ore {
namespace data {

class CboMCEngineBuilder : public EngineBuilder {
public:
    //! constructor that builds a usual pricing engine
    CboMCEngineBuilder() : EngineBuilder("OneFactorCopula", "MonteCarloCBOEngine", {"CBO"}) {};

    QuantLib::ext::shared_ptr<QuantLib::PricingEngine> engine(const QuantLib::ext::shared_ptr<QuantLib::Pool>& pool);
};

} // namespace data
} // namespace ore
