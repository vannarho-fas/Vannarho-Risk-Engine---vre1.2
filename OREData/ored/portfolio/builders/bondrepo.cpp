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

#include <ored/portfolio/builders/bondrepo.hpp>
#include <qle/pricingengines/accrualbondrepoengine.hpp>
#include <qle/pricingengines/discountingbondrepoengine.hpp>

namespace ore {
namespace data {

QuantLib::ext::shared_ptr<QuantLib::PricingEngine>
DiscountingBondRepoEngineBuilder::engineImpl(const std::string& repoCurveId) {
    bool includeSecurityLeg = parseBool(modelParameter("IncludeSecurityLeg"));
    return QuantLib::ext::make_shared<QuantExt::DiscountingBondRepoEngine>(
        market_->yieldCurve(repoCurveId, configuration(MarketContext::pricing)), includeSecurityLeg);
}

QuantLib::ext::shared_ptr<QuantLib::PricingEngine> AccrualBondRepoEngineBuilder::engineImpl(const std::string&) {
    bool includeSecurityLeg = parseBool(modelParameter("IncludeSecurityLeg"));
    return QuantLib::ext::make_shared<QuantExt::AccrualBondRepoEngine>(includeSecurityLeg);
}

} // namespace data
} // namespace ore
