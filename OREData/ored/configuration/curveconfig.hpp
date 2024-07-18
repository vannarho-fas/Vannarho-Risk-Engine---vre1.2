/*
 Copyright (C) 2017 Quaternion Risk Management Ltd
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

/*! \file ored/configuration/curveconfig.hpp
    \brief Base curve configuration classes
    \ingroup configuration
*/

#pragma once

#include <ored/marketdata/curvespec.hpp>
#include <ored/utilities/xmlutils.hpp>

#include <set>

namespace ore {
namespace data {
using ore::data::XMLSerializable;
using std::set;
using std::string;

//! Base curve configuration
/*!
  \ingroup configuration
*/
class CurveConfig : public XMLSerializable {
public:
    //! \name Constructors/Destructors
    //@{
    //! Detailed constructor
    CurveConfig(const string& curveID, const string& curveDescription, const vector<string>& quotes = vector<string>())
        : curveID_(curveID), curveDescription_(curveDescription), quotes_(quotes) {}
    //! Default constructor
    CurveConfig() {}
    //@}

    //! \name Inspectors
    //@{
    const string& curveID() const { return curveID_; }
    const string& curveDescription() const { return curveDescription_; }
    const set<string>& requiredCurveIds(const CurveSpec::CurveType& curveType) const;
    const map<CurveSpec::CurveType, set<string>>& requiredCurveIds() const;
    //@}

    //! \name Setters
    //@{
    string& curveID() { return curveID_; }
    string& curveDescription() { return curveDescription_; }
    set<string>& requiredCurveIds(const CurveSpec::CurveType& curveType);
    map<CurveSpec::CurveType, set<string>>& requiredCurveIds();
    //@}

    //! Return all the market quotes required for this config
    virtual const vector<string>& quotes() { return quotes_; }

protected:
    string curveID_;
    string curveDescription_;
    vector<string> quotes_;
    map<CurveSpec::CurveType, set<string>> requiredCurveIds_;
};

} // namespace data
} // namespace ore
