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

/*! \file /OREAnalytics/orea/saccrv/saccrvcalculationmanager.hpp
    \brief SACCRV header declares the calculation manager class. This manager provides methods for populating and retrieving data efficiently. It defines a singleton class and the following structs: ExposureDetails, TradeData, TimeBucket, Currency, CommodityType, HedgingSet, ReferenceEntity, CurrencyPair, OtherSubClass, IRD, Commodity, Credit, FX, Equity, OtherExposure, AssetClasses, ReplacementCost, Counterparty, CounterpartyCreditRiskExposureTree, and SaccrvData.
    \ingroup saccrv
*/

#pragma once
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <orea/saccrv/saccrvinputmanager.hpp>

namespace ore {
namespace analytics {


struct ExposureDetails {
    double adjNotional;
    double maturityFactor;
    double supervDelta;
    double effectiveNotional;

    ExposureDetails() : adjNotional(0.0), maturityFactor(0.0), supervDelta(0.0), effectiveNotional(0.0) {}
};

struct TradeData {
    std::string id;
    ExposureDetails exposureDetails;
    SaccrvTrades trades;

    TradeData() : id(""), exposureDetails(), trades() {}
};


struct TimeBucket {
    int timeBucketNum;
    double effectiveNotional;
    std::vector<TradeData> tradeData;

    TimeBucket() : timeBucketNum(0), effectiveNotional(0.0), tradeData() {}

};

struct CurrencyData {
    std::string currency;
    double addon;
    double effectiveNotional;
    double supervisoryFactor;
    std::vector<TimeBucket> timeBuckets;

    CurrencyData() : addon(0.0), effectiveNotional(0.0), supervisoryFactor(0.0), timeBuckets() {}

};

struct CommodityType {
    std::string commodity;
    double addon;
    std::string subClass; //TODO
    double effectiveNotional;
    double supervisoryFactor;
    std::vector<TradeData> tradeData;

    CommodityType() : commodity(""), addon(0.0), subClass(""), effectiveNotional(0.0), supervisoryFactor(0.0), tradeData() {}

};

struct HedgingSet {
    std::string hedgingSet;
    double addon;
    double supervisoryCorel;
    std::vector<CommodityType> commodityTypes;

    HedgingSet() : hedgingSet(""), addon(0.0), supervisoryCorel(0.0), commodityTypes() {}

};

struct ReferenceEntity {
    std::string referenceEntity;
    double addon;
    std::string subClass; // new - also needed for commodity to know subclass as ref level
    double supervisoryCorel;
    double supervisoryFactor;
    std::vector<TradeData> tradeData;

    ReferenceEntity() : referenceEntity(""), addon(0.0), subClass(""), supervisoryCorel(0.0), supervisoryFactor(0.0), tradeData() {}

};

struct CurrencyPair {
    std::string currencyPair;
    double addon;
    double supervisoryFactor;
    std::vector<TradeData> tradeData;

    CurrencyPair() : currencyPair(""), addon(0.0), supervisoryFactor(0.0), tradeData() {}

};

struct OtherSubClass {
    std::string otherSubClass;
    double addon;
    double supervisoryFactor;
    std::vector<TradeData> tradeData;

    OtherSubClass() : otherSubClass(""), addon(0.0), supervisoryFactor(0.0), tradeData() {}

};

struct IRDData {
    double addon;
    std::vector<CurrencyData> currencies;

    IRDData() : addon(0.0), currencies() {}

};

struct CommodityData {
    double addon;
    std::vector<HedgingSet> hedgingSets;

    CommodityData() : addon(0.0), hedgingSets() {}


};

struct CreditData {
    double addon;
    double idiosynchraticComponent;
    double systematicComponent;
    std::vector<ReferenceEntity> referenceEntities;

    CreditData() : addon(0.0), idiosynchraticComponent(0.0), systematicComponent(0.0), referenceEntities() {}


};

struct FXData {
    double addon;
    std::vector<CurrencyPair> currencyPairs;

    FXData() : addon(0.0), currencyPairs() {}


};

struct EquityData {
    double addon;
    double idiosynchraticComponent;
    double systematicComponent;
    std::vector<ReferenceEntity> referenceEntities;

    EquityData() : addon(0.0), idiosynchraticComponent(0.0), systematicComponent(0.0), referenceEntities() {}

};

struct OtherExposureData {
    double addon;
    std::vector<OtherSubClass> otherSubClasses;

    OtherExposureData() : addon(0.0), otherSubClasses() {}

};

struct AssetClasses {
    IRDData ird;
    CommodityData commodity;
    CreditData credit;
    FXData fx;
    EquityData equity;
    OtherExposureData otherExposure;

