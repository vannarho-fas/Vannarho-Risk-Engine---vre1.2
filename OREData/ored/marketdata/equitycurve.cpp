/*
 Copyright (C) 2016 Quaternion Risk Management Ltd
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

#include <ored/marketdata/equitycurve.hpp>
#include <ored/utilities/log.hpp>
#include <ql/math/interpolations/backwardflatinterpolation.hpp>
#include <ql/math/interpolations/convexmonotoneinterpolation.hpp>
#include <ql/math/interpolations/loginterpolation.hpp>
#include <ql/termstructures/yield/discountcurve.hpp>
#include <ql/termstructures/yield/flatforward.hpp>
#include <ql/termstructures/yield/zerocurve.hpp>

#include <ored/utilities/parsers.hpp>

#include <algorithm>
#include <regex>

using namespace QuantLib;
using namespace std;

namespace ore {
namespace data {

EquityCurve::EquityCurve(Date asof, EquityCurveSpec spec, const Loader& loader, const CurveConfigurations& curveConfigs,
                         const Conventions& conventions,
                         const map<string, boost::shared_ptr<YieldCurve>>& requiredYieldCurves) {

    try {

        const boost::shared_ptr<EquityCurveConfig>& config = curveConfigs.equityCurveConfig(spec.curveConfigID());

        dc_ = Actual365Fixed();
        if (config->dayCountID() == "") {
            DLOG("No Day Count convention specified for " << spec.curveConfigID() << ", using A365F as default");
        } else {
            dc_ = parseDayCounter(config->dayCountID());
        }

        // We loop over all market data, looking for quotes that match the configuration
        // until we found the whole set of quotes or do not have more quotes in the
        // market data

        vector<boost::shared_ptr<EquityForwardQuote>> qt;   // for sorting quotes_/terms_ pairs
        equitySpot_ = Null<Real>();
        Size quotesRead = 0;

        // in case of wild-card in config
        bool wcFlag = false;  
		bool foundRegex = false;
        regex reg1;
        
		// check for regex string in config
        for (Size i = 0; i < config->fwdQuotes().size(); i++) {
            foundRegex |= config->fwdQuotes()[i].find("*") != string::npos;
        }
        if (config->type() == EquityCurveConfig::Type::ForwardPrice && foundRegex) {
            QL_REQUIRE(config->fwdQuotes().size() == 1, "wild card specified in " << config->curveID() << " but more quotes also specified.");
            LOG("Wild card quote specified for " << config->curveID())
            wcFlag = true;
            string regexstr = config->fwdQuotes()[0];
            boost::replace_all(regexstr, "*", ".*");
            reg1 = regex(regexstr);
        }
        else {
            for (Size i=0; i < config->fwdQuotes().size(); i++) {
                quotes_.push_back(Null<Real>());
                terms_.push_back(Null<Date>());
            }
        }

        for (auto& md : loader.loadQuotes(asof)) {

            if (md->asofDate() == asof && md->instrumentType() == MarketDatum::InstrumentType::EQUITY_SPOT &&
                md->quoteType() == MarketDatum::QuoteType::PRICE) {

                boost::shared_ptr<EquitySpotQuote> q = boost::dynamic_pointer_cast<EquitySpotQuote>(md);

                if (q->name() == config->equitySpotQuoteID()) {
                    QL_REQUIRE(equitySpot_ == Null<Real>(), "duplicate equity spot quote " << q->name() << " found.");
                    equitySpot_ = q->quote()->value();
                }
            }

            if (config->type() == EquityCurveConfig::Type::ForwardPrice && md->asofDate() == asof &&
                md->instrumentType() == MarketDatum::InstrumentType::EQUITY_FWD &&
                md->quoteType() == MarketDatum::QuoteType::PRICE) {

                boost::shared_ptr<EquityForwardQuote> q = boost::dynamic_pointer_cast<EquityForwardQuote>(md);

                if (wcFlag) {
                    // is the quote 'in' the config? (also check expiry not before asof)
                    if (regex_match(q->name(), reg1) && asof <= q->expiryDate()) {
                        QL_REQUIRE(find(qt.begin(), qt.end(), q) == qt.end(), "duplicate market datum found for " << q->name());
                        DLOG("EquityCurve Forward Price found for quote: " << q->name());
                        qt.push_back(q); // terms_ and quotes_
                        quotesRead++;
                    }
                }
                else {
                    vector<string>::const_iterator it1 =
                        std::find(config->fwdQuotes().begin(), config->fwdQuotes().end(), q->name());

                    // is the quote one of the list in the config ?
                    if (it1 != config->fwdQuotes().end()) {
                        Size pos = it1 - config->fwdQuotes().begin();
                        QL_REQUIRE(terms_[pos] == Null<Date>(),
                            "duplicate market datum found for " << config->fwdQuotes()[pos]);
                        terms_[pos] = q->expiryDate();
                        quotes_[pos] = q->quote()->value();
                        quotesRead++;
                    }
                }
            }

            if (config->type() == EquityCurveConfig::Type::DividendYield && md->asofDate() == asof &&
                md->instrumentType() == MarketDatum::InstrumentType::EQUITY_DIVIDEND &&
                md->quoteType() == MarketDatum::QuoteType::RATE) {

                boost::shared_ptr<EquityDividendYieldQuote> q =
                    boost::dynamic_pointer_cast<EquityDividendYieldQuote>(md);

                vector<string>::const_iterator it1 =
                    std::find(config->fwdQuotes().begin(), config->fwdQuotes().end(), q->name());

                // is the quote one of the list in the config ?
                if (it1 != config->fwdQuotes().end()) {
                    Size pos = it1 - config->fwdQuotes().begin();
                    QL_REQUIRE(terms_[pos] == Null<Date>(), "duplicate market datum found for " << config->fwdQuotes()[pos]);
                    DLOG("EquityCurve Dividend Yield found for quote: " << q->name());
                    terms_[pos]=q->tenorDate();
                    quotes_[pos]=q->quote()->value();
                    quotesRead++;
                }
            }
        }
        LOG("EquityCurve: read " << quotesRead << " quotes of type " << config->type());
        QL_REQUIRE(equitySpot_ != Null<Real>(), "Equity spot quote not found for " << config->curveID());

        // sort quotes and terms in case of wild-card
        if (wcFlag){
            QL_REQUIRE(quotesRead > 0, "Wild card quote specified, but no quotes read.")

            // sort
            std::sort(qt.begin(), qt.end(),
                [](const boost::shared_ptr<EquityForwardQuote>& a, const boost::shared_ptr<EquityForwardQuote>& b) -> bool {
                return a->expiryDate() < b->expiryDate();
            });

            // populate individual quote, term vectors
            for (Size i = 0; i < qt.size(); i++) {
                terms_.push_back(qt[i]->expiryDate());
                quotes_.push_back(qt[i]->quote()->value());
            }
        }
        else {
            QL_REQUIRE(quotesRead == config->fwdQuotes().size(), 
                "read " << quotesRead << ", but " << config->fwdQuotes().size() << " required.");
        }
              
        for (Size i = 0; i < terms_.size(); i++) {
            QL_REQUIRE(terms_[i] > asof, "Invalid Fwd Expiry " << terms_[i] << " vs. " << asof);
            if (i > 0) {
                QL_REQUIRE(terms_[i] > terms_[i - 1], "terms must be increasing in curve config");
            }
        }
        curveType_ = config->type();
        YieldCurveSpec ycspec(config->currency(), config->forecastingCurve());
        // at this stage we should have built the curve already
        //  (consider building curve on fly if not? Would need to work around fact that requiredYieldCurves is currently
        //  const ref)
        auto itr = requiredYieldCurves.find(ycspec.name());
        QL_REQUIRE(itr != requiredYieldCurves.end(),
                   "Yield Curve Spec - " << ycspec.name() << " - not found during equity curve build");
        boost::shared_ptr<YieldCurve> yieldCurve = itr->second;
        forecastYieldTermStructure_ = yieldCurve->handle();

        dividendInterpVariable_ = parseYieldCurveInterpolationVariable(config->dividendInterpolationVariable());
        dividendInterpMethod_ = parseYieldCurveInterpolationMethod(config->dividendInterpolationMethod());
    } catch (std::exception& e) {
        QL_FAIL("equity curve building failed: " << e.what());
    } catch (...) {
        QL_FAIL("equity curve building failed: unknown error");
    }
}

boost::shared_ptr<YieldTermStructure> EquityCurve::divYieldTermStructure(const Date& asof) const {

    try {
        vector<Rate> dividendRates;
        if (curveType_ == EquityCurveConfig::Type::ForwardPrice) {
            // Convert Fwds into dividends.
            // Fwd = Spot e^{(r-q)T}
            // => q = 1/T Log(Spot/Fwd) + r
            for (Size i = 0; i < quotes_.size(); i++) {
                QL_REQUIRE(quotes_[i] > 0, "Invalid Fwd Price " << quotes_[i] << " for " << spec_.name());
                Time t = dc_.yearFraction(asof, terms_[i]);
                Rate ir_rate = forecastYieldTermStructure_->zeroRate(t, Continuous);
                dividendRates.push_back(::log(equitySpot_ / quotes_[i]) / t + ir_rate);
            }
        } else if (curveType_ == EquityCurveConfig::Type::DividendYield) {
            dividendRates = quotes_;
        } else if (curveType_ == EquityCurveConfig::Type::NoDividends) {
            // Return a flat curve @ 0%
            return boost::make_shared<FlatForward>(asof, 0.0, dc_);
        } else
            QL_FAIL("Invalid Equity curve configuration type for " << spec_.name());

        QL_REQUIRE(dividendRates.size() > 0, "No dividend yield rates extracted for " << spec_.name());
        QL_REQUIRE(dividendRates.size() == terms_.size(), "vector size mismatch - dividend rates ("
                                                              << dividendRates.size() << ") vs terms (" << terms_.size()
                                                              << ")");
        // store "dividend discount factors" - in case we wish to interpolate according to discounts
        vector<Real> dividendDiscountFactors;
        for (Size i = 0; i < quotes_.size(); i++) {
            Time t = dc_.yearFraction(asof, terms_[i]);
            dividendDiscountFactors.push_back(std::exp(-dividendRates[i] * t));
        }

        boost::shared_ptr<YieldTermStructure> divCurve;
        // Build Dividend Term Structure
        if (dividendRates.size() == 1) {
            // We only have 1 quote so we build a flat curve
            divCurve.reset(new FlatForward(asof, dividendRates[0], dc_));
        } else {
            // Build a ZeroCurve
            Size n = terms_.size();
            vector<Date> dates(n + 1); // n+1 so we include asof
            vector<Rate> rates(n + 1);
            vector<Real> discounts(n + 1);
            for (Size i = 0; i < n; i++) {
                dates[i + 1] = terms_[i];
                rates[i + 1] = dividendRates[i];
                discounts[i + 1] = dividendDiscountFactors[i];
            }
            dates[0] = asof;
            rates[0] = rates[1];
            discounts[0] = 1.0;
            if (forecastYieldTermStructure_->maxDate() > dates.back()) {
                dates.push_back(forecastYieldTermStructure_->maxDate());
                rates.push_back(rates.back());
                Time maxTime = dc_.yearFraction(asof, forecastYieldTermStructure_->maxDate());
                discounts.push_back(
                    std::exp(-rates.back() * maxTime)); // flat zero extrapolation used to imply dividend DF
            }
            if (dividendInterpVariable_ == YieldCurve::InterpolationVariable::Zero) {
                switch (dividendInterpMethod_) {
                case YieldCurve::InterpolationMethod::Linear:
                    divCurve.reset(new InterpolatedZeroCurve<QuantLib::Linear>(dates, rates, dc_, QuantLib::Linear()));
                    break;
                case YieldCurve::InterpolationMethod::LogLinear:
                    divCurve.reset(
                        new InterpolatedZeroCurve<QuantLib::LogLinear>(dates, rates, dc_, QuantLib::LogLinear()));
                    break;
                case YieldCurve::InterpolationMethod::NaturalCubic:
                    divCurve.reset(new InterpolatedZeroCurve<QuantLib::Cubic>(dates, rates, dc_,
                                                                              Cubic(CubicInterpolation::Kruger, true)));
                    break;
                case YieldCurve::InterpolationMethod::FinancialCubic:
                    divCurve.reset(new InterpolatedZeroCurve<QuantLib::Cubic>(
                        dates, rates, dc_,
                        Cubic(CubicInterpolation::Kruger, true, CubicInterpolation::SecondDerivative, 0.0,
                              CubicInterpolation::FirstDerivative)));
                    break;
                case YieldCurve::InterpolationMethod::ConvexMonotone:
                    divCurve.reset(new InterpolatedZeroCurve<QuantLib::ConvexMonotone>(dates, rates, dc_,
                                                                                       QuantLib::ConvexMonotone()));
                    break;
                default:
                    QL_FAIL("Interpolation method not recognised.");
                }
            } else if (dividendInterpVariable_ == YieldCurve::InterpolationVariable::Discount) {
                switch (dividendInterpMethod_) {
                case YieldCurve::InterpolationMethod::Linear:
                    divCurve.reset(
                        new InterpolatedDiscountCurve<QuantLib::Linear>(dates, discounts, dc_, QuantLib::Linear()));
                    break;
                case YieldCurve::InterpolationMethod::LogLinear:
                    divCurve.reset(new InterpolatedDiscountCurve<QuantLib::LogLinear>(dates, discounts, dc_,
                                                                                      QuantLib::LogLinear()));
                    break;
                case YieldCurve::InterpolationMethod::NaturalCubic:
                    divCurve.reset(new InterpolatedDiscountCurve<QuantLib::Cubic>(
                        dates, discounts, dc_, Cubic(CubicInterpolation::Kruger, true)));
                    break;
                case YieldCurve::InterpolationMethod::FinancialCubic:
                    divCurve.reset(new InterpolatedDiscountCurve<QuantLib::Cubic>(
                        dates, discounts, dc_,
                        Cubic(CubicInterpolation::Kruger, true, CubicInterpolation::SecondDerivative, 0.0,
                              CubicInterpolation::FirstDerivative)));
                    break;
                case YieldCurve::InterpolationMethod::ConvexMonotone:
                    divCurve.reset(new InterpolatedDiscountCurve<QuantLib::ConvexMonotone>(dates, discounts, dc_,
                                                                                           QuantLib::ConvexMonotone()));
                    break;
                default:
                    QL_FAIL("Interpolation method not recognised.");
                }
            } else {
                QL_FAIL("Unsupported interpolation variable for dividend yield curve");
            }
            divCurve->enableExtrapolation();
        }
        return divCurve;
    } catch (std::exception& e) {
        QL_FAIL("equity curve building failed: " << e.what());
    } catch (...) {
        QL_FAIL("equity curve building failed: unknown error");
    }
}
} // namespace data
} // namespace ore
