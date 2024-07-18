/*
 Copyright (C) 2023 Quaternion Risk Management Ltd
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

/*! \file qle/pricingengines/numericalintegrationindexcdsoptionengine.hpp
    \brief numerical integration index credit default swap option engine.
*/

#pragma once

#include <qle/pricingengines/indexcdsoptionbaseengine.hpp>

namespace QuantExt {

class NumericalIntegrationIndexCdsOptionEngine : public QuantExt::IndexCdsOptionBaseEngine {
public:
    using IndexCdsOptionBaseEngine::IndexCdsOptionBaseEngine;

private:
    void doCalc() const override;
    Real forwardRiskyAnnuityStrike(const Real strike) const;
};

} // namespace QuantExt
