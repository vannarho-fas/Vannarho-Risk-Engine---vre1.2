
/*! \file /OREAnalytics/orea/app/analytics/saccrvanalytic.cpp
    \brief SACCRV analytic
    \ingroup analytics
*/
#include <orea/engine/observationmode.hpp>
#include <ored/portfolio/trade.hpp>
#include <orea/app/oreapp.hpp>
#include <orea/app/structuredanalyticswarning.hpp>
#include <orea/app/marketdatainmemoryloader.hpp>
#include <ored/utilities/xmlutils.hpp>
#include <orea/app/reportwriter.hpp>

#include <orea/app/analytics/saccrvanalytic.hpp>
#include <orea/saccrv/saccrvpopulatemanagers.hpp>
#include <orea/saccrv/calculation/runsaccrvcalcs.hpp>
#include <orea/saccrv/saccrvtradedatafactory.hpp>
#include <orea/saccrv/saccrvinputmanager.hpp>

#include <ostream>
#include <vector>
#include <iostream>
#include <sstream>

using namespace ore::data;
using namespace boost::filesystem;

namespace ore {
namespace analytics {

void SaccrvAnalyticImpl::runAnalytic(const boost::shared_ptr<ore::data::InMemoryLoader>& loader,
                                     const std::set<std::string>& runTypes) {
 
    boost::shared_ptr<InMemoryReport> report = boost::make_shared<InMemoryReport>();
    LOG("SACCRV: Build Market & Portfolio");
    QL_REQUIRE(inputs_->portfolio(), "SaccrvAnalytic::runAnalytic: No portfolio loaded.");
    nettingSetManager_ = inputs_->nettingSetManager();
    QL_REQUIRE(nettingSetManager_, "SaccrvAnalytic::runAnalytic: No nettingSetManager loaded.");

    Settings::instance().evaluationDate() = inputs_->asof();
    ObservationMode::instance().setMode(inputs_->exposureObservationModel());

    CONSOLEW("SACCRV: Build Market");
    analytic()->buildMarket(loader);
    CONSOLE("OK");

    CONSOLEW("SACCRV: Build Portfolio");
    analytic()->buildPortfolio();
    saccrvPortfolio_ = analytic()->portfolio();
    CONSOLE("OK");

    LOG("SACCRV: Load Configurations");
    setUpConfigurations();

    CONSOLEW("SACCRV: Populate Managers");
    SaccrvPopulateManagers saccrvPopulateManagers;
    saccrvPopulateManagers.populate();
    CONSOLE("OK");

    CONSOLEW("SACCRV: Run Calculations");
    RunSaccrvCalculations runSaccrvCalculations;
    runSaccrvCalculations.runCalcs();
    CONSOLE("OK");

    CONSOLEW("SACCRV: Write Report");
    // Get the singleton instance
    ore::analytics::SaccrvCalculationManager* manager = ore::analytics::SaccrvCalculationManager::getInstance();
    // Get the data from the singleton
    const ore::analytics::CounterpartyCreditRiskExposureTree& data = manager->getCounterpartyTree();
    ReportWriter(inputs_->reportNaString())
        .writeSaccrvReport(*report, data);
    analytic()->reports()["SACCRV"]["saccrv"] = report;
    CONSOLE("OK");

}

void SaccrvAnalyticImpl::setUpConfigurations() {
    LOG("SaccrvAnalyticImpl::setUpConfigurations() called");
    
    SaccrvConfigurations config;
    config.analytics_ = {
        {"simplified", inputs_->simplifiedSACCRV()},
        {"OEM", inputs_->OEMSACCRV()},
        {"ignoreMargin", inputs_->ignoreMarginSACCRV()}
    };
    config.outputFile = inputs_->saccrvOutputFile();
    config.supervisoryFactors = inputs_->saccrvSupervisoryFactors();
    config.nettingSetManager = inputs_->nettingSetManager();
    config.saccrvPortfolio = saccrvPortfolio_;
    config.saccrvCollateral = inputs_->getSaccrvCollateral();

    SaccrvConfigManager::getInstance()->setConfigurations(config);

    analytic()->configurations().todaysMarketParams = inputs_->todaysMarketParams();
}

} // namespace analytics
} // namespace ore
