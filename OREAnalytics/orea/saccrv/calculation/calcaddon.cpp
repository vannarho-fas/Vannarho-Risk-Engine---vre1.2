/*! \file /OREAnalytics/orea/saccrv/calculation/calcaddon.cpp
    \brief Implementation of the SaccrvAddonCalculator class methods to compute the SA-CCR add-on for a collection of trades.
    \ingroup saccrv
*/


#include <orea/saccrv/saccrvinputmanager.hpp>
#include <orea/saccrv/calculation/calcaddon.hpp>
#include <orea/saccrv/saccrvcalculationmanager.hpp>
#include <orea/saccrv/calculation/singletradeaddon.hpp>
#include <orea/saccrv/calculation/calculatefactormult.hpp>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>


namespace ore {
namespace analytics {

SaccrvAddonCalculator::SaccrvAddonCalculator(SaccrvCalculationContext context)
: simplified_(context.simplified), oem_(context.OEM), counterpartyId_(context.counterpartyId), nettingsetId_(context.nettingsetId), isMargined_(context.isMargined), mporDays_(context.mporDays), hasIlliquidCollateral_(context.hasIlliquidCollateralOrOTCNotEasilyReplaced), applyMarginCallDisputes_(context.applyMarginCallDisputes), greaterThan5000Transactions_(context.greaterThan5000TransactionsNotCentrallyCleared), remarginFreq_(context.remarginFreq) {}

double SaccrvAddonCalculator::calculate() {
    
    // Get the netting set data
    auto saccrvCalcManager = SaccrvCalculationManager::getInstance();
    NettingSet* nettingSet = saccrvCalcManager->getNettingSet(counterpartyId_, nettingsetId_);
    double totalAddon = 0.0;

    if (nettingSet != nullptr) {
        // Process IRD asset class
        totalAddon += calculateIRDAddon(nettingSet->assetClasses.ird);

        // Process Commodity asset class
        totalAddon += calculateCommodityAddon(nettingSet->assetClasses.commodity);

        // Process Credit asset class
        totalAddon += calculateCreditAddon(nettingSet->assetClasses.credit);

        // Process FX asset class
        totalAddon += calculateFXAddon(nettingSet->assetClasses.fx);

        // Process Equity asset class
        totalAddon += calculateEquityAddon(nettingSet->assetClasses.equity);

        // Process OtherExposure asset class
        totalAddon += calculateOtherExposureAddon(nettingSet->assetClasses.otherExposure);

        // Update the add-on in the netting set data
        nettingSet->addOn = totalAddon;
        return totalAddon;
    } else {
        WLOG("Nettingset not found in the counterparty data. Netting set ID: " << nettingsetId_);
        return 0.0;
    }
}


double SaccrvAddonCalculator::calculateIRDAddon(IRDData& irdData) {
    double totalAddon = 0.0;
    auto supervisoryFactors = SaccrvConfigManager::getInstance()->getConfigurations().getSupervisoryFactors();

    // Iterate over each currency in the IRD asset class

    LOG("SACCRV: calculateIRDAddon - Number of currencies: " << irdData.currencies.size());
    for (auto& currency : irdData.currencies) {
        double currencyAddon = 0.0;
        double currencyEffectiveNotional = 0.0;
        std::vector<double> bucketEffectiveNotionals(3, 0.0); // Effective notionals for each maturity bucket
        LOG("SACCRV: calculateIRDAddon - Number of time buckets: " << currency.timeBuckets.size());

        // Determine supervisory factor 
        double supervisoryFactor;
        if (!simplified_ && !oem_ && !currency.timeBuckets.empty()) {
            // Adjust the supervisory factor based on trade characteristics
            LOG("SACCRV: calculateIRDAddon - Adjusting supervisory factor based on trade characteristics");
            std::string tradeGroup = "IRD";
            std::string subClass = ""; // no subclass for IRD
            supervisoryFactor = supervisoryFactors[tradeGroup][subClass].supervisory_factor;
        } else {
            supervisoryFactor = 0.005; // Default supervisory factor for IRD
        }
        // Update the supervisory factor for the currency
        currency.supervisoryFactor = supervisoryFactor;

        // Iterate over each time bucket in the currency
        for (auto& timeBucket : currency.timeBuckets) {
            double bucketEffectiveNotional = 0.0;

            // Iterate over each trade in the time bucket
            for (auto& trade : timeBucket.tradeData) {
                int tradeIndex = &trade - &timeBucket.tradeData[0] + 1;
                int totalTrades = timeBucket.tradeData.size();
                LOG("SACCRV: calculateIRDAddon - Processing trade: " << tradeIndex << " of " << totalTrades << ", Trade ID: " << trade.trades.getId());
                double maturityFactor = getMaturityFactor(trade.trades);
                SingleTradeAddon tradeAddon(trade.trades, maturityFactor, simplified_);
                tradeAddon.calculateAddon();

                // Update TradeData with calculated values
                trade.exposureDetails.adjNotional = tradeAddon.getAdjustedNotional();
                trade.exposureDetails.maturityFactor = maturityFactor;
                trade.exposureDetails.supervDelta = tradeAddon.getSupervDelta();
                trade.exposureDetails.effectiveNotional = tradeAddon.getEffectiveNotional();

                // Add to the bucket effective notional
                bucketEffectiveNotional += tradeAddon.getEffectiveNotional();
            }

            // Update the effective notional for the time bucket
            timeBucket.effectiveNotional = bucketEffectiveNotional;

            // Add to the corresponding maturity bucket
            LOG("SACCRV: Calcaddon - timeBucketNum: " << timeBucket.timeBucketNum)
            int timeBucketInt = timeBucket.timeBucketNum;
            if (timeBucketInt >= 1 && timeBucketInt <= 3) {
                bucketEffectiveNotionals[timeBucketInt - 1] += bucketEffectiveNotional;
            } else {
                WLOG("Invalid maturity bucket: " << timeBucket.timeBucketNum);
            }
        }

        // Calculate the effective notional of the hedging set (ENHS) recognising offsets CRE52.57 step 5
        double ENHS = std::sqrt(
            std::pow(bucketEffectiveNotionals[0] + 1.4 * bucketEffectiveNotionals[1] + 0.6 * bucketEffectiveNotionals[2], 2) +
            std::pow(1.4 * bucketEffectiveNotionals[1] + bucketEffectiveNotionals[2], 2)
        );

        // Calculate the currency add-on
        currencyAddon = supervisoryFactor * ENHS;
        LOG("SACCRV: currencyAddon: " << currencyAddon << " for currency: " << currency.currency << " with supervfactor: " << supervisoryFactor << " and ENHS: " << ENHS); 

        // Update addOn for the currency
        currency.addon = currencyAddon;
        currencyEffectiveNotional = std::abs(ENHS);
    
        totalAddon += currencyAddon;
    }

    // Update the add-on in the IRD data
    irdData.addon = totalAddon;

    return totalAddon;
}


double SaccrvAddonCalculator::calculateFXAddon(FXData& fxData) {
    double totalAddon = 0.0;
    double supervisoryFactor = 0.04; // Prescribed supervisory factor for FX

    // Iterate through each currency pair
    for (auto& currencyPair : fxData.currencyPairs) {
        double pairAddon = 0.0;
        double pairEffectiveNotional = 0.0;

        // Calculate the add-on for each trade in the currency pair
        for (auto& trade : currencyPair.tradeData) {
            double maturityFactor = getMaturityFactor(trade.trades);
            SingleTradeAddon tradeAddon(trade.trades, maturityFactor, simplified_);
            tradeAddon.calculateAddon();

            // Update TradeData with calculated values
            trade.exposureDetails.adjNotional = tradeAddon.getAdjustedNotional();
            trade.exposureDetails.maturityFactor = maturityFactor;
            trade.exposureDetails.supervDelta = tradeAddon.getSupervDelta();
            trade.exposureDetails.effectiveNotional = tradeAddon.getEffectiveNotional();

            // Aggregate pair effective notional
            pairEffectiveNotional += trade.exposureDetails.effectiveNotional;
        }

        // Calculate pair add-on
        pairAddon = std::abs(pairEffectiveNotional) * supervisoryFactor;

        // Update addOn and supervisory factor for the currency pair
        currencyPair.addon = pairAddon;
        currencyPair.supervisoryFactor = supervisoryFactor;
        
        totalAddon += pairAddon;
    }

    fxData.addon = totalAddon;
    return totalAddon;
}


double SaccrvAddonCalculator::calculateCommodityAddon(CommodityData& commodityData) {
    double totalAddon = 0.0;
    auto supervisoryFactors = SaccrvConfigManager::getInstance()->getConfigurations().getSupervisoryFactors();

    // Iterating over hedging sets for commodities
    for (auto& hedgingSet : commodityData.hedgingSets) {
        double hedgingSetAddon = 0.0;
        double hedgingSetAddonSquared = 0.0;

        // Iterating over commodities types in a hedging set
        for (auto& commodityType : hedgingSet.commodityTypes) {
            double commodityTypeAddon = 0.0;
            double commodityTypeEffectiveNotional = 0.0;

            // Determine supervisory factor for the commodity
            double supervisoryFactor;
            if (oem_) {
                supervisoryFactor = 0.06; // Example factor for OEM
            } else {
                supervisoryFactor = supervisoryFactors["Commodity"][commodityType.commodity].supervisory_factor;
            }

            // Iterating over trades for a commodity type
            for (auto& trade : commodityType.tradeData) {
                double maturityFactor = getMaturityFactor(trade.trades);
                SingleTradeAddon tradeAddon(trade.trades, maturityFactor, simplified_);
                tradeAddon.calculateAddon();

                // Update TradeData with calculated values
                trade.exposureDetails.adjNotional = tradeAddon.getAdjustedNotional();
                trade.exposureDetails.maturityFactor = maturityFactor;
                trade.exposureDetails.supervDelta = tradeAddon.getSupervDelta();
                if (oem_) {
                    if (trade.trades.notional && trade.trades.Ei) {
                        trade.exposureDetails.effectiveNotional = *(trade.trades.notional) * *(trade.trades.Ei);
                    } else {
                        // Handle the case where notional or Ei doesn't have a value
                    }
                } else {
                    trade.exposureDetails.effectiveNotional = tradeAddon.getEffectiveNotional();
                }

                // Aggregate type addOn and effective notional
                commodityTypeAddon += trade.exposureDetails.effectiveNotional * supervisoryFactor;
                commodityTypeEffectiveNotional += trade.exposureDetails.adjNotional;
            }

            // Update addOn for the commodity type
            commodityType.addon = commodityTypeAddon;
            commodityType.supervisoryFactor = supervisoryFactor;
            commodityType.effectiveNotional = commodityTypeEffectiveNotional;

            hedgingSetAddon += commodityTypeAddon;
            hedgingSetAddonSquared += std::pow(commodityTypeAddon, 2);
        }

        double supervisoryCorrelation = simplified_ ? 1.0 : 0.4; // Supervisory correlation for commodities
        double hedgingSetTotalAddon = std::sqrt(std::pow(hedgingSetAddon * supervisoryCorrelation, 2) + (1 - std::pow(supervisoryCorrelation, 2)) * hedgingSetAddonSquared);

        // Update addOn for the hedging set
        hedgingSet.addon = hedgingSetTotalAddon;
        hedgingSet.supervisoryCorel = supervisoryCorrelation;                             
        totalAddon += hedgingSetTotalAddon;
    }
    commodityData.addon = totalAddon;
    return totalAddon;
}


double SaccrvAddonCalculator::calculateCreditAddon(CreditData& creditData) {
    double totalAddon = 0.0;
    auto supervisoryFactors = SaccrvConfigManager::getInstance()->getConfigurations().getSupervisoryFactors();

    // Maps to store the combined effective notional and add-on for each entity
    std::map<std::string, double> combinedEffectiveNotionals;
    std::map<std::string, double> entityAddOns;

    // Step 1: Calculate the effective notional for each trade
    for (auto& refEntity : creditData.referenceEntities) {
        for (auto& trade : refEntity.tradeData) {
            double maturityFactor = getMaturityFactor(trade.trades);
            SingleTradeAddon tradeAddon(trade.trades, maturityFactor, simplified_);
            tradeAddon.calculateAddon();

            // Calculate effective notional for the trade
            double effectiveNotional = tradeAddon.getAdjustedNotional() * tradeAddon.getSupervDelta() * maturityFactor;
            trade.exposureDetails.effectiveNotional = effectiveNotional;
            trade.exposureDetails.adjNotional = tradeAddon.getAdjustedNotional();
            trade.exposureDetails.maturityFactor = maturityFactor;
            trade.exposureDetails.supervDelta = tradeAddon.getSupervDelta();

            // Step 2: Calculate the combined effective notional for all derivatives that reference the same entity
            combinedEffectiveNotionals[refEntity.referenceEntity] += effectiveNotional;
        }
    }

    // Step 3: Calculate the add-on for each entity
    for (auto& refEntity : creditData.referenceEntities) {
        double supervisoryFactor = supervisoryFactors["Credit"][refEntity.referenceEntity].supervisory_factor;
        double correlation = supervisoryFactors["Credit"][refEntity.referenceEntity].correlation;
        double entityEffectiveNotional = combinedEffectiveNotionals[refEntity.referenceEntity];
        entityAddOns[refEntity.referenceEntity] = entityEffectiveNotional * supervisoryFactor;
        refEntity.supervisoryCorel = correlation;
        refEntity.supervisoryFactor = supervisoryFactor;
    }

    // Step 4: Calculate the asset class level add-on
    double systematicComponent = 0.0;
    double idiosyncraticComponent = 0.0;
    for (auto& addonPair : entityAddOns) {
        double correlation = supervisoryFactors["Credit"][addonPair.first].correlation;
        systematicComponent += correlation * addonPair.second;
        idiosyncraticComponent += (1 - std::pow(correlation, 2)) * std::pow(addonPair.second, 2);
    }
    systematicComponent = std::pow(systematicComponent, 2);

    totalAddon = std::sqrt(systematicComponent + idiosyncraticComponent);

    creditData.addon = totalAddon;
    creditData.idiosynchraticComponent = idiosyncraticComponent;
    creditData.systematicComponent = systematicComponent;

    return totalAddon;
}



double SaccrvAddonCalculator::calculateEquityAddon(EquityData& equityData) {
    double totalAddon = 0.0;
    double systematicComponent = 0.0;
    double idiosyncraticComponent = 0.0;
    std::vector<double> addons;
    std::vector<double> supervisoryCorrelations;

    auto supervisoryFactors = SaccrvConfigManager::getInstance()->getConfigurations().getSupervisoryFactors();

    // Iterate through reference entities for the equity asset class
    for (auto& refEntity : equityData.referenceEntities) {
        refEntity.addon = 0.0;

        // Iterate through each trade associated with the reference entity
        for (auto& trade : refEntity.tradeData) {
            double maturityFactor = getMaturityFactor(trade.trades);
            SingleTradeAddon tradeAddon(trade.trades, maturityFactor, simplified_);
            tradeAddon.calculateAddon();

            // Update TradeData with calculated values
            trade.exposureDetails.adjNotional = tradeAddon.getAdjustedNotional();
            trade.exposureDetails.maturityFactor = maturityFactor;
            trade.exposureDetails.supervDelta = tradeAddon.getSupervDelta();
            trade.exposureDetails.effectiveNotional = tradeAddon.getEffectiveNotional();

            // Add the effective notional to the addOn for the reference entity
            refEntity.addon += trade.exposureDetails.effectiveNotional;
        }

        std::string assetClass = "EQ";
        double supervisoryFactor;
        if (oem_) {
            supervisoryFactor = 0.32;
        } else {
            double factorMult = FactorMultiplierCalculator::CalculateFactorMult(refEntity.referenceEntity);
            supervisoryFactor = factorMult * supervisoryFactors[assetClass][""].supervisory_factor; //TODO check no equity subclasses in superv table?
        }

        refEntity.addon *= supervisoryFactor;
        double correlation = supervisoryFactors[assetClass][""].correlation; //TODO check
        addons.push_back(refEntity.addon);
        supervisoryCorrelations.push_back(correlation);

        refEntity.supervisoryCorel = correlation;
        refEntity.supervisoryFactor = supervisoryFactor;
    }

    // Calculate systematic and idiosyncratic components
    systematicComponent += std::pow(std::inner_product(supervisoryCorrelations.begin(), supervisoryCorrelations.end(), addons.begin(), 0.0), 2);
    idiosyncraticComponent += std::inner_product(addons.begin(), addons.end(), addons.begin(), 0.0, std::plus<double>(), 
                                                 [](double addOn, double correlation) {
                                                     return addOn * (1 - std::pow(correlation, 2));
                                                 });

    totalAddon = std::sqrt(systematicComponent + idiosyncraticComponent);
    equityData.addon = totalAddon;
    equityData.idiosynchraticComponent = idiosyncraticComponent;
    equityData.systematicComponent = systematicComponent;
    
    return totalAddon;
}


double SaccrvAddonCalculator::calculateOtherExposureAddon(OtherExposureData& otherData) {
    double totalAddon = 0.0;
    auto supervisoryFactors = SaccrvConfigManager::getInstance()->getConfigurations().getSupervisoryFactors();

    // Iterate over each subclass in 'Other Exposure'
    for (auto& otherSubClass : otherData.otherSubClasses) {
        otherSubClass.addon = 0.0;

        // Calculate the add-on for each trade under the subclass
        for (auto& trade : otherSubClass.tradeData) {
            double maturityFactor = getMaturityFactor(trade.trades);
            SingleTradeAddon tradeAddon(trade.trades, maturityFactor, simplified_);
            tradeAddon.calculateAddon();

            // Update TradeData with calculated values
            trade.exposureDetails.adjNotional = tradeAddon.getAdjustedNotional();
            trade.exposureDetails.maturityFactor = maturityFactor;
            trade.exposureDetails.supervDelta = tradeAddon.getSupervDelta();
            trade.exposureDetails.effectiveNotional = tradeAddon.getEffectiveNotional();

            // Add the effective notional to the addOn for the subclass
            otherSubClass.addon += trade.exposureDetails.effectiveNotional;

        }

        double factorMult = FactorMultiplierCalculator::CalculateFactorMult("");
        double supervisoryFactor = 0.08 * factorMult;

        otherSubClass.supervisoryFactor = supervisoryFactor;
        double subclassAddon = supervisoryFactor * otherSubClass.addon;
        otherSubClass.addon = std::abs(subclassAddon);

        totalAddon += std::abs(subclassAddon);
    }

    otherData.addon = totalAddon;
    return totalAddon;
}

double SaccrvAddonCalculator::aggregateAddonsWithCorrelation(const std::vector<double>& addons, const std::vector<double>& correlations) {
    double systematicComponent = 0.0;
    double idiosyncraticComponent = 0.0;


    // Systematic component calculation (taking into account the correlations)
    for (size_t i = 0; i < addons.size(); ++i) {
        systematicComponent += addons[i] * correlations[i];
    }
    systematicComponent = std::pow(systematicComponent, 2);

    // Idiosyncratic component calculation
    for (size_t i = 0; i < addons.size(); ++i) {
        idiosyncraticComponent += (1 - std::pow(correlations[i], 2)) * std::pow(addons[i], 2);
    }

    // Total addOn is the square root of the sum of systematic and idiosyncratic components
    return std::sqrt(systematicComponent + idiosyncraticComponent);
}

double SaccrvAddonCalculator::getMaturityFactor(const SaccrvTrades& trade) {
    double Ei = trade.getEi().value_or(1.0); // Default to 1 if Ei is not available
    double Si = trade.getSi().value_or(0.0); // Default to 0 if Si is not available
    LOG("SACCRV: Ei: " << Ei << " Si: " << Si << " isMargined: " << isMargined_ << " mporDays: " << mporDays_ << " hasIlliquidCollateralOrOTCNotEasilyReplaced: " << hasIlliquidCollateral_ << " applyMarginCallDisputes: " << applyMarginCallDisputes_ << " greaterThan5000TransactionsNotCentrallyCleared: " << greaterThan5000Transactions_ << "");
    // Calculate the effective maturity as the maximum of (Ei - Si, 10 days), expressed in years
    double effectiveMaturity = std::max((Ei - Si), 10.0 / 250.0);

    // Calculate the maturity factor using the SA-CCR formula in 52.48 to 52.52
    double maturityFactor;
    if (isMargined_) {
        // For margined transactions
        int MPORi = mporDays_; //
        // Apply the rules for exceptions to the floors on the minimum margin period of risk
        if (greaterThan5000Transactions_) {
            MPORi = std::max(MPORi, 20);
        }
        if (hasIlliquidCollateral_) {
            MPORi = std::max(MPORi, 20);
        }
        if (applyMarginCallDisputes_) {
            MPORi = MPORi * 2;
        }
        if (remarginFreq_ > 1) { // For non-centrally cleared transactions that are not subject to daily margin agreements
            MPORi = std::max(MPORi, 9 + remarginFreq_);
        }
        maturityFactor = std::sqrt(std::min(static_cast<double>(MPORi) / 250.0, 1.0));
    } else {
        // For unmargined transactions
        maturityFactor = std::sqrt(std::min(effectiveMaturity, 1.0));
    }

    return maturityFactor;
}

} // namespace analytics
} // namespace ore

