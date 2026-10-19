#pragma once
#include <string>
#include "vehicles/IVehicle.h"

// OCP: a new pricing rule is a new class implementing this
// interface. RentalService never needs to change for it.
class IPricingPolicy {
public:
    virtual ~IPricingPolicy() = default;
    virtual double calculate(const IVehicle& vehicle, int days) const = 0;
    virtual std::string getName() const = 0;
};
