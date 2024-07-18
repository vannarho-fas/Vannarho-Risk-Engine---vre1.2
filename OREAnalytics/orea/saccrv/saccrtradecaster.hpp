#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <memory>
#include <functional>
#include <ored/portfolio/trade.hpp>
#include <ored/portfolio/swap.hpp>


using namespace ore::data;
using namespace boost::filesystem;


namespace ore {
namespace analytics {


// Helper for casting trades to their specific types in order to access their data and populate the SaccrvTradeData struct

class TradeCaster {
public:
    TradeCaster() {
        // // Register the types and the corresponding casting lambdas
        // castMap["Swap"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::Swap>(trade);
        // };
        // castMap["Swaption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::Swaption>(trade);
        // };
        // castMap["CrossCurrencySwap"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::CrossCurrencySwap>(trade);
        // };
        // castMap["CommoditySpreadOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::CommoditySpreadOption>(trade);
        // };
        // castMap["EquityFutureOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::EquityFutureOption>(trade);
        // };
        // castMap["BondTRS"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::BondTRS>(trade);
        // };
        // castMap["CommodityOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::CommodityOption>(trade);
        // };
        // castMap["CapFloor"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::CapFloor>(trade);
        // };
        // castMap["FxDigitalOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::FxDigitalOption>(trade);
        // };
        // castMap["CommoditySwaption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::CommoditySwaption>(trade);
        // };
        // castMap["FxDigitalBarrierOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::FxDigitalBarrierOption>(trade);
        // };
        // castMap["ForwardRateAgreement"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::ForwardRateAgreement>(trade);
        // };
        // castMap["CommodityDigitalAveragePriceOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::CommodityDigitalAveragePriceOption>(trade);
        // };
        // castMap["CommoditySwap"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::CommoditySwap>(trade);
        // };
        // castMap["EquitySwap"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::EquitySwap>(trade);
        // };
        // castMap["FxForward"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::FxForward>(trade);
        // };
        // castMap["BondRepo"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::BondRepo>(trade);
        // };
        // castMap["FxAverageForward"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::FxAverageForward>(trade);
        // };
        // castMap["FxEuropeanBarrierOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::FxEuropeanBarrierOption>(trade);
        // };
        // castMap["FxTouchOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::FxTouchOption>(trade);
        // };
        // castMap["EquityAsianOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::EquityAsianOption>(trade);
        // };
        // castMap["FxAsianOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::FxAsianOption>(trade);
        // };
        // castMap["CommodityAsianOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::CommodityAsianOption>(trade);
        // };
        // // castMap["EquityVarianceSwap"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        // //     return boost::dynamic_pointer_cast<ore::data::EquityVarianceSwap>(trade);
        // // };
        // // castMap["FxVarianceSwap"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        // //     return boost::dynamic_pointer_cast<ore::data::FxVarianceSwap>(trade);
        // // };
        // // castMap["CommodityVarianceSwap"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        // //     return boost::dynamic_pointer_cast<ore::data::CommodityVarianceSwap>(trade);
        // // };
        // castMap["FxDoubleTouchOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::FxDoubleTouchOption>(trade);
        // };
        // castMap["FxDoubleBarrierOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::FxDoubleBarrierOption>(trade);
        // };
        // castMap["EquityBarrierOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::EquityBarrierOption>(trade);
        // };
        // castMap["FxSwap"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::FxSwap>(trade);
        // };
        // castMap["EquityTouchOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::EquityTouchOption>(trade);
        // };
        // castMap["EquityDigitalOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::EquityDigitalOption>(trade);
        // };
        // castMap["CompositeTrade"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::CompositeTrade>(trade);
        // };
        // castMap["MultiLegOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::MultiLegOption>(trade);
        // };
        // castMap["IndexCreditDefaultSwap"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::IndexCreditDefaultSwap>(trade);
        // };
        // castMap["CommodityForward"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::CommodityForward>(trade);
        // };
        // castMap["EquityCliquetOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::EquityCliquetOption>(trade);
        // };
        // castMap["CommodityDigitalOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::CommodityDigitalOption>(trade);
        // };
        // castMap["EquityForward"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::EquityForward>(trade);
        // };
        // castMap["IndexCreditDefaultSwapOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::IndexCreditDefaultSwapOption>(trade);
        // };
        // castMap["CommodityAveragePriceOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::CommodityAveragePriceOption>(trade);
        // };
        // castMap["CreditDefaultSwapOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::CreditDefaultSwapOption>(trade);
        // };
        // castMap["Failed"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::FailedTrade>(trade);
        // };
        // castMap["ForwardBond"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::ForwardBond>(trade);
        // };
        // castMap["EquityDoubleTouchOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::EquityDoubleTouchOption>(trade);
        // };
        // castMap["CommodityOptionStrip"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::CommodityOptionStrip>(trade);
        // };
        // castMap["SyntheticCDO"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::SyntheticCDO>(trade);
        // };
        // castMap["Bond"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::Bond>(trade);
        // };
        // castMap["CreditLinkedSwap"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::CreditLinkedSwap>(trade);
        // };
        // castMap["EquityEuropeanBarrierOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::EquityEuropeanBarrierOption>(trade);
        // };
        // castMap["InflationSwap"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::InflationSwap>(trade);
        // };
        // castMap["EquityDoubleBarrierOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::EquityDoubleBarrierOption>(trade);
        // };
        // castMap["BondOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::BondOption>(trade);
        // };
        // castMap["CreditDefaultSwap"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::CreditDefaultSwap>(trade);
        // };
        // castMap["FxKIKOBarrierOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::FxKIKOBarrierOption>(trade);
        // };
        // castMap["FxBarrierOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::FxBarrierOption>(trade);
        // };
        // castMap["EquityOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::EquityOption>(trade);
        // };
        // castMap["FxOption"] = [](const boost::shared_ptr<ore::data::Trade>& trade) -> boost::shared_ptr<ore::data::Trade> {
        //     return boost::dynamic_pointer_cast<ore::data::FxOption>(trade);
        // };


    }

    std::function<boost::shared_ptr<ore::data::Trade>(const boost::shared_ptr<ore::data::Trade>&)> getCastFunction(const std::string& tradeType) {
        if (castMap.find(tradeType) != castMap.end()) {
            return castMap[tradeType];
        } else {
            throw std::runtime_error("Trade type not found in castMap: " + tradeType);
        }
    }




    template<typename T>
    boost::shared_ptr<T> castTrade(const boost::shared_ptr<ore::data::Trade>& trade, const std::string& tradeType) {
        if (castMap.find(tradeType) != castMap.end()) {
            return boost::dynamic_pointer_cast<T>(castMap[tradeType](trade));
        } else {
            throw std::runtime_error("Trade type not found in castMap: " + tradeType);
        }
    }


private:
    std::map<std::string, std::function<boost::shared_ptr<ore::data::Trade>(const boost::shared_ptr<ore::data::Trade>&)>> castMap;
};


} // namespace analytics
} // namespace ore