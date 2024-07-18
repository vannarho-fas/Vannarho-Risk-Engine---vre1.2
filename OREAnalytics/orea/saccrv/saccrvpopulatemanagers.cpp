/*! \file /OREAnalytics/orea/saccrv/saccrvpopulatemanagers.cpp
    \brief SACCRV calculator - populates SaccrvTrades from SaccrvTradeDataFactory, extract CSA data using the Netting Set Manager and store using SaccrvCsaManager, add the  collateral to the manager
    \ingroup saccrv
*/

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <boost/shared_ptr.hpp>

#include <orea/saccrv/saccrvinputmanager.hpp>
#include <orea/saccrv/saccrvcalculationmanager.hpp>
#include <orea/saccrv/saccrvpopulatemanagers.hpp>
#include <orea/saccrv/saccrvtradedatafactory.hpp>
#include <orea/saccrv/saccrvcsa.hpp>
#include <orea/saccrv/tradeparsers/utils.hpp>
#include <orea/saccrv/saccrvinputmanager.hpp>


using namespace ore::data;
using namespace boost::filesystem;

namespace ore {
namespace analytics {

void SaccrvPopulateManagers::populate() {
    // Setup configurations
    auto& configs = SaccrvConfigManager::getInstance()->getConfigurations();

    // Populate SaccrvTrades from SaccrvTradeDataFactory
    LOG("SACCRV: Populate SaccrvTrades from SaccrvTradeDataFactory");
    SaccrvTradeDataFactory factory;
    auto saccrvTradesManager = SaccrvTradesManager::getInstance();
    // auto saccrvTradeMtMManager = SaccrvTradeMtMManager::getInstance();

    for (const auto& id : configs.saccrvPortfolio->ids()) {
        auto trade = configs.saccrvPortfolio->get(id);
        SaccrvTrades tradeData = factory.buildTradeData(trade);

        if (!tradeData.id.empty()) {
            // add the mTm from built portfolio
            tradeData.mTm = trade->instrument()->NPV();
            // add the trade data to the trades manager
            saccrvTradesManager->updateSaccrvTrade(tradeData.id, tradeData);
            LOG("SACCRV: Updated trade data " << tradeData);
        }
        else {
            LOG("SACCRV: Trade data not found for trade id: " << trade->id());
        }
    }

    auto trade_size = saccrvTradesManager->getTradeVectorSize();
    LOG("SACCRV: Trade vector size: " << trade_size);

    // If the NettingSetManager has NettingSetDetails, extract CSA data using the Netting Set Manager and store using SaccrvCsaManager
    LOG("SACCRV: Extracting CSA data using the Netting Set Manager and storing using SaccrvCsaManager");
    std::vector<SaccrvCsa> csaData = extractCsaData(configs.nettingSetManager, configs.saccrvPortfolio);
    // After extracting CSA data
    auto saccrvCsaManager = SaccrvCsaManager::getInstance();
    for (const auto& csa : csaData) {
        saccrvCsaManager->addSaccrvCsa(csa);
    }
    LOG("SACCRV: CSA data extracted and stored in manager");


    // Get the instance of SaccrvCollateralManager, collect the vector and populate the manager
    auto saccrvCollateralManager = SaccrvCollateralManager::getInstance();
    auto collateralVector = configs.getSaccrvCollateral();
    for (const auto& collateral : collateralVector) {
         saccrvCollateralManager->addSaccrvCollateral(collateral);
    }
    LOG("SACCRV: Collateral data stored in manager");

    LOG("SACCRV: populate() complete");
}

} // namespace analytics
} // namespace ore