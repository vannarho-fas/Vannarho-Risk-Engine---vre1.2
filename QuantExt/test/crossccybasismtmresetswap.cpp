/*
 Copyright (C) 2018 Quaternion Risk Management Ltd
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
#include <boost/make_shared.hpp>
#include <boost/test/unit_test.hpp>
#include <ql/cashflows/couponpricer.hpp>
#include <ql/cashflows/floatingratecoupon.hpp>
#include <ql/currencies/all.hpp>
#include <ql/indexes/ibor/gbplibor.hpp>
#include <ql/indexes/ibor/usdlibor.hpp>
#include <ql/quotes/simplequote.hpp>
#include <ql/termstructures/yield/discountcurve.hpp>
#include <ql/time/calendars/all.hpp>
#include <ql/time/daycounters/actual360.hpp>
#include <ql/types.hpp>
#include <qle/instruments/crossccybasismtmresetswap.hpp>
#include <qle/pricingengines/crossccyswapengine.hpp>

using namespace std;
using namespace boost::unit_test_framework;
using namespace QuantLib;
using namespace QuantExt;

namespace {

Handle<YieldTermStructure> USDDiscountCurve() {

    vector<Date> dates(27);
    vector<DiscountFactor> dfs(27);
    Actual365Fixed dayCounter;

    dates[0] = Date(11, Sep, 2018);
    dfs[0] = 1;
    dates[1] = Date(14, Sep, 2018);
    dfs[1] = 0.99994666951096;
    dates[2] = Date(20, Sep, 2018);
    dfs[2] = 0.999627719221066;
    dates[3] = Date(27, Sep, 2018);
    dfs[3] = 0.999254084816959;
    dates[4] = Date(04, Oct, 2018);
    dfs[4] = 0.998837020905631;
    dates[5] = Date(15, Oct, 2018);
    dfs[5] = 0.998176132423265;
    dates[6] = Date(13, Nov, 2018);
    dfs[6] = 0.99644587210048;
    dates[7] = Date(13, Dec, 2018);
    dfs[7] = 0.994644668243218;
    dates[8] = Date(14, Jan, 2019);
    dfs[8] = 0.992596634984033;
    dates[9] = Date(13, Feb, 2019);
    dfs[9] = 0.990636503861861;
    dates[10] = Date(13, Mar, 2019);
    dfs[10] = 0.988809127958345;
    dates[11] = Date(13, Jun, 2019);
    dfs[11] = 0.982417991680868;
    dates[12] = Date(13, Sep, 2019);
    dfs[12] = 0.975723193871552;
    dates[13] = Date(13, Mar, 2020);
    dfs[13] = 0.96219213956104;
    dates[14] = Date(14, Sep, 2020);
    dfs[14] = 0.948588232418325;
    dates[15] = Date(13, Sep, 2021);
    dfs[15] = 0.92279636773464;
    dates[16] = Date(13, Sep, 2022);
    dfs[16] = 0.898345201557914;
    dates[17] = Date(13, Sep, 2023);
    dfs[17] = 0.874715322269088;
    dates[18] = Date(15, Sep, 2025);
    dfs[18] = 0.828658611114833;
    dates[19] = Date(13, Sep, 2028);
    dfs[19] = 0.763030152740947;
    dates[20] = Date(13, Sep, 2030);
    dfs[20] = 0.722238847877756;
    dates[21] = Date(13, Sep, 2033);
    dfs[21] = 0.664460629674362;
    dates[22] = Date(13, Sep, 2038);
    dfs[22] = 0.580288693473926;
    dates[23] = Date(14, Sep, 2043);
    dfs[23] = 0.510857007600479;
    dates[24] = Date(14, Sep, 2048);
    dfs[24] = 0.44941525649436;
    dates[25] = Date(13, Sep, 2058);
    dfs[25] = 0.352389176933952;
    dates[26] = Date(13, Sep, 2068);
    dfs[26] = 0.28183300653329;

    return Handle<YieldTermStructure>(QuantLib::ext::make_shared<DiscountCurve>(dates, dfs, dayCounter));
}

Handle<YieldTermStructure> USDProjectionCurve() {

    vector<Date> dates(25);
    vector<DiscountFactor> dfs(25);
    Actual365Fixed dayCounter;

    dates[0] = Date(11, Sep, 2018);
    dfs[0] = 1;
    dates[1] = Date(13, Dec, 2018);
    dfs[1] = 0.994134145990132;
    dates[2] = Date(19, Dec, 2018);
    dfs[2] = 0.993695776146116;
    dates[3] = Date(20, Mar, 2019);
    dfs[3] = 0.987047992958673;
    dates[4] = Date(19, Jun, 2019);
    dfs[4] = 0.980016364694049;
    dates[5] = Date(18, Sep, 2019);
    dfs[5] = 0.972708376777628;
    dates[6] = Date(18, Dec, 2019);
    dfs[6] = 0.965277162951128;
    dates[7] = Date(18, Mar, 2020);
    dfs[7] = 0.957799302363697;
    dates[8] = Date(14, Sep, 2020);
    dfs[8] = 0.943264331984248;
    dates[9] = Date(13, Sep, 2021);
    dfs[9] = 0.914816470778467;
    dates[10] = Date(13, Sep, 2022);
    dfs[10] = 0.88764714641623;
    dates[11] = Date(13, Sep, 2023);
    dfs[11] = 0.861475671008934;
    dates[12] = Date(13, Sep, 2024);
    dfs[12] = 0.835944798717806;
    dates[13] = Date(15, Sep, 2025);
    dfs[13] = 0.810833947617338;
    dates[14] = Date(14, Sep, 2026);
    dfs[14] = 0.78631849267276;
    dates[15] = Date(13, Sep, 2027);
    dfs[15] = 0.762267648509673;
    dates[16] = Date(13, Sep, 2028);
    dfs[16] = 0.738613627359076;
    dates[17] = Date(13, Sep, 2029);
    dfs[17] = 0.715502378943932;
    dates[18] = Date(13, Sep, 2030);
    dfs[18] = 0.693380472578176;
    dates[19] = Date(13, Sep, 2033);
    dfs[19] = 0.631097994110912;
    dates[20] = Date(13, Sep, 2038);
    dfs[20] = 0.540797634630251;
    dates[21] = Date(14, Sep, 2043);
    dfs[21] = 0.465599237331079;
    dates[22] = Date(14, Sep, 2048);
    dfs[22] = 0.402119473746341;
    dates[23] = Date(13, Sep, 2058);
    dfs[23] = 0.303129773289934;
    dates[24] = Date(13, Sep, 2068);
    dfs[24] = 0.23210070222569;

    return Handle<YieldTermStructure>(QuantLib::ext::make_shared<DiscountCurve>(dates, dfs, dayCounter));
}

Handle<YieldTermStructure> GBPDiscountCurve() {

    vector<Date> dates(27);
    vector<DiscountFactor> dfs(27);
    Actual365Fixed dayCounter;

    dates[0] = Date(11, Sep, 2018);
    dfs[0] = 1;
    dates[1] = Date(14, Sep, 2018);
    dfs[1] = 0.99994666951096;
    dates[2] = Date(20, Sep, 2018);
    dfs[2] = 0.999627719221066;
    dates[3] = Date(27, Sep, 2018);
    dfs[3] = 0.999254084816959;
    dates[4] = Date(04, Oct, 2018);
    dfs[4] = 0.998837020905631;
    dates[5] = Date(15, Oct, 2018);
    dfs[5] = 0.998176132423265;
    dates[6] = Date(13, Nov, 2018);
    dfs[6] = 0.99644587210048;
    dates[7] = Date(13, Dec, 2018);
    dfs[7] = 0.994644668243218;
    dates[8] = Date(14, Jan, 2019);
    dfs[8] = 0.992596634984033;
    dates[9] = Date(13, Feb, 2019);
    dfs[9] = 0.990636503861861;
    dates[10] = Date(13, Mar, 2019);
    dfs[10] = 0.988809127958345;
    dates[11] = Date(13, Jun, 2019);
    dfs[11] = 0.982417991680868;
    dates[12] = Date(13, Sep, 2019);
    dfs[12] = 0.975723193871552;
    dates[13] = Date(13, Mar, 2020);
    dfs[13] = 0.96219213956104;
    dates[14] = Date(14, Sep, 2020);
    dfs[14] = 0.948588232418325;
    dates[15] = Date(13, Sep, 2021);
    dfs[15] = 0.92279636773464;
    dates[16] = Date(13, Sep, 2022);
    dfs[16] = 0.898345201557914;
    dates[17] = Date(13, Sep, 2023);
    dfs[17] = 0.874715322269088;
    dates[18] = Date(15, Sep, 2025);
    dfs[18] = 0.828658611114833;
    dates[19] = Date(13, Sep, 2028);
    dfs[19] = 0.763030152740947;
    dates[20] = Date(13, Sep, 2030);
    dfs[20] = 0.722238847877756;
    dates[21] = Date(13, Sep, 2033);
    dfs[21] = 0.664460629674362;
    dates[22] = Date(13, Sep, 2038);
    dfs[22] = 0.580288693473926;
    dates[23] = Date(14, Sep, 2043);
    dfs[23] = 0.510857007600479;
    dates[24] = Date(14, Sep, 2048);
    dfs[24] = 0.44941525649436;
    dates[25] = Date(13, Sep, 2058);
    dfs[25] = 0.352389176933952;
    dates[26] = Date(13, Sep, 2068);
    dfs[26] = 0.28183300653329;

    return Handle<YieldTermStructure>(QuantLib::ext::make_shared<DiscountCurve>(dates, dfs, dayCounter));
}

Handle<YieldTermStructure> GBPProjectionCurve() {

    vector<Date> dates(25);
    vector<DiscountFactor> dfs(25);
    Actual365Fixed dayCounter;

    dates[0] = Date(11, Sep, 2018);
    dfs[0] = 1;
    dates[1] = Date(13, Dec, 2018);
    dfs[1] = 0.994134145990132;
    dates[2] = Date(19, Dec, 2018);
    dfs[2] = 0.993695776146116;
    dates[3] = Date(20, Mar, 2019);
    dfs[3] = 0.987047992958673;
    dates[4] = Date(19, Jun, 2019);
    dfs[4] = 0.980016364694049;
    dates[5] = Date(18, Sep, 2019);
    dfs[5] = 0.972708376777628;
    dates[6] = Date(18, Dec, 2019);
    dfs[6] = 0.965277162951128;
    dates[7] = Date(18, Mar, 2020);
    dfs[7] = 0.957799302363697;
    dates[8] = Date(14, Sep, 2020);
    dfs[8] = 0.943264331984248;
    dates[9] = Date(13, Sep, 2021);
    dfs[9] = 0.914816470778467;
    dates[10] = Date(13, Sep, 2022);
    dfs[10] = 0.88764714641623;
    dates[11] = Date(13, Sep, 2023);
    dfs[11] = 0.861475671008934;
    dates[12] = Date(13, Sep, 2024);
    dfs[12] = 0.835944798717806;
    dates[13] = Date(15, Sep, 2025);
    dfs[13] = 0.810833947617338;
    dates[14] = Date(14, Sep, 2026);
    dfs[14] = 0.78631849267276;
    dates[15] = Date(13, Sep, 2027);
    dfs[15] = 0.762267648509673;
    dates[16] = Date(13, Sep, 2028);
    dfs[16] = 0.738613627359076;
    dates[17] = Date(13, Sep, 2029);
    dfs[17] = 0.715502378943932;
    dates[18] = Date(13, Sep, 2030);
    dfs[18] = 0.693380472578176;
    dates[19] = Date(13, Sep, 2033);
    dfs[19] = 0.631097994110912;
    dates[20] = Date(13, Sep, 2038);
    dfs[20] = 0.540797634630251;
    dates[21] = Date(14, Sep, 2043);
    dfs[21] = 0.465599237331079;
    dates[22] = Date(14, Sep, 2048);
    dfs[22] = 0.402119473746341;
    dates[23] = Date(13, Sep, 2058);
    dfs[23] = 0.303129773289934;
    dates[24] = Date(13, Sep, 2068);
    dfs[24] = 0.23210070222569;

    return Handle<YieldTermStructure>(QuantLib::ext::make_shared<DiscountCurve>(dates, dfs, dayCounter));
}

QuantLib::ext::shared_ptr<CrossCcyBasisMtMResetSwap> makeTestSwap(Rate spotFx, Spread GBPSpread) {

    // USD nominal
    Real GBPNominal = 10000000.0;
    Handle<Quote> fxSpotQuote = Handle<Quote>(QuantLib::ext::make_shared<SimpleQuote>(spotFx));

    // Dates and calendars
    JointCalendar payCalendar = JointCalendar(UnitedStates(UnitedStates::Settlement), UnitedKingdom());
    Date referenceDate = Settings::instance().evaluationDate();
    referenceDate = payCalendar.adjust(referenceDate);
    Date start = payCalendar.advance(referenceDate, 2 * Days);
    Date end = start + 5 * Years;
    Schedule schedule(start, end, 3 * Months, payCalendar, ModifiedFollowing, ModifiedFollowing,
                      DateGeneration::Backward, false);

    // Indices
    QuantLib::ext::shared_ptr<IborIndex> USDindex = QuantLib::ext::make_shared<USDLibor>(3 * Months, USDProjectionCurve());
    QuantLib::ext::shared_ptr<IborIndex> GBPindex = QuantLib::ext::make_shared<GBPLibor>(3 * Months, GBPProjectionCurve());
    QuantLib::ext::shared_ptr<FxIndex> fxIndex = QuantLib::ext::make_shared<FxIndex>(
        "dummy", 0, GBPCurrency(), USDCurrency(), payCalendar, fxSpotQuote, GBPDiscountCurve(), USDDiscountCurve());

    // Create swap
    return QuantLib::ext::shared_ptr<CrossCcyBasisMtMResetSwap>(new CrossCcyBasisMtMResetSwap(
        GBPNominal, GBPCurrency(), schedule, GBPindex, GBPSpread, USDCurrency(), schedule, USDindex, 0.0, fxIndex));
}
} // namespace

BOOST_FIXTURE_TEST_SUITE(QuantExtTestSuite, qle::test::TopLevelFixture)

BOOST_AUTO_TEST_SUITE(CrossCcyBasisMtMResetSwapTest)

BOOST_AUTO_TEST_CASE(testSwapPricing) {

    BOOST_TEST_MESSAGE("Test cross currency MtM resetting swap pricing against known results");

    SavedSettings backup;
    Settings::instance().evaluationDate() = Date(11, Sep, 2018);

    // Create swap
    Rate spotFx = 1;
    Spread spread = 0;
    QuantLib::ext::shared_ptr<CrossCcyBasisMtMResetSwap> swap = makeTestSwap(spotFx, spread);

    // Attach pricing engine
    Handle<Quote> fxSpotQuote = Handle<Quote>(QuantLib::ext::make_shared<SimpleQuote>(spotFx));
    QuantLib::ext::shared_ptr<PricingEngine> engine = QuantLib::ext::make_shared<CrossCcySwapEngine>(
        USDCurrency(), USDDiscountCurve(), GBPCurrency(), GBPDiscountCurve(), fxSpotQuote);

    swap->setPricingEngine(engine);

    // Check values
    Real tol = 0.01;

    BOOST_CHECK_SMALL(swap->NPV(), tol);

    Real expBps = -4670.170509677384; // cached value
    BOOST_CHECK_SMALL(swap->legBPS(0) - expBps, tol);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
