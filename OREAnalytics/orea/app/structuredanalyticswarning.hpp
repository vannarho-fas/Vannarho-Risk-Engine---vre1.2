/*
 Copyright (C) 2021 Quaternion Risk Management Ltd
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

/*! \file orea/app/structuredanalyticswarning.hpp
    \brief Class for structured analytics warnings
    \ingroup app
*/

#pragma once

#include <ored/utilities/log.hpp>

namespace ore {
namespace analytics {

class StructuredAnalyticsWarningMessage : public ore::data::StructuredMessage {
public:
    StructuredAnalyticsWarningMessage(const std::string& analyticType, const std::string& warningType,
                                      const std::string& warningWhat,
                                      const std::map<std::string, std::string>& subFields = {})
        : StructuredMessage(
              Category::Warning, Group::Analytics, warningWhat,
              std::map<std::string, std::string>({{"warningType", warningType}, {"analyticType", analyticType}})) {

        addSubFields(subFields);
    }
};

} // namespace analytics
} // namespace ore
