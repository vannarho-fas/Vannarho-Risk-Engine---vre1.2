/*! \file /OREAnalytics/orea/saccrv/saccrvinputmanager.cpp
    \brief SACCRV defines the methods for SaccrvConfigManager, SaccrvTradesManager, SaccrvCollateralManager and SaccrvTradeMtMManager classes, including the Singleton pattern implementation (getInstance), methods to add elements
    \ingroup saccrv
*/

#include <orea/saccrv/saccrvinputmanager.hpp>

namespace ore {
namespace analytics {

// -------------------------------------------------------------------------
// Definitions for SaccrvConfigManager
// -------------------------------------------------------------------------


SaccrvConfigManager* SaccrvConfigManager::instance_ = nullptr;

SaccrvConfigManager* SaccrvConfigManager::getInstance() {
    if (instance_ == nullptr) {
        instance_ = new SaccrvConfigManager();
    }
    return instance_;
}

void SaccrvConfigManager::setConfigurations(const SaccrvConfigurations& config) {
    config_ = config;
}

const SaccrvConfigurations& SaccrvConfigManager::getConfigurations() const {
    return config_;
}

SaccrvConfigManager::SaccrvConfigManager() {
    // Initialize default configurations or load from a file if necessary
}

// Implementations for SaccrvConfigurations getters
std::map<std::string, bool> SaccrvConfigurations::getAnalytics() const {
    return analytics_;
}

std::string SaccrvConfigurations::getOutputFile() const {
    return outputFile;
}

std::map<std::string, std::map<std::string, SaccrvSupervisoryFactor>> SaccrvConfigurations::getSupervisoryFactors() const {
    return supervisoryFactors;
}

std::vector<SaccrvCollateral> SaccrvConfigurations::getSaccrvCollateral() const {
    return saccrvCollateral;
}

boost::shared_ptr<NettingSetManager> SaccrvConfigurations::getNettingSetManager() const {
    return nettingSetManager;
}

boost::shared_ptr<Portfolio> SaccrvConfigurations::getSaccrvPortfolio() const {
    return saccrvPortfolio;
}

bool SaccrvConfigManager::isSimplified() const {
    auto config_ = getConfigurations();
    return config_.analytics_.at("simplified");
}

bool SaccrvConfigManager::isOEM() const {
    auto config_ = getConfigurations();
    return config_.analytics_.at("OEM");
}

bool SaccrvConfigManager::shouldIgnoreMargin() const {
    auto config_ = getConfigurations();
    return config_.analytics_.at("ignoreMargin");
}

// Setters
void SaccrvConfigManager::setSimplified(bool simplified) { config_.analytics_["simplified"] = simplified; }
void SaccrvConfigManager::setOEM(bool OEM) { config_.analytics_["OEM"] = OEM; }
void SaccrvConfigManager::setIgnoreMargin(bool ignoreMargin) { config_.analytics_["ignoreMargin"] = ignoreMargin; }


// -------------------------------------------------------------------------
// Definitions for SaccrvCsaManager
// -------------------------------------------------------------------------


SaccrvCsaManager* SaccrvCsaManager::instance_ = nullptr;

SaccrvCsaManager::SaccrvCsaManager() {}

SaccrvCsaManager* SaccrvCsaManager::getInstance() {
    if (instance_ == nullptr) {
        instance_ = new SaccrvCsaManager();
    }
    return instance_;
}

void SaccrvCsaManager::addSaccrvCsa(const SaccrvCsa& csaData) {
    csaVector.push_back(csaData);
}

const std::vector<SaccrvCsa>& SaccrvCsaManager::getCsaVector() const {
    return csaVector;
}


// -------------------------------------------------------------------------
// Definitions for SaccrvTrades
// -------------------------------------------------------------------------


// Default constructor
SaccrvTrades::SaccrvTrades()
    : notional(std::nullopt), mTm(std::nullopt),
      Si(std::nullopt), Ei(std::nullopt),
      underlyingPrice(std::nullopt), strikePrice(std::nullopt),
      payLegTenor(std::nullopt), recLegTenor(std::nullopt),
      volStrike(std::nullopt), tradedPrice(std::nullopt),
      maturityDate(std::nullopt), amountPaying(std::nullopt),
      amountReceiving(std::nullopt) {
    // Constructor implementation
}

// Constructor that initializes only tradeType and id
SaccrvTrades::SaccrvTrades(const std::string& id, const std::string& tradeType)
    : id(id), tradeType(tradeType)
      // ... [initialize other members with default values] ...
{
    // Constructor implementation
}

SaccrvTrades::SaccrvTrades(const double& mTm) {
    this->mTm = mTm;
    // Initialize other members as needed...
}



// Definitions for SaccrvTradesManager
SaccrvTradesManager* SaccrvTradesManager::instance_ = nullptr;

SaccrvTradesManager* SaccrvTradesManager::getInstance() {
    if (instance_ == nullptr) {
        instance_ = new SaccrvTradesManager();
    }
    return instance_;
}

void SaccrvTradesManager::addSaccrvTrade(const SaccrvTrades& tradeData) {
    tradeVector.push_back(tradeData);
}

void SaccrvTradesManager::addSaccrvTradeHeader(const std::string& id, const std::string& tradeType) {
    SaccrvTrades partialTrade(id, tradeType);
    tradeVector.push_back(partialTrade);
}
    
void SaccrvTradesManager::addSaccrvMtM(const std::string& tradeId, const double& mTm) {
    auto it = std::find_if(tradeVector.begin(), tradeVector.end(), 
                           [&tradeId](const SaccrvTrades& trade) { return trade.id == tradeId; });
    if (it != tradeVector.end()) {
        it->mTm = mTm; // Assuming `mTm` is a member of `SaccrvTrades`
    }
}


void SaccrvTradesManager::updateSaccrvTrade(const std::string& tradeId, const SaccrvTrades& updatedTrade) {
    auto it = std::find_if(tradeVector.begin(), tradeVector.end(), 
                           [&tradeId](const SaccrvTrades& trade) { return trade.id == tradeId; });
    if (it != tradeVector.end()) {
        *it = updatedTrade;
    }
    else {
        LOG("SACCRV: Inputmanager - Trade not found: " << tradeId << ". Adding new trade.");
        tradeVector.push_back(updatedTrade);
    }
}

void SaccrvTradesManager::updateSaccrvTradeType(const std::string& tradeId, const std::string& newTradeType) {
    auto it = std::find_if(tradeVector.begin(), tradeVector.end(), 
                           [&tradeId](const SaccrvTrades& trade) { return trade.getId() == tradeId; });
    if (it != tradeVector.end()) {
        it->tradeType = newTradeType;
    }
}


const std::vector<SaccrvTrades>& SaccrvTradesManager::getTradeVector() const {
    return tradeVector;
}

size_t SaccrvTradesManager::getTradeVectorSize() const {
    return tradeVector.size();
}


// Implementations for SaccrvTrades getters
std::string SaccrvTrades::getTradeGroup() const {
    return tradeGroup;
}

std::string SaccrvTrades::getTradeType() const {
    return tradeType;
}

std::string SaccrvTrades::getSubClass() const {
    return subClass;
}

std::optional<double> SaccrvTrades::getNotional() const {
    return notional;
}

std::optional<double> SaccrvTrades::getMtM() const {
    return mTm;
}

std::string SaccrvTrades::getCurrency() const {
    return currency;
}

std::optional<double> SaccrvTrades::getSi() const {
    return Si;
}

std::optional<double> SaccrvTrades::getEi() const {
    return Ei;
}

std::optional<double> SaccrvTrades::getTi() const {
    return Ti;
}

std::string SaccrvTrades::getBuySell() const {
    return buySell;
}

std::string SaccrvTrades::getId() const {
    return id;
}

std::string SaccrvTrades::getCounterparty() const {
    return counterparty;
}

std::string SaccrvTrades::getNettingSet() const {
    return nettingSet;
}

std::string SaccrvTrades::getOptionType() const {
    return optionType;
}

std::optional<double> SaccrvTrades::getUnderlyingPrice() const {
    return underlyingPrice;
}

std::optional<double> SaccrvTrades::getStrikePrice() const {
    return strikePrice;
}

int SaccrvTrades::getQuantity() const {
    return quantity;
}

std::string SaccrvTrades::getCommodityType() const {
    return commodityType;
}

std::string SaccrvTrades::getRefEntity() const {
    return refEntity;
}

std::string SaccrvTrades::getCcyPair() const {
    return ccyPair;
}

std::string SaccrvTrades::getPayLegType() const {
    return payLegType;
}

std::string SaccrvTrades::getPayLegRef() const {
    return payLegRef;
}

std::optional<double> SaccrvTrades::getPayLegTenor() const {
    return payLegTenor;
}

std::string SaccrvTrades::getRecLegType() const {
    return recLegType;
}

std::string SaccrvTrades::getRecLegRef() const {
    return recLegRef;
}

std::optional<double> SaccrvTrades::getRecLegTenor() const {
    return recLegTenor;
}

std::optional<double> SaccrvTrades::getVolStrike() const {
    return volStrike;
}

std::optional<double> SaccrvTrades::getTradedPrice() const {
    return tradedPrice;
}

std::string SaccrvTrades::getDeliveryType() const {
    return deliveryType;
}

std::string SaccrvTrades::getUnderlyingInstrument() const {
    return underlyingInstrument;
}

std::string SaccrvTrades::getExoticType() const {
    return exoticType;
}

std::string SaccrvTrades::getISIN() const {
    return ISIN;
}

std::string SaccrvTrades::getCouponType() const {
    return couponType;
}

std::string SaccrvTrades::getIssuer() const {
    return issuer;
}

std::optional<double> SaccrvTrades::getMaturityDate() const {
    return maturityDate;
}

std::string SaccrvTrades::getPaymentFrequency() const {
    return paymentFrequency;
}

std::string SaccrvTrades::getCreditRiskWeight() const {
    return creditRiskWeight;
}

std::string SaccrvTrades::getCdoAttachPoint() const {
    return cdoAttachPoint;
}

std::string SaccrvTrades::getCdoDetachPoint() const {
    return cdoDetachPoint;
}

std::string SaccrvTrades::getFxNearLegFields() const {
    return fxNearLegFields;
}

std::string SaccrvTrades::getCcyPaying() const {
    return ccyPaying;
}

std::string SaccrvTrades::getBaseCcy() const {
    return baseCcy;
}

std::optional<double> SaccrvTrades::getAmountPaying() const {
    return amountPaying;
}

std::string SaccrvTrades::getCcyReceiving() const {
    return ccyReceiving;
}

std::optional<double> SaccrvTrades::getAmountReceiving() const {
    return amountReceiving;
}

int SaccrvTrades::getTimeBucket() const {
    return timeBucket;
}


SaccrvTradesManager::SaccrvTradesManager() {
    // Private constructor implementation
}


// -------------------------------------------------------------------------
// Definitions for SaccrvTradeMtM
// -------------------------------------------------------------------------



// // Definitions for SaccrvTradeMtM
// SaccrvTradeMtM::SaccrvTradeMtM(const std::string& tradeId, double mTm)
//     : id(tradeId), mtm(mTm) {}


// // Definitions for SaccrvTradeMtMManager
// SaccrvTradeMtMManager* SaccrvTradeMtMManager::instance_ = nullptr;

// SaccrvTradeMtMManager* SaccrvTradeMtMManager::getInstance() {
//     if (instance_ == nullptr) {
//         instance_ = new SaccrvTradeMtMManager();
//     }
//     return instance_;
// }

// void SaccrvTradeMtMManager::addSaccrvTradeMtM(const std::string& id, double mtm) {
//     tradeMtMVector.push_back(SaccrvTradeMtM(id, mtm));
// }

// const std::vector<SaccrvTradeMtM>& SaccrvTradeMtMManager::getTradeMtMVector() const {
//     return tradeMtMVector;
// }

// const std::optional<double> SaccrvTradeMtM::getMtM() const {
//     return mtm;
// }


// double SaccrvTradeMtMManager::getMtM(const std::string& tradeId) const {
//     for (const auto& trade : tradeMtMVector) {
//         if (trade.id == tradeId) {
//             return trade.mtm;
//         }
//     }
//     throw std::runtime_error("Trade ID not found"); // Or handle this case appropriately
// }

// SaccrvTradeMtMManager::SaccrvTradeMtMManager() {
//     // Private constructor implementation
// }


// -------------------------------------------------------------------------
// Definitions for SaccrvCollateralManager
// -------------------------------------------------------------------------

SaccrvCollateralManager* SaccrvCollateralManager::instance_ = nullptr;

SaccrvCollateralManager::SaccrvCollateralManager() {
    // Constructor implementation
}

SaccrvCollateralManager* SaccrvCollateralManager::getInstance() {
    if (instance_ == nullptr) {
        instance_ = new SaccrvCollateralManager();
    }
    return instance_;
}

void SaccrvCollateralManager::addSaccrvCollateral(const SaccrvCollateral& collateral) {
    collateralVector_.push_back(collateral); 
}

const std::vector<SaccrvCollateral>& SaccrvCollateralManager::getCollateralVector() const {
    return collateralVector_;
}


} // namespace analytics
} // namespace ore
