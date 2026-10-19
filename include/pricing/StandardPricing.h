#pragma once
#include <string>
#include "pricing/IPricingPolicy.h"

// Default rule: daily rate x number of days.
class StandardPricing : public IPricingPolicy {
public:
    double calculate(const IVehicle& vehicle, int days) const override {
        return vehicle.getDailyRate() * days;
    }

    std::string getName() const override { return "Standard pricing"; }
};
