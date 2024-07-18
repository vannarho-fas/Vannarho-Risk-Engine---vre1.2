/*! \file /OREAnalytics/orea/saccrv/tradeparsers/swapparser.cpp
    \brief SACCRV parser for Swap trades, which populates a SaccrvTrades struct
    \ingroup saccrv/tradeparsers
*/


#include <string>
#include <stdexcept>
#include <cctype>
#include <sstream>


#include <ored/portfolio/trade.hpp>
#include <ored/portfolio/swap.hpp>
#include <orea/saccrv/tradeparsers/swapparser.hpp>
#include <orea/saccrv/tradeparsers/utils.hpp>


using namespace ore::data;
using namespace boost::filesystem;

namespace ore {
namespace analytics {


SaccrvTrades SwapParser::parse(const boost::shared_ptr<ore::data::Trade>& saccrvTrade) {
    SaccrvTrades tradeInputs;

    LOG("SACCRV: SwapParser::parse");
    auto asOfDate = QuantLib::Settings::instance().evaluationDate();

    // Assuming saccrvTrade is a valid Swap trade
    boost::shared_ptr<ore::data::Swap> castedTrade = boost::dynamic_pointer_cast<ore::data::Swap>(saccrvTrade);

    if (!castedTrade) {
        // Handle the case where the cast fails
        LOG("Failed to cast to Swap");
        return tradeInputs; // Return empty or default-initialized struct
    }

    // Populate SaccrvTrades
    tradeInputs.id = castedTrade->id();
    LOG("SACCRV: TradeId = " << castedTrade->id() );
    tradeInputs.tradeType = "Swap";
    tradeInputs.tradeGroup = "IRD";
    tradeInputs.subClass = "";
    tradeInputs.counterparty = castedTrade->envelope().counterparty();
    tradeInputs.timeBucket = 0;


    // Envelope details
    // std::set<std::string> portfolioIdsSet = castedTrade->envelope().portfolioIds();
    // std::vector<std::string> portfolioIdsVector(portfolioIdsSet.begin(), portfolioIdsSet.end());
    // tradeInputs.PortfolioIds = portfolioIdsVector;
    tradeInputs.counterparty = castedTrade->envelope().counterparty();
    tradeInputs.nettingSet = castedTrade->envelope().nettingSetId();
    std::string startDateStr = "";
    std::string endDateStr = "";

    LOG("SACCRV: enter legdata");
    // Leg details - Assuming only two legs: one pay, one receive
    for (const auto& legData : castedTrade->legData()) {
        if (legData.isPayer()) {
            LOG("SACCRV: Payer");
            tradeInputs.payLegType = legData.concreteLegData()->legType();
            LOG("SACCRV: tenor - " << legData.schedule().rules().front().tenor());
            // Assuming tenor from the first date
            if (!legData.schedule().rules().empty()) {
                LOG("SACCRV: parseTenor Payer");
                double tenorYears = ore::analytics::parseTenor(legData.schedule().rules().front().tenor());
                LOG("SACCRV: parseTenor Payer done");
                tradeInputs.payLegTenor = tenorYears;
            }
            // if (!legData.schedule().rules().empty()) {
            const auto& firstScheduleRule = legData.schedule().rules().front();
            startDateStr = firstScheduleRule.startDate(); // Accessing startDate "20160301"; 
            endDateStr = firstScheduleRule.endDate();     // Accessing endDate "20360301"
            // }
            tradeInputs.ccyPaying = legData.currency();
        } else {
            LOG("SACCRV: Rec");
            tradeInputs.recLegType = legData.concreteLegData()->legType();
            // Assuming tenor from the first date
            if (!legData.schedule().rules().empty()) {
                LOG("SACCRV: parseTenor rec");
                double tenorYears = ore::analytics::parseTenor(legData.schedule().rules().front().tenor());
                LOG("SACCRV: parseTenor rec done");
                tradeInputs.recLegTenor = tenorYears;
            }
            tradeInputs.ccyReceiving = legData.currency();
        }
        // notional - Assuming using the first notional for simplicity
        if (!legData.notionals().empty()) {
            tradeInputs.notional = legData.notionals().front();
        }
        tradeInputs.currency = legData.currency();
    }

        
        // Use startDateStr and endDateStr here
    if (!startDateStr.empty() && !endDateStr.empty()) {
        LOG("SACCRV: set durations. Asof - " << asOfDate << "start - " << startDateStr << " End - " << endDateStr);
        auto durations = ore::analytics::calculateDuration(asOfDate, startDateStr, endDateStr);
        tradeInputs.Si = durations.first;
        tradeInputs.Ei = durations.second;
        LOG("SACCRV: durations set")
        // CONSOLE("SACCRV: SwapParser::parse - Si: " << (tradeInputs.Si.has_value() ? std::to_string(tradeInputs.Si.value()) : "No value") 
        // << " Ei: " << (tradeInputs.Ei.has_value() ? std::to_string(tradeInputs.Ei.value()) : "No value"));
    }

    // // Maturity and Payment Frequency
    // auto maturityDate = castedTrade->maturity();
    // tradeInputs.maturityDate = ore::analytics::dateToFloatYears(asOfDate, maturityDate);

    // std::ostringstream oss;
    // oss << tradeInputs;
    // std::cout << tradeInputs << std::endl;
    // CONSOLE("SACCRV: swap parser completed");
    return tradeInputs;
}

} // namespace analytics
} // namespace ore
