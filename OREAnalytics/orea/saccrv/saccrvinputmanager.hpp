/*
 Copyright (C) 2024 Vannarho Pty Ltd
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

/*! \file /OREAnalytics/orea/saccrv/saccrvinputmanager.hpp
    \brief SACCRV header declares three Singleton classes, SaccrvConfigManager, SaccrvTradesManager and SaccrvTradeMtMManager, along with their associated struct definitions.
    \ingroup saccrv
*/

#pragma once

#include <vector>
#include <string>
#include <optional>
#include <orea/app/inputparameters.hpp> // include input parameters
#include <orea/saccrv/saccrvcsa.hpp>

namespace ore {
namespace analytics {

// -------------------------------------------------------------------------
// Definitions for SaccrvConfigurations
// -------------------------------------------------------------------------

struct SaccrvConfigurations {
    std::map<std::string, bool> analytics_;
    std::string outputFile;
    std::map<std::string, std::map<std::string, SaccrvSupervisoryFactor>> supervisoryFactors;
    boost::shared_ptr<NettingSetManager> nettingSetManager;
    boost::shared_ptr<Portfolio> saccrvPortfolio;
    std::vector<SaccrvCollateral> saccrvCollateral;

    // Additional configurations as needed

        // Getters
    std::map<std::string, bool> getAnalytics() const;
    std::string getOutputFile() const;
    std::map<std::string, std::map<std::string, SaccrvSupervisoryFactor>> getSupervisoryFactors() const;
    boost::shared_ptr<NettingSetManager> getNettingSetManager() const;
    boost::shared_ptr<Portfolio> getSaccrvPortfolio() const;
    std::vector<SaccrvCollateral> getSaccrvCollateral() const;
};

class SaccrvConfigManager {
private:
    static SaccrvConfigManager* instance_;
    SaccrvConfigurations config_;
    SaccrvConfigManager(); // Private constructor

public:
    static SaccrvConfigManager* getInstance();
    void setConfigurations(const SaccrvConfigurations& config);
    const SaccrvConfigurations& getConfigurations() const;
    bool isSimplified() const;
    bool isOEM() const;
    bool shouldIgnoreMargin() const;
    // Additional methods for manipulating configurations
    void setSimplified(bool simplified);
    void setOEM(bool OEM);
    void setIgnoreMargin(bool ignoreMargin);
};

// -------------------------------------------------------------------------
// Definitions for SaccrvCsaManager
// -------------------------------------------------------------------------



class SaccrvCsaManager {
private:
    static SaccrvCsaManager* instance_;
    std::vector<SaccrvCsa> csaVector;

    SaccrvCsaManager();

public:
    static SaccrvCsaManager* getInstance();
    void addSaccrvCsa(const SaccrvCsa& csaData);
    const std::vector<SaccrvCsa>& getCsaVector() const;
    
};


// -------------------------------------------------------------------------
// Definitions for SaccrvTrades
// -------------------------------------------------------------------------


struct SaccrvTrades {
    // std::string TradeObj;  // not needed
    std::string id;     // tradeId
    std::string tradeType; // vannarho instrument
    std::string tradeGroup;  // asset class as defined in supervisory factors - FX, IRD, CreditSingle, CreditIndex (grouped as Credit), Commodity, EQ, OtherExposure
    std::string subClass;   // asset subclass
    std::optional<double> notional;  
    std::optional<double> mTm;
    std::string currency;
    std::optional<double> Si;
    std::optional<double> Ei;
    std::optional<double> Ti; // for options - latest date of exercise
    std::string buySell;
    std::string counterparty;
    std::string nettingSet;
    std::string optionType;
    std::optional<double> underlyingPrice;
    std::optional<double> strikePrice;
    int quantity;
    std::string commodityType;
    std::string refEntity;
    std::string ccyPair;
    std::string payLegType;
    std::string payLegRef;
    std::optional<double> payLegTenor;
    std::string recLegType;
    std::string recLegRef;
    std::optional<double> recLegTenor;
    std::optional<double> volStrike;
    std::optional<double> tradedPrice;
    std::string deliveryType;
    std::string underlyingInstrument;
    std::string exoticType;
    std::string ISIN;
    std::string couponType;
    std::string issuer;
    std::optional<double> maturityDate;
    std::string paymentFrequency;
    std::string creditRiskWeight;
    std::string cdoAttachPoint;
    std::string cdoDetachPoint;
    std::string fxNearLegFields;
    std::string ccyPaying;
    std::string baseCcy;
    std::optional<double> amountPaying;
    std::string ccyReceiving;
    std::optional<double> amountReceiving;
    int timeBucket; 
    
