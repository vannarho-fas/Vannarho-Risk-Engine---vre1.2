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

/*! \file ored/marketdata/clonedloader.hpp
    \brief loader providing cloned data from another loader
    \ingroup marketdata
*/

#include <ored/marketdata/inmemoryloader.hpp>

#pragma once

namespace ore {
namespace data {

class ClonedLoader : public ore::data::InMemoryLoader {

public:
    ClonedLoader(const QuantLib::Date& loaderDate, const QuantLib::ext::shared_ptr<Loader>& inLoader);

    const QuantLib::Date& getLoaderDate() const { return loaderDate_; };

protected:
    QuantLib::Date loaderDate_;
};

} // namespace data
} // namespace ore
