/*
 Copyright (C) 2022 Quaternion Risk Management Ltd
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

/*! \file ored/portfolio/commoditydigitaloption.hpp
    \brief Commodity digital option representation as call spread
    \ingroup tradedata
*/

#pragma once

#include <ored/portfolio/optiondata.hpp>
#include <ored/portfolio/vanillaoption.hpp>

namespace ore {
namespace data {

//! Commodity digital option trade representation as call spread
/*! \ingroup tradedata
 */
class CommodityDigitalOption : public ore::data::Trade {
public:
    //! Default constructor
    CommodityDigitalOption();

    //! Detailed constructor
    CommodityDigitalOption(const Envelope& env, const OptionData& optionData, const std::string& commodityName,
			   const std::string& currency, QuantLib::Real strike, QuantLib::Real payoff,
			   const boost::optional<bool>& isFuturePrice = boost::none,
			   const QuantLib::Date& futureExpiryDate = QuantLib::Date());

    //! Build underlying instrument and link pricing engine
    void build(const QuantLib::ext::shared_ptr<EngineFactory>& engineFactory) override;

    //! Add underlying Commodity names
    std::map<AssetClass, std::set<std::string>>
    underlyingIndices(const QuantLib::ext::shared_ptr<ReferenceDataManager>& referenceDataManager = nullptr) const override;

    //! \name Serialisation
    //@{
    virtual void fromXML(XMLNode* node) override;
    virtual XMLNode* toXML(XMLDocument& doc) const override;
    //@}

    //! \name Trade
    //@{
    bool hasCashflows() const override { return false; }
    //@}

    //! \name Inspectors
    //@{
    const boost::optional<bool>& isFuturePrice() const { return isFuturePrice_; }
    const QuantLib::Date& futureExpiryDate() const { return futureExpiryDate_; }
    //@}

private:
    OptionData optionData_;
    std::string name_;
    string currency_;
    Real strike_;
    Real payoff_;  

    /*! Indicates if the option underlying is a commodity future settlement price, \c true, or a spot price \c false.
        If not explicitly set, it is assumed to be \c true.
    */
    boost::optional<bool> isFuturePrice_;

    /*! An explicit expiry date for the underlying future contract. This can be used if the option trade references a
        future contract settlement price and the option's expiry date does not match the future contract expiry date.
    */
    QuantLib::Date futureExpiryDate_;

    //! An index is needed if the option is to be automatically exercised on expiry.
    QuantLib::ext::shared_ptr<QuantLib::Index> index_;

    //! Hold the external index name if needed e.g. in the case of an FX index.
    std::string indexName_;

    //! Store the option expiry date.
    QuantLib::Date expiryDate_;

    Date forwardDate_;
};

} // namespace data
} // namespace ore