    // Default constructor
    SaccrvTrades();

    // Constructor that initializes only tradeType and id
    SaccrvTrades(const std::string& tradeType, const std::string& id);

    SaccrvTrades(const double& mTm);

        // Getters
    std::string getId() const;
    std::string getTradeType() const;
    std::string getTradeGroup() const;
    std::string getSubClass() const;
    std::optional<double> getNotional() const;
    std::optional<double> getMtM() const;
    std::string getCurrency() const;
    std::optional<double> getSi() const;
    std::optional<double> getEi() const;
    std::optional<double> getTi() const;
    std::string getBuySell() const;
    std::string getCounterparty() const;
    std::string getNettingSet() const;
    std::string getOptionType() const;
    std::optional<double> getUnderlyingPrice() const;
    std::optional<double> getStrikePrice() const;
    int getQuantity() const;
    std::string getCommodityType() const;
    std::string getRefEntity() const;
    std::string getCcyPair() const;
    std::string getPayLegType() const;
    std::string getPayLegRef() const;
    std::optional<double> getPayLegTenor() const;
    std::string getRecLegType() const;
    std::string getRecLegRef() const;
    std::optional<double> getRecLegTenor() const;
    std::optional<double> getVolStrike() const;
    std::optional<double> getTradedPrice() const;
    std::string getDeliveryType() const;
    std::string getUnderlyingInstrument() const;
    std::string getExoticType() const;
    std::string getISIN() const;
    std::string getCouponType() const;
    std::string getIssuer() const;
    std::optional<double> getMaturityDate() const;
    std::string getPaymentFrequency() const;
    std::string getCreditRiskWeight() const;
    std::string getCdoAttachPoint() const;
    std::string getCdoDetachPoint() const;
    std::string getFxNearLegFields() const;
    std::string getCcyPaying() const;
    std::string getBaseCcy() const;
    std::optional<double> getAmountPaying() const;
    std::string getCcyReceiving() const;
    std::optional<double> getAmountReceiving() const;
    int getTimeBucket() const;