    AssetClasses() : ird(), commodity(), credit(), fx(), equity(), otherExposure() {}

};

struct ReplacementCost {
    double v_c;
    double rc;
    double v;

    ReplacementCost() : v_c(0.0), rc(0.0), v(0.0) {}
};

struct NettingSet {
    std::string id;
    std::string counterpartyId;
    bool isMargined;
    bool hasIlliquidCollateralOrOTCNotEasilyReplaced;
    bool applyMarginCallDisputes;
    double addOn;
    double ead;
    double pfe;
    ReplacementCost replacementCost;
    AssetClasses assetClasses;

    NettingSet() : id(""), isMargined(false), hasIlliquidCollateralOrOTCNotEasilyReplaced(false), applyMarginCallDisputes(false), addOn(0.0), ead(0.0), pfe(0.0), assetClasses() {}

};


struct Counterparty {
    std::string counterpartyId;
    double addOn;
    double ead;
    double pfe;
    ReplacementCost replacementCost;
    std::vector<NettingSet> nettingSets;
    size_t getNettingSetsSize() const {
        return nettingSets.size();
    }

    Counterparty() : counterpartyId(""), addOn(0.0), ead(0.0), pfe(0.0), replacementCost(), nettingSets() {}

};

struct CounterpartyCreditRiskExposureTree {
    std::vector<Counterparty> counterparties;

    CounterpartyCreditRiskExposureTree() : counterparties() {}

};

struct SaccrvCalculationContext {
    bool isMargined = false;
    std::string counterpartyId = "";
    std::string nettingsetId = "";
    std::vector<SaccrvTrades> trades = {};
    std::vector<SaccrvCollateral> collaterals = {};
    SaccrvCsa csa = SaccrvCsa();
    bool simplified = false;
    bool OEM = false;
    int mporDays = 0;
    bool hasIlliquidCollateralOrOTCNotEasilyReplaced = false;
    bool applyMarginCallDisputes = false;
    bool greaterThan5000TransactionsNotCentrallyCleared = false;
    int remarginFreq = 0;
    bool ignoreMargin = false;
};


class SaccrvCalculationManager {
private:
    static SaccrvCalculationManager* instance_;
    static std::mutex mutex_;
    
    SaccrvCalculationManager() : data_(std::make_shared<CounterpartyCreditRiskExposureTree>()) {} // Private constructor

    std::shared_ptr<Counterparty> currentCounterparty_;
    std::shared_ptr<CounterpartyCreditRiskExposureTree> data_;
    void printIRD(const IRDData& ird);
    void printCommodity(const CommodityData& commodity);
    void printCredit(const CreditData& credit);
    void printFX(const FXData& fx);
    void printEquity(const EquityData& equity) ;
    void printOtherExposure(const OtherExposureData& otherExposure) ;

    void printIRDTree(const IRDData& ird);
    // void printCommodityTree(const CommodityData& commodity);
    // void printCreditTree(const CreditData& credit);
    // void printFXTree(const FXData& fx);
    // void printEquityTree(const EquityData& equity);
    // void printOtherExposureTree(const OtherExposureData& otherExposure);
    




public:

    static SaccrvCalculationManager* getInstance();
    std::shared_ptr<Counterparty> getCurrentCounterparty() { return currentCounterparty_; } 
    std::vector<Counterparty>& getCounterparties() {
        return data_->counterparties;
    }
    
    CounterpartyCreditRiskExposureTree& getCounterpartyTree() {
        return *data_;
    }
    
    void initialiseCounterpartyTree(const std::vector<Counterparty>& counterparties);
    // void createSaccrvCounterpartyTree(const std::vector<SaccrvTrades>& trades);
    void createSaccrvNettingSetTree(const std::string& counterpartyId, const std::string& nettingSetId, const std::vector<SaccrvTrades>& trades,const bool isMargined);
    void printSaccrvData() ;
    void updateReplacementCost(const std::string& counterpartyId, const std::string& nettingSetId, double V_C, double RC, double V);
    void updatePFE(const std::string& counterpartyId, const std::string& nettingSetId, double PFE);
    void updateEAD(const std::string& counterpartyId, const std::string& nettingSetId, double EAD);
    void sumNettingSetValuesToCounterparty();
    int calculateTimeBucketForTrade(const TradeData& tradeData);
    Counterparty* getCounterparty(const std::string& counterpartyId);
    void addCounterparty(Counterparty& counterparty);
    NettingSet* getNettingSet(const std::string& counterpartyId, const std::string& nettingSetId);
    void deleteNettingSet(const std::string& counterpartyId, const std::string& nettingsetId);
    double getEAD(const std::string& counterpartyId);
    void printCounterpartyCreditRiskExposureTree();
   
};



} // namespace analytics
} // namespace ore
