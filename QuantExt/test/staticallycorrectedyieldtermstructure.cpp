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

#include "toplevelfixture.hpp"
#include "utilities.hpp"
#include <boost/test/unit_test.hpp>

#include <qle/termstructures/staticallycorrectedyieldtermstructure.hpp>

#include <ql/quotes/simplequote.hpp>
#include <ql/termstructures/yield/flatforward.hpp>
#include <ql/time/calendars/nullcalendar.hpp>

#include <boost/make_shared.hpp>

using namespace QuantExt;
using namespace QuantLib;
using namespace boost::unit_test_framework;

BOOST_FIXTURE_TEST_SUITE(QuantExtTestSuite, qle::test::TopLevelFixture)

BOOST_AUTO_TEST_SUITE(StaticallyCorrectedYieVldTermStructureTest)

BOOST_AUTO_TEST_CASE(testCorrectedYts) {

    SavedSettings backup;

    Date refDate(18, Feb, 2016);
    Settings::instance().evaluationDate() = refDate;

    QuantLib::ext::shared_ptr<SimpleQuote> q = QuantLib::ext::make_shared<SimpleQuote>(0.02);
    Handle<Quote> hq(q);
    Handle<YieldTermStructure> floating(QuantLib::ext::make_shared<FlatForward>(0, NullCalendar(), hq, Actual365Fixed()));

    Handle<YieldTermStructure> source(QuantLib::ext::make_shared<FlatForward>(refDate, 0.02, Actual365Fixed()));
    Handle<YieldTermStructure> target(QuantLib::ext::make_shared<FlatForward>(refDate, 0.03, Actual365Fixed()));

    // we can not test the flavours of roll down with flat
    // termstructures, todo add tests with non-flat ts for this

    Handle<YieldTermStructure> corrected1(
        QuantLib::ext::make_shared<StaticallyCorrectedYieldTermStructure>(floating, source, target, ConstantDiscounts));
    Handle<YieldTermStructure> corrected2(
        QuantLib::ext::make_shared<StaticallyCorrectedYieldTermStructure>(floating, source, target, ForwardForward));

    Real tol = 1.0E-12;

    BOOST_CHECK_MESSAGE(std::fabs(target->discount(1.0) - corrected1->discount(1.0)) < tol,
                        "can not verify corrected1 df ("
                            << corrected1->discount(1.0) << ") against target df (" << target->discount(1.0)
                            << ") on eval date " << Settings::instance().evaluationDate() << ", difference is "
                            << (corrected1->discount(1.0) - target->discount(1.0)) << ", tolerance is " << tol);
    BOOST_CHECK_MESSAGE(std::fabs(target->discount(1.0) - corrected2->discount(1.0)) < tol,
                        "can not verify corrected2 df ("
                            << corrected2->discount(1.0) << ") against target df (" << target->discount(1.0)
                            << ") on eval date " << Settings::instance().evaluationDate() << ", difference is "
                            << (corrected2->discount(1.0) - target->discount(1.0)) << ", tolerance is " << tol);

    // move market data
    q->setValue(0.03);

    BOOST_CHECK_MESSAGE(std::fabs(floating->discount(1.0) * target->discount(1.0) / source->discount(1.0) -
                                  corrected1->discount(1.0)) < tol,
                        "can not verify corrected1 df ("
                            << corrected1->discount(1.0) << ") against expected df ("
                            << floating->discount(1.0) * target->discount(1.0) / source->discount(1.0)
                            << ") on eval date " << Settings::instance().evaluationDate() << ", difference is "
                            << (corrected1->discount(1.0) -
                                floating->discount(1.0) * target->discount(1.0) / source->discount(1.0))
                            << ", tolerance is " << tol);
    BOOST_CHECK_MESSAGE(std::fabs(floating->discount(1.0) * target->discount(1.0) / source->discount(1.0) -
                                  corrected2->discount(1.0)) < tol,
                        "can not verify corrected2 df ("
                            << corrected2->discount(1.0) << ") against expected df ("
                            << floating->discount(1.0) * target->discount(1.0) / source->discount(1.0)
                            << ") on eval date " << Settings::instance().evaluationDate() << ", difference is "
                            << (corrected2->discount(1.0) -
                                floating->discount(1.0) * target->discount(1.0) / source->discount(1.0))
                            << ", tolerance is " << tol);

    // move forward in time
    Settings::instance().evaluationDate() = Date(18, Feb, 2022);
    Real t = Actual365Fixed().yearFraction(refDate, Settings::instance().evaluationDate());

    BOOST_CHECK_MESSAGE(std::fabs(floating->discount(1.0) * target->discount(1.0) / source->discount(1.0) -
                                  corrected1->discount(1.0)) < tol,
                        "can not verify corrected1 df ("
                            << corrected1->discount(1.0) << ") against expected df ("
                            << floating->discount(1.0) * target->discount(1.0) / source->discount(1.0)
                            << ") on eval date " << Settings::instance().evaluationDate() << ", difference is "
                            << (corrected1->discount(1.0) -
                                floating->discount(1.0) * target->discount(1.0) / source->discount(1.0))
                            << ", tolerance is " << tol);
    BOOST_CHECK_MESSAGE(std::fabs(floating->discount(1.0) * target->discount(t + 1.0) * source->discount(t) /
                                      (target->discount(t) * source->discount(t + 1.0)) -
                                  corrected2->discount(1.0)) < tol,
                        "can not verify corrected2 df ("
                            << corrected2->discount(1.0) << ") against expected df ("
                            << floating->discount(1.0) * target->discount(t + 1.0) * source->discount(t) /
                                   (target->discount(t) * source->discount(t + 1.0))
                            << ") on eval date " << Settings::instance().evaluationDate() << ", difference is "
                            << floating->discount(1.0) * target->discount(t + 1.0) * source->discount(t) /
                                       (target->discount(t) * source->discount(t + 1.0)) -
                                   corrected2->discount(1.0)
                            << ", tolerance is " << tol);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
