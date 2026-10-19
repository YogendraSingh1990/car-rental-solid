#pragma once
#include <string>
#include "pricing/IPricingPolicy.h"

// EXTENSION: new requirement "give a discount on long rentals".
// Added as a brand-new class. No existing class was modified.
class LongRentalDiscountPricing : public IPricingPolicy {
public:
    explicit LongRentalDiscountPricing(int minDays = 5,
                                       double discountRate = 0.15)
        : minDays_(minDays), discountRate_(discountRate) {}

    double calculate(const IVehicle& vehicle, int days) const override {
        double base = vehicle.getDailyRate() * days;
        if (days >= minDays_)
            return base * (1.0 - discountRate_);
        return base;
    }

    std::string getName() const override {
        return "Long-rental discount (" +
               std::to_string(static_cast<int>(discountRate_ * 100 + 0.5)) +
               "% off for " + std::to_string(minDays_) + "+ days)";
    }

private:
    int minDays_;
    double discountRate_;
};
