/*! \file /OREAnalytics/orea/saccrv/tradeparsers/utils.cpp
    \brief SACCRV parser utilities
    \ingroup saccrv/tradeparsers
*/

#include <stdexcept>
#include <cctype>
#include <iostream>
#include <optional>

#include <ql/time/date.hpp>
#include <ql/time/daycounters/actual365fixed.hpp>
#include <ql/time/period.hpp>
#include <ql/time/daycounters/actualactual.hpp>

#include <orea/saccrv/tradeparsers/utils.hpp>


namespace ore {
namespace analytics {

double parseTenor(const std::string& tenor) {
    // Check if the string is empty
    if (tenor.empty()) {
        throw std::invalid_argument("Tenor string is empty");
    }

    // Extract the number and the unit (D, M, Y)
    std::size_t pos = 0;
    while (pos < tenor.size() && std::isdigit(tenor[pos])) {
        ++pos;
    }

    // Convert the numeric part to a number
    double number = std::stod(tenor.substr(0, pos));

    // Get the unit (the rest of the string)
    std::string unit = tenor.substr(pos);

    // Convert the tenor to years based on the unit
    if (unit == "D" || unit == "d") {
        // Convert days to years
        return number / 365.0;
    } else if (unit == "M" || unit == "m") {
        // Convert months to years
        return number / 12.0;
    } else if (unit == "Y" || unit == "y") {
        // Already in years
        return number;
    } else {
        throw std::invalid_argument("Invalid tenor unit: " + unit);
    }
}

int periodToDays(const QuantLib::Period& period) {
    int days = 0;

    // Convert the period to days
    switch (period.units()) {
        case QuantLib::Days:
            days = period.length();
            break;
        case QuantLib::Weeks:
            days = period.length() * 7;
            break;
        case QuantLib::Months:
            // Approximate conversion, assuming an average month length of 30 days
            days = period.length() * 30;
            break;
        case QuantLib::Years:
            // Approximate conversion, assuming 365 days in a year
            days = period.length() * 365;
            break;
        default:
            throw std::runtime_error("Unsupported period unit for conversion to days.");
    }

    return days;
}


double dateToFloatYears(const QuantLib::Date& asOfDate, const QuantLib::Date& targetDate) {
    QuantLib::DayCounter dayCounter = QuantLib::Actual365Fixed();
    return dayCounter.yearFraction(asOfDate, targetDate);
}

QuantLib::Date stringToDate(const std::string& dateString) {
    // Check for date format and remove '-' if present
    std::string cleanDateString;
    if (dateString.find('-') != std::string::npos) {
        cleanDateString = dateString.substr(0, 4) + dateString.substr(5, 2) + dateString.substr(8, 2);
    } else {
        cleanDateString = dateString;
    }

    // Assumes cleanDateString is in YYYYMMDD format
    QuantLib::Year year = std::stoi(cleanDateString.substr(0, 4));
    QuantLib::Month month = static_cast<QuantLib::Month>(std::stoi(cleanDateString.substr(4, 2)));
    QuantLib::Day day = std::stoi(cleanDateString.substr(6, 2));

    return QuantLib::Date(day, month, year);
}

std::pair<float, float> calculateDuration(const QuantLib::Date& asOfDate, 
                                          const std::string& startDateStr, 
                                          const std::string& endDateStr) {

    QuantLib::Date startDate = stringToDate(startDateStr);
    QuantLib::Date endDate = stringToDate(endDateStr);

    // Specify the ActualActual convention. Here, I am using ISDA, but you can change it as needed.
    QuantLib::ActualActual dayCounter(QuantLib::ActualActual::ISDA);

    float durationFromStart = 0.0;
    if (asOfDate > startDate) {
        durationFromStart = dayCounter.yearFraction(startDate, asOfDate);
    }

    float durationToEnd = dayCounter.yearFraction(asOfDate, endDate);
    
    return {durationFromStart, durationToEnd};
}




std::ostream& operator<<(std::ostream& os, const std::optional<double>& opt) {
    if (opt) {
        os << *opt; // Dereference the optional if it contains a value
    } else {
        os << "null"; // Print 'null' or some other placeholder if the optional is empty
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const SaccrvTrades& trade) {
    os << "id: " << trade.id << "\n";
    os << "tradeType: " << trade.tradeType << "\n";
    os << "TradeObj: " << trade.tradeGroup << "\n";
    os << "subClass: " << trade.subClass << "\n";
    os << "counterparty: " << trade.counterparty << "\n";
    os << "nettingSet: " << trade.nettingSet << "\n";
    os << "notional: " << trade.notional << "\n";
    os << "mTm: " << trade.mTm << "\n";
    os << "currency: " << trade.currency << "\n";
    os << "Si: " << trade.Si << "\n";
    os << "Ei: " << trade.Ei << "\n";
    os << "Ti: " << trade.Ti << "\n";
    os << "buySell: " << trade.buySell << "\n";
    os << "optionType: " << trade.optionType << "\n";
    os << "underlyingPrice: " << trade.underlyingPrice << "\n";
    os << "strikePrice: " << trade.strikePrice << "\n";
    os << "tradedPrice: " << trade.tradedPrice << "\n";
    os << "commodityType: " << trade.commodityType << "\n";
    os << "refEntity: " << trade.refEntity << "\n";
    os << "ccyPair: " << trade.ccyPair << "\n";
    os << "payLegType: " << trade.payLegType << "\n";
    os << "payLegRef: " << trade.payLegRef << "\n";
    os << "payLegTenor: " << trade.payLegTenor << "\n";
    os << "recLegType: " << trade.recLegType << "\n";
    os << "recLegRef: " << trade.recLegRef << "\n";
    os << "recLegTenor: " << trade.recLegTenor << "\n";
    os << "volStrike: " << trade.volStrike << "\n";
    os << "deliveryType: " << trade.deliveryType << "\n";
    os << "underlyingInstrument: " << trade.underlyingInstrument << "\n";
    os << "exoticType: " << trade.exoticType << "\n";
    os << "ISIN: " << trade.ISIN << "\n";
    os << "couponType: " << trade.couponType << "\n";
    os << "issuer: " << trade.issuer << "\n";
    os << "Maturity: " << trade.maturityDate << "\n";
    os << "paymentFrequency: " << trade.paymentFrequency << "\n";
    os << "creditRiskWeight: " << trade.creditRiskWeight << "\n";
    os << "cdoAttachPoint: " << trade.cdoAttachPoint << "\n";
    os << "cdoDetachPoint: " << trade.cdoDetachPoint << "\n";
    os << "fxNearLegFields: " << trade.fxNearLegFields << "\n";
    os << "ccyPaying: " << trade.ccyPaying << "\n";
    os << "baseCcy: " << trade.baseCcy << "\n";
    os << "amountPaying: " << trade.amountPaying << "\n";
    os << "ccyReceiving: " << trade.ccyReceiving << "\n";
    os << "amountReceiving: " << trade.amountReceiving << "\n";
    os << "timeBucket: " << trade.timeBucket << "\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, const SaccrvCsa& csa) {
    os << "ID:             " << csa.id << "\n"
       << "Active CSA:     " << csa.active << "\n"
       << "Threshold Cpty: " << csa.thresCpty << "\n"
       << "Threshold PO:   " << csa.thresPO << "\n"
       << "IM Cpty:        " << csa.imCpty << "\n"
       << "IM PO:          " << csa.imPO << "\n"
       << "MTA Cpty:       " << csa.mtaCpty << "\n"
       << "MTA PO:         " << csa.mtaPO << "\n"
       << "MPOR Days:      " << csa.mporDays << "\n"
       << "Re-margin Freq: " << csa.remarginFreq << "\n"
       << "counterparty:   " << csa.counterparty << "\n"
       << "currency:       " << csa.currency << "\n"
       << "Trade Groups:   " << csa.tradeGroups << "\n"
       << "Values Type:    " << csa.valuesType << "\n"
       << "Has Illiquid Collateral: " << csa.hasIlliquidCollateralOrOTCNotEasilyReplaced << "\n"
       << "Apply Margin Call Disputes: " << csa.applyMarginCallDisputes << "\n"
       << "GreaterThan5000TransactionsNotCentrallyCleared: " << csa.greaterThan5000TransactionsNotCentrallyCleared << "\n";

    return os;
}

} // namespace analytics
} // namespace ore
