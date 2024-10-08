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

/*! \file immfraratehelper.hpp
    \brief IMM FRA rate helper
    \ingroup termstructures
*/

#ifndef quantext_immfraratehelper_hpp
#define quantext_immfraratehelper_hpp

#include <ql/termstructures/yield/ratehelpers.hpp>

namespace QuantExt {
using namespace QuantLib;

Date getImmDate(Date asof, Size i);

typedef RelativeDateBootstrapHelper<YieldTermStructure> RelativeDateRateHelper;

//! Rate helper for bootstrapping over %FRA rates
//! \ingroup termstructures
class ImmFraRateHelper : public RelativeDateRateHelper {
public:
    ImmFraRateHelper(const Handle<Quote>& rate, const Size imm1, const Size imm2,
                     const QuantLib::ext::shared_ptr<IborIndex>& iborIndex, Pillar::Choice pillar = Pillar::LastRelevantDate,
                     Date customPillarDate = Date());

    //! \name RateHelper interface
    //@{
    Real impliedQuote() const override;
    void setTermStructure(YieldTermStructure*) override;
    //@}
    //! \name Visitability
    //@{
    void accept(AcyclicVisitor&) override;
    //@}

private:
    void initializeDates() override;
    Date fixingDate_;
    Size imm1_, imm2_;
    Pillar::Choice pillarChoice_;
    QuantLib::ext::shared_ptr<IborIndex> iborIndex_;
    RelinkableHandle<YieldTermStructure> termStructureHandle_;
};

}; // namespace QuantExt

#endif
