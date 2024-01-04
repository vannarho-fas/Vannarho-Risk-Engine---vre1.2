/*
 Copyright (C) 2017 Quaternion Risk Management Ltd
 All rights reserved.

 This file is part of ORE, a free-software/open-source library
 for transparent pricing and risk analysis - http://opensourcerisk.org

 ORE is free software: you can redistribute it and/or modify it
 under the terms of the Modified BSD License.  You should have received a
 copy of the license along with this program.
 The license is also available online at <http://opensourcerisk.org>

 This program is distributed on the basis that it will form a useful
 contribution to risk analytics and model standardisation, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
*/

#include <boost/range/adaptor/indexed.hpp>
#include <orea/cube/sensitivitycube.hpp>

#include <ored/utilities/log.hpp>

using namespace QuantLib;
using namespace std;

using std::make_pair;

namespace ore {
namespace analytics {

namespace {

// Utility method for lookup in the various maps
template <class KeyType, class ValueType> ValueType index(const KeyType& k, const map<KeyType, ValueType>& m) {

    auto it = m.find(k);
    QL_REQUIRE(it != m.end(), "Key, " << k << ", was not found in the sensitivity cube.");
    return it->second;
}

ore::analytics::SensitivityCube::FactorData
index(const ore::analytics::RiskFactorKey& k,
      const std::map<ore::analytics::RiskFactorKey, SensitivityCube::FactorData>& m) {

    auto it = m.find(k);
    QL_REQUIRE(it != m.end(), "Key, " << k << ", was not found in the sensitivity cube.");
    return it->second;
}

} // namespace

std::ostream& operator<<(std::ostream& out, const SensitivityCube::crossPair& cp) {
    return out << cp.first << "-" << cp.second;
}

SensitivityCube::SensitivityCube(const boost::shared_ptr<NPVSensiCube>& cube,
                                 const vector<ShiftScenarioDescription>& scenarioDescriptions,
                                 const map<RiskFactorKey, QuantLib::Real>& shiftsizes,
                                 const std::map<RiskFactorKey, std::string>& shiftSchemes)
    : cube_(cube), scenarioDescriptions_(scenarioDescriptions), shiftSizes_(shiftsizes), shiftSchemes_(shiftSchemes) {
    initialise();
}

SensitivityCube::SensitivityCube(const boost::shared_ptr<NPVSensiCube>& cube,
                                 const vector<string>& scenarioDescriptions,
                                 const map<RiskFactorKey, QuantLib::Real>& shiftsizes,
                                 const std::map<RiskFactorKey, std::string>& shiftSchemes)
    : cube_(cube), shiftSizes_(shiftsizes), shiftSchemes_(shiftSchemes) {

    // Populate scenarioDescriptions_ from string descriptions
    scenarioDescriptions_.reserve(scenarioDescriptions.size());
    for (const auto& des : scenarioDescriptions) {
        scenarioDescriptions_.push_back(ShiftScenarioDescription(des));
    }

    initialise();
}

void SensitivityCube::initialise() {
    QL_REQUIRE(scenarioDescriptions_[0].type() == ShiftScenarioDescription::Type::Base,
               "Expected the first scenario in the sensitivity cube to be of type 'Base'");

    // Populate the scenario lookup map
    crossPair factorPair;
    std::map<crossPair, Size> crossFactors;
    for (Size i = 0; i < scenarioDescriptions_.size(); i++) {
        auto des = scenarioDescriptions_[i];
        FactorData fd;
        // Don't info add for base - missing from maps
        if (des.type() != ShiftScenarioDescription::Type::Base) {
            fd.index = i;
            fd.shiftSize = shiftSize(des.key1());
            fd.factorDesc = deconstructFactor(des.factor1()).second;
        }
        scenarioIdx_[des] = i;

        // Populate factors_ = list of factors for which we can calculate a delta/gamma
        switch (des.type()) {
        case ShiftScenarioDescription::Type::Up:
            QL_REQUIRE(upFactors_.count(des.key1()) == 0, "Cannot have multiple up factors with "
                                                          "the same risk factor key["
                                                              << des.key1() << "]");
            factors_.insert(des.key1());
            upFactors_[des.key1()] = fd;
            upIndexToKey_[fd.index] = des.key1();
            break;
        case ShiftScenarioDescription::Type::Down:
            QL_REQUIRE(downFactors_.count(des.key1()) == 0, "Cannot have multiple down factors with "
                                                            "the same risk factor key ["
                                                                << des.key1() << "]");
            factors_.insert(des.key1());
            downFactors_[des.key1()] = fd;
            downIndexToKey_[fd.index] = des.key1();
            break;
        case ShiftScenarioDescription::Type::Cross:
            factorPair = make_pair(des.key1(), des.key2());
            QL_REQUIRE(crossFactors.count(factorPair) == 0, "Cannot have multiple cross factors with "
                                                            "the same risk factor key pair ["
                                                                << des.key1() << ", " << des.key2() << "]");
            crossFactors[factorPair] = i;
            crossIndexToKey_[i] = factorPair;
            break;
        default:
            // Do nothing
            break;
        }
    }

    // Add each cross factor to crossFactors with index of the two contributing factors
    for (auto cf : crossFactors) {
        FactorData id_1 = index(cf.first.first, upFactors_);
        FactorData id_2 = index(cf.first.second, upFactors_);
        crossFactors_[cf.first] = make_tuple(id_1, id_2, cf.second);
    }

    // Log warnings if each factor does not have a shift size entry and that it is not a Null<Real>()
    if (factors_.size() != shiftSizes_.size()) {
        WLOG("The number of factors from up / down shifts (" << factors_.size() << ") does not equal "
                                                             << "the number of shift sizes (" << shiftSizes_.size()
                                                             << ") supplied");
    }

    for (auto const& f : factors_) {
        auto it = shiftSizes_.find(f);
        if (it == shiftSizes_.end()) {
            WLOG("No entry for risk factor " << f << " in shift sizes.");
        }
    }
}

bool SensitivityCube::hasTrade(const string& tradeId) const { return tradeIdx_.count(tradeId) > 0; }

RiskFactorKey SensitivityCube::upDownFactor(const Size index) const {
    if (auto k = upIndexToKey_.find(index); k != upIndexToKey_.end()) {
        return k->second;
    } else if (auto k = downIndexToKey_.find(index); k != downIndexToKey_.end()) {
        return k->second;
    } else {
        return RiskFactorKey();
    }
}

SensitivityCube::crossPair SensitivityCube::crossFactor(const Size crossIndex) const {
    if (auto k = crossIndexToKey_.find(crossIndex); k != crossIndexToKey_.end()) {
        return k->second;
    } else {
        return std::make_pair(RiskFactorKey(), RiskFactorKey());
    }
}

bool SensitivityCube::hasScenario(const ShiftScenarioDescription& scenarioDescription) const {
    return scenarioIdx_.find(scenarioDescription) != scenarioIdx_.end();
}

std::string SensitivityCube::factorDescription(const RiskFactorKey& riskFactorKey) const {
    Size scenarioIdx = index(riskFactorKey, upFactors_).index;
    return scenarioDescriptions_[scenarioIdx].factor1();
}

const set<RiskFactorKey>& SensitivityCube::factors() const { return factors_; }

const std::map<SensitivityCube::crossPair, tuple<SensitivityCube::FactorData, SensitivityCube::FactorData, Size>>&
SensitivityCube::crossFactors() const {
    return crossFactors_;
}

Real SensitivityCube::shiftSize(const RiskFactorKey& riskFactorKey) const {
    auto it = shiftSizes_.find(riskFactorKey);
    QL_REQUIRE(it != shiftSizes_.end(), "Risk factor, " << riskFactorKey << ", was not found in the shift sizes.");
    return it->second;
}

std::string SensitivityCube::shiftScheme(const RiskFactorKey& riskFactorKey) const {
    auto it = shiftSchemes_.find(riskFactorKey);
    QL_REQUIRE(it != shiftSchemes_.end(), "Risk factor, " << riskFactorKey << ", was not found in the shift schemes.");
    return it->second;
}

Real SensitivityCube::npv(const string& tradeId) const { return cube_->getT0(tradeId, 0); }

Real SensitivityCube::npv(Size id) const { return cube_->getT0(id, 0); }

Real SensitivityCube::npv(Size id, Size scenarioIdx) const { return cube_->get(id, scenarioIdx); }

Real SensitivityCube::npv(const string& tradeId, const ShiftScenarioDescription& scenarioDescription) const {
    Size scenarioIdx = index(scenarioDescription, scenarioIdx_);
    Size tradeIdx = cube_->getTradeIndex(tradeId);
    return npv(tradeIdx, scenarioIdx);
}

Real SensitivityCube::delta(const Size tradeIdx, const RiskFactorKey& riskFactorKey) const {
    auto s = shiftSchemes_.find(riskFactorKey);
    QL_REQUIRE(s != shiftSchemes_.end(),
               "SensitivityCube::delta(" << tradeIdx << ", " << riskFactorKey << "): no shift scheme stored.");
    if (s->second == "Forward") {
        Size scenarioIdx = index(riskFactorKey, upFactors_).index;
        return cube_->get(tradeIdx, scenarioIdx) - cube_->getT0(tradeIdx, 0);
    } else if (s->second == "Backward") {
        Size scenarioIdx = index(riskFactorKey, downFactors_).index;
        return cube_->getT0(tradeIdx, 0) - cube_->get(tradeIdx, scenarioIdx);
    } else if (s->second == "Central") {
        Size upIdx = index(riskFactorKey, upFactors_).index;
        Size downIdx = index(riskFactorKey, downFactors_).index;
        return (cube_->get(tradeIdx, upIdx) - cube_->get(tradeIdx, downIdx)) / 2.0;
    } else {
        QL_FAIL("SensitivityCube::delta(" << tradeIdx << ", " << riskFactorKey << "): unknown shift scheme '"
                                          << s->second << "'");
    }    
}

Real SensitivityCube::delta(const string& tradeId, const RiskFactorKey& riskFactorKey) const {
    return delta(cube_->getTradeIndex(tradeId), riskFactorKey);
}

Real SensitivityCube::gamma(const Size tradeIdx, const RiskFactorKey& riskFactorKey) const {
    Size upIdx = index(riskFactorKey, upFactors_).index;
    Size downIdx = index(riskFactorKey, downFactors_).index;
    Real baseNpv = cube_->getT0(tradeIdx, 0);
    Real upNpv = cube_->get(tradeIdx, upIdx);
    Real downNpv = cube_->get(tradeIdx, downIdx);
    return upNpv - 2.0 * baseNpv + downNpv;
}

Real SensitivityCube::gamma(const string& tradeId, const RiskFactorKey& riskFactorKey) const {
    return gamma(cube_->getTradeIndex(tradeId), riskFactorKey);
}

QuantLib::Real SensitivityCube::crossGamma(QuantLib::Size id, QuantLib::Size upIdx_1, QuantLib::Size upIdx_2,
                                           QuantLib::Size crossIdx) const {
    // Approximate f_{xy}|(x,y) by
    // ([f_{x}|(x,y + dy)] - [f_{x}|(x,y)]) / dy
    // ([f(x + dx,y + dy) - f(x, y + dy)] - [f(x + dx,y) - f(x,y)]) / (dx dy)
    Real baseNpv = cube_->getT0(id, 0);
    Real upNpv_1 = cube_->get(id, upIdx_1);
    Real upNpv_2 = cube_->get(id, upIdx_2);
    Real crossNpv = cube_->get(id, crossIdx);
    return crossNpv - upNpv_1 - upNpv_2 + baseNpv;
}

Real SensitivityCube::crossGamma(const Size tradeIdx, const crossPair& riskFactorKeyPair) const {
    FactorData upFd_1, upFd_2;
    Size upIdx_1, upIdx_2, crossIdx;
    std::tie(upFd_1, upFd_2, crossIdx) = index(riskFactorKeyPair, crossFactors_);
    upIdx_1 = upFd_1.index;
    upIdx_2 = upFd_2.index;
    return crossGamma(tradeIdx, upIdx_1, upIdx_2, crossIdx);
}

Real SensitivityCube::crossGamma(const std::string& tradeId, const crossPair& riskFactorKeyPair) const {
    return crossGamma(cube_->getTradeIndex(tradeId), riskFactorKeyPair);
}

std::set<RiskFactorKey> SensitivityCube::relevantRiskFactors() const {
    std::set<RiskFactorKey> result;
    for (auto const i : cube_->relevantScenarios()) {
        result.insert(scenarioDescriptions_[i].key1());
        if (scenarioDescriptions_[i].type() == ShiftScenarioDescription::Type::Cross)
            result.insert(scenarioDescriptions_[i].key2());
    }
    return result;
}

} // namespace analytics
} // namespace ore
