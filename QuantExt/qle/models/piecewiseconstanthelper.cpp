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

#include <qle/models/piecewiseconstanthelper.hpp>

#include <ql/termstructures/yieldtermstructure.hpp>

namespace QuantExt {

namespace {

void checkTimes(const Array& t) {
    if (t.size() == 0)
        return;
    QL_REQUIRE(t.front() > 0.0, "first time (" << t.front() << ") must be positive");
    for (Size i = 0; i < t.size() - 1; ++i) {
        QL_REQUIRE(t[i] < t[i + 1], "times must be strictly increasing, entries at ("
                                        << i << "," << i + 1 << ") are (" << t[i] << "," << t[i + 1] << ")");
    }
}

Array datesToTimes(const std::vector<Date>& dates, const Handle<YieldTermStructure>& yts) {
    Array res(dates.size());
    for (Size i = 0; i < dates.size(); ++i) {
        res[i] = yts->timeFromReference(dates[i]);
    }
    return res;
}

} // anonymous namespace

PiecewiseConstantHelper1::PiecewiseConstantHelper1(const Array& t,
    const QuantLib::ext::shared_ptr<Constraint>& constraint)
    : t_(t), y_(QuantLib::ext::make_shared<PseudoParameter>(t.size() + 1, *constraint)) {
    checkTimes(t_);
}

PiecewiseConstantHelper1::PiecewiseConstantHelper1(const std::vector<Date>& dates,
    const Handle<YieldTermStructure>& yts,
    const QuantLib::ext::shared_ptr<Constraint>& constraint)
    : t_(datesToTimes(dates, yts)),
      y_(QuantLib::ext::make_shared<PseudoParameter>(dates.size() + 1, *constraint)) {
    checkTimes(t_);
}

PiecewiseConstantHelper11::PiecewiseConstantHelper11(const Array& t1, const Array& t2,
    const QuantLib::ext::shared_ptr<Constraint>& constraint1,
    const QuantLib::ext::shared_ptr<Constraint>& constraint2)
    : h1_(t1, constraint1), h2_(t2, constraint2) {}

PiecewiseConstantHelper11::PiecewiseConstantHelper11(const std::vector<Date>& dates1,
    const std::vector<Date>& dates2,
    const Handle<YieldTermStructure>& yts,
    const QuantLib::ext::shared_ptr<Constraint>& constraint1,
    const QuantLib::ext::shared_ptr<Constraint>& constraint2)
    : h1_(dates1, yts, constraint1), h2_(dates2, yts, constraint2) {}

PiecewiseConstantHelper2::PiecewiseConstantHelper2(const Array& t,
    const QuantLib::ext::shared_ptr<Constraint>& constraint)
    : zeroCutoff_(1.0E-6), t_(t),
      y_(QuantLib::ext::make_shared<PseudoParameter>(t.size() + 1, *constraint)) {
    checkTimes(t_);
}

PiecewiseConstantHelper2::PiecewiseConstantHelper2(const std::vector<Date>& dates,
    const Handle<YieldTermStructure>& yts,
    const QuantLib::ext::shared_ptr<Constraint>& constraint)
    : zeroCutoff_(1.0E-6), t_(datesToTimes(dates, yts)),
      y_(QuantLib::ext::make_shared<PseudoParameter>(dates.size() + 1, *constraint)) {
    checkTimes(t_);
}

PiecewiseConstantHelper2::PiecewiseConstantHelper2(const Array& t, const QuantLib::ext::shared_ptr<PseudoParameter>& p)
    : zeroCutoff_(1.0E-6), t_(t), y_(p) {}

PiecewiseConstantHelper3::PiecewiseConstantHelper3(const Array& t1, const Array& t2,
    const QuantLib::ext::shared_ptr<Constraint>& constraint1,
    const QuantLib::ext::shared_ptr<Constraint>& constraint2)
    : zeroCutoff_(1.0E-6), t1_(t1), t2_(t2),
      y1_(QuantLib::ext::make_shared<PseudoParameter>(t1.size() + 1, *constraint1)),
      y2_(QuantLib::ext::make_shared<PseudoParameter>(t2.size() + 1, *constraint2)) {
    checkTimes(t1_);
    checkTimes(t2_);
}

PiecewiseConstantHelper3::PiecewiseConstantHelper3(const std::vector<Date>& dates1,
    const std::vector<Date>& dates2,
    const Handle<YieldTermStructure>& yts,
    const QuantLib::ext::shared_ptr<Constraint>& constraint1,
    const QuantLib::ext::shared_ptr<Constraint>& constraint2)
    : zeroCutoff_(1.0E-6),
      t1_(datesToTimes(dates1, yts)),
      t2_(datesToTimes(dates2, yts)),
      y1_(QuantLib::ext::make_shared<PseudoParameter>(dates1.size() + 1, *constraint1)),
      y2_(QuantLib::ext::make_shared<PseudoParameter>(dates2.size() + 1, *constraint2)) {
    checkTimes(t1_);
    checkTimes(t2_);
}

} // namespace QuantExt
