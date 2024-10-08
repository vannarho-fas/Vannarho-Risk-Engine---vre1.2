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

/*! \file qle/indexes/inflationindexobserver.hpp
    \brief inflation index observer class
    \ingroup indexes
*/

#ifndef quantext_inflation_index_observer_hpp
#define quantext_inflation_index_observer_hpp

namespace QuantExt {

//! Inflation Index observer
/*! \ingroup indexes */
class InflationIndexObserver : public TermStructure {
public:
    InflationIndexObserver(const QuantLib::ext::shared_ptr<InflationIndex>& index, const Handle<Quote>& quote,
                           const Period& observationLag, const DayCounter& dayCounter = DayCounter())
        : TermStructure(dayCounter), index_(index), quote_(quote), observationLag_(observationLag) {
        registerWith(quote_);
    }

    void update() override { // called when the quote changes
        setFixing();
    }

    Date maxDate() const override {
        Date today = Settings::instance().evaluationDate();
        return today;
    }

private:
    void setFixing() {
        // something like this
        Date today = Settings::instance().evaluationDate();
        Date fixingDate = today - observationLag_;
        // overwrite the current fixing in the QuantLib::FixingManager
        index_->addFixing(fixingDate, quote_->value(), true);
    }

    QuantLib::ext::shared_ptr<InflationIndex> index_;
    Handle<Quote> quote_;
    Period observationLag_;
};
} // namespace QuantExt

#endif