    // setters
    void setId(const std::string& id_) { id = id_; }
    void setTradeType(const std::string& tradeType_) { tradeType = tradeType_; }
    void setTradeGroup(const std::string& tradeGroup_) { tradeGroup = tradeGroup_; }
    void setSubClass(const std::string& subClass_) { subClass = subClass_; }
    void setNotional(const std::optional<double>& notional_) { notional = notional_; }
    void setMtM(const std::optional<double>& mTm_) { mTm = mTm_; }
    void setCurrency(const std::string& currency_) { currency = currency_; }
    void setSi(const std::optional<double>& Si_) { Si = Si_; }
    void setEi(const std::optional<double>& Ei_) { Ei = Ei_; }
    void setTi(const std::optional<double>& Ti_) { Ti = Ti_; }
    void setBuySell(const std::string& buySell_) { buySell = buySell_; }
    void setCounterparty(const std::string& counterparty_) { counterparty = counterparty_; }
    void setNettingSet(const std::string& nettingSet_) { nettingSet = nettingSet_; }
    void setOptionType(const std::string& optionType_) { optionType = optionType_; }
    void setUnderlyingPrice(const std::optional<double>& underlyingPrice_) { underlyingPrice = underlyingPrice_; }
    void setStrikePrice(const std::optional<double>& strikePrice_) { strikePrice = strikePrice_; }
    void setQuantity(const int& quantity_) { quantity = quantity_; }
    void setCommodityType(const std::string& commodityType_) { commodityType = commodityType_; }
    void setRefEntity(const std::string& refEntity_) { refEntity = refEntity_; }
    void setCcyPair(const std::string& ccyPair_) { ccyPair = ccyPair_; }
    void setPayLegType(const std::string& payLegType_) { payLegType = payLegType_; }
    void setPayLegRef(const std::string& payLegRef_) { payLegRef = payLegRef_; }
    void setPayLegTenor(const std::optional<double>& payLegTenor_) { payLegTenor = payLegTenor_; }
    void setRecLegType(const std::string& recLegType_) { recLegType = recLegType_; }
    void setRecLegRef(const std::string& recLegRef_) { recLegRef = recLegRef_; }
    void setRecLegTenor(const std::optional<double>& recLegTenor_) { recLegTenor = recLegTenor_; }
    void setVolStrike(const std::optional<double>& volStrike_) { volStrike = volStrike_; }
    void setTradedPrice(const std::optional<double>& tradedPrice_) { tradedPrice = tradedPrice_; }
    void setDeliveryType(const std::string& deliveryType_) { deliveryType = deliveryType_; }
    void setUnderlyingInstrument(const std::string& underlyingInstrument_) { underlyingInstrument = underlyingInstrument_; }
    void setExoticType(const std::string& exoticType_) { exoticType = exoticType_; }
    void setISIN(const std::string& ISIN_) { ISIN = ISIN_; }
    void setCouponType(const std::string& couponType_) { couponType = couponType_; }
    void setIssuer(const std::string& issuer_) { issuer = issuer_; }
    void setMaturityDate(const std::optional<double>& maturityDate_) { maturityDate = maturityDate_; }
    void setPaymentFrequency(const std::string& paymentFrequency_) { paymentFrequency = paymentFrequency_; }
    void setCreditRiskWeight(const std::string& creditRiskWeight_) { creditRiskWeight = creditRiskWeight_; }
    void setCdoAttachPoint(const std::string& cdoAttachPoint_) { cdoAttachPoint = cdoAttachPoint_; }
    void setCdoDetachPoint(const std::string& cdoDetachPoint_) { cdoDetachPoint = cdoDetachPoint_; }
    void setFxNearLegFields(const std::string& fxNearLegFields_) { fxNearLegFields = fxNearLegFields_; }
    void setCcyPaying(const std::string& ccyPaying_) { ccyPaying = ccyPaying_; }
    void setBaseCcy(const std::string& baseCcy_) { baseCcy = baseCcy_; }
    void setAmountPaying(const std::optional<double>& amountPaying_) { amountPaying = amountPaying_; }
    void setCcyReceiving(const std::string& ccyReceiving_) { ccyReceiving = ccyReceiving_; }
    void setAmountReceiving(const std::optional<double>& amountReceiving_) { amountReceiving = amountReceiving_; }
    void setTimeBucket(int timeBucket_) { timeBucket = timeBucket_; }
};



class SaccrvTradesManager {
private:
    static SaccrvTradesManager* instance_;
    std::vector<analytics::SaccrvTrades> tradeVector;
    SaccrvTradesManager(); // Private constructor

public:
    static SaccrvTradesManager* getInstance();

    //setters
    void addSaccrvTrade(const analytics::SaccrvTrades& tradeData);
    void addSaccrvTradeHeader(const std::string& id, const std::string& tradeType);
    void addSaccrvMtM(const std::string& tradeId, const double& mTm);
    void updateSaccrvTrade(const std::string& tradeId, const analytics::SaccrvTrades& updatedTrade);
    void updateSaccrvTradeType(const std::string& tradeId, const std::string& newTradeType);
    size_t getTradeVectorSize() const;

    // getters
    const std::vector<analytics::SaccrvTrades>& getTradeVector() const;
    
};


// -------------------------------------------------------------------------
// Definitions for SaccrvTradeMtM
// -------------------------------------------------------------------------


// struct SaccrvTradeMtM {
//     std::string id;
//     double mtm;
//     SaccrvTradeMtM(const std::string& tradeId, double tradeMtM);
//     const std::optional<double> getMtM() const;
// };


// class SaccrvTradeMtMManager {
// private:
//     static SaccrvTradeMtMManager* instance_;
//     std::vector<SaccrvTradeMtM> tradeMtMVector;
//     SaccrvTradeMtMManager(); // Private constructor

// public:
//     static SaccrvTradeMtMManager* getInstance();
//     void addSaccrvTradeMtM(const std::string& tradeId, double tradeMtM);
//     const std::vector<SaccrvTradeMtM>& getTradeMtMVector() const;
//     double getMtM(const std::string& tradeId) const;
// };

// -------------------------------------------------------------------------
// Definitions for SaccrvCollateralManager
// -------------------------------------------------------------------------


class SaccrvCollateralManager {
private:
    static SaccrvCollateralManager* instance_;
    std::vector<SaccrvCollateral> collateralVector_; // Declare the vector here

public:
    SaccrvCollateralManager(); // Add noexcept if it's required
    static SaccrvCollateralManager* getInstance();
    void addSaccrvCollateral(const SaccrvCollateral& collateral);
    const std::vector<SaccrvCollateral>& getCollateralVector() const; 
};


} // namespace analytics
} // namespace ore
