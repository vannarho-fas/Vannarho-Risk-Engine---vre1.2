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

/*! \file scenario/scenariogenerator.hpp
    \brief Scenario generator base classes
    \ingroup scenario
*/

#pragma once

#include <orea/scenario/scenariogenerator.hpp>

namespace ore {
namespace analytics {

class ClonedScenarioGenerator : public ScenarioGenerator {
public:
    ClonedScenarioGenerator(const QuantLib::ext::shared_ptr<ScenarioGenerator>& scenarioGenerator,
                            const std::vector<Date>& dates, const Size nSamples);
    QuantLib::ext::shared_ptr<Scenario> next(const Date& d) override;
    virtual void reset() override;

private:
    std::map<Date, size_t> dates_;
    Date firstDate_;
    Size nSim_ = 0;
    std::vector<QuantLib::ext::shared_ptr<Scenario>> scenarios_;
};

} // namespace analytics
} // namespace ore
