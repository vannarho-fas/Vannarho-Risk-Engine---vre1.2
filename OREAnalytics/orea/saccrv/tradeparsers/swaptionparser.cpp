/*! \file /OREAnalytics/orea/saccrv/tradeparsers/swapparser.cpp
    \brief SACCRV parser for Swap trades, which populates a SaccrvTrades struct
    \ingroup saccrv/tradeparsers
*/


#include <string>
#include <stdexcept>
#include <cctype>
#include <sstream>


#include <ored/portfolio/trade.hpp>
#include <ored/portfolio/swaption.hpp>
#include <orea/saccrv/tradeparsers/swaptionparser.hpp>
#include <orea/saccrv/tradeparsers/utils.hpp>


using namespace ore::data;
using namespace boost::filesystem;

namespace ore {
namespace analytics {


SaccrvTrades SwaptionParser::parse(const boost::shared_ptr<ore::data::Trade>& saccrvTrade) {
    SaccrvTrades tradeInputs;

    LOG("SACCRV: SwaptionParser::parse");
    auto asOfDate = QuantLib::Settings::instance().evaluationDate();

    // Assuming saccrvTrade is a valid Swaption trade
    LOG("SACCRV: SwaptionParser::parse - Parsing Swaption trade");
    boost::shared_ptr<ore::data::Swaption> castedTrade = boost::dynamic_pointer_cast<ore::data::Swaption>(saccrvTrade);

    std::string startDateStr = "";
    std::string endDateStr = "";

    if (!castedTrade) {
        // Handle the case where the cast fails
        LOG("Failed to cast to Swaption");
        return tradeInputs; // Return empty or default-initialized struct
    }

    // Populate SaccrvTrades
    tradeInputs.id = castedTrade->id();
    tradeInputs.tradeType = "Swaption";
    tradeInputs.tradeGroup = "IRD";
    tradeInputs.subClass = "";
    tradeInputs.counterparty = castedTrade->envelope().counterparty();
    tradeInputs.nettingSet = castedTrade->envelope().nettingSetId();
    tradeInputs.timeBucket = 0;

    // Extract swaption specific data
    tradeInputs.buySell = castedTrade->optionData().longShort() == "Long" ? "Buy" : "Sell";
    tradeInputs.optionType = castedTrade->optionData().callPut();

    LOG("SACCRV: SwaptionParser::parse - Extracting leg data");

    // Extract leg data optionData()
    for (const auto& legData : castedTrade->legData()) {
        LOG("SACCRV: SwaptionParser - inside lgdata loop");
        if (legData.isPayer()) {
            LOG("SACCRV: SwaptionParser - inside payer leg");
            tradeInputs.payLegType = legData.legType();
            if (!legData.schedule().rules().empty() && !legData.schedule().rules().front().tenor().empty()) {
                double tenorYears = ore::analytics::parseTenor(legData.schedule().rules().front().tenor());
                tradeInputs.payLegTenor = tenorYears;
                LOG("SACCRV: SwaptionParser:  - payLegTenor = " << (tradeInputs.payLegTenor.has_value() ? std::to_string(tradeInputs.payLegTenor.value()) : "not set"));
            }
        } else {
            LOG("SACCRV: SwaptionParser - inside receiver leg");
            tradeInputs.recLegType = legData.legType();
            LOG("SACCRV: SwaptionParser:  - recLegType = " << tradeInputs.recLegType);
            if (!legData.schedule().rules().empty() && !legData.schedule().rules().front().tenor().empty()) {
                double tenorYears = ore::analytics::parseTenor(legData.schedule().rules().front().tenor());
                tradeInputs.recLegTenor = tenorYears;
                LOG("SACCRV: SwaptionParser:  - recLegTenor = " << (tradeInputs.recLegTenor.has_value() ? std::to_string(tradeInputs.recLegTenor.value()) : "not set"));
            
            }
        }
        if (!legData.notionals().empty()) {
            LOG("SACCRV: SwaptionParser - inside notional loop");
            tradeInputs.notional = legData.notionals().front();
        }
        
        tradeInputs.currency = legData.currency();
        const auto& firstScheduleRule = legData.schedule().rules().front();
        startDateStr = firstScheduleRule.startDate(); // Accessing startDate "20160301"; 
        endDateStr = firstScheduleRule.endDate();     // Accessing endDate "20360301"
        LOG("SACCRV: SwaptionParser::parse - StartDate: " << startDateStr << " EndDate: " << endDateStr);
    }

    LOG("SACCRV: SwaptionParser::parse - Extracting additional swaption data");

    if (!startDateStr.empty() && !endDateStr.empty()) {
        auto durations = ore::analytics::calculateDuration(asOfDate, startDateStr, endDateStr);
        tradeInputs.Si = durations.first;
        tradeInputs.Ei = durations.second;
    }
    return tradeInputs;
}

} // namespace analytics
} // namespace ore
