/*! \file /OREAnalytics/orea/app/analytics/saccrvanalytic.hpp
    \brief SACCRV analytic
    \ingroup analytics
*/

#pragma once

#include <orea/app/analytic.hpp>
#include <orea/app/inputparameters.hpp> // include input parameters
#include <ored/portfolio/portfolio.hpp>
#include <ored/portfolio/trade.hpp>
#include <ored/portfolio/nettingsetmanager.hpp>
#include <orea/saccrv/saccrvinputmanager.hpp> 
#include <map>
#include <string>

namespace ore {
namespace analytics {

// This file is /OREAnalytics/orea/app/analytics/saccrvanalytic.hpp


// This class manages the overll workflow of the SA-CCR calculation
// It is responsible for:
// - collecting the input parameters
// - creating the necessary trade-level inputs from the portfolio
// - running the SA-CCR calculation
// - writing the results to the report and to the file
// It is designed to have a separation of concerns between the SACCR analytics modules and the base ORE codebase

// Struct definition
// struct SaccrvConfigurations {
//     std::map<std::string, bool> analytics;
//     std::string outputFile;
//     std::map<std::string, std::map<std::string, SaccrvSupervisoryFactor>> supervisoryFactors;
//     std::vector<SaccrvTrades> trades;
//     boost::shared_ptr<NettingSetManager> nettingSetManager;
//     boost::shared_ptr<Portfolio> saccrvPortfolio;
//     // Add other relevant configurations here
// };


class SaccrvAnalyticImpl : public Analytic::Impl {
public:
    static constexpr const char* LABEL = "SACCRV";

    SaccrvAnalyticImpl(const boost::shared_ptr<InputParameters>& inputs) 
        : Analytic::Impl(inputs) { setLabel(LABEL); }
    void setUpConfigurations() override;
    void runAnalytic(const boost::shared_ptr<ore::data::InMemoryLoader>& loader,
                     const std::set<std::string>& runTypes = {}) override;
    // void setNettingSet();
    std::vector<SaccrvTrades> loadSACCRVTradeHeaders();

    // SaccrvConfigurations config;

    boost::shared_ptr<Portfolio> saccrvPortfolio_;
    boost::shared_ptr<NettingSetManager> nettingSetManager_;


protected:
    boost::shared_ptr<InputParameters> saccrvInputs_;
    

private: 
    map<string, bool> saccrvAnalytics_;
    std::vector<SaccrvTrades> saccrvTrades_;

};

class SaccrvAnalytic : public Analytic {
public:
    SaccrvAnalytic(const boost::shared_ptr<InputParameters>& inputs)
        : Analytic(std::make_unique<SaccrvAnalyticImpl>(inputs), {"SACCRV", "SACCRV"}, inputs, false, false, false, false) {}
};

} // namespace analytics
} // namespace ore
