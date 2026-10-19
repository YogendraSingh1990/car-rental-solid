#pragma once
#include <string>
#include <utility>
#include "vehicles/VehicleBase.h"

class Bike : public VehicleBase {
public:
    Bike(std::string id, std::string name, double dailyRate)
        : VehicleBase(std::move(id), std::move(name), dailyRate) {}

    std::string getType() const override { return "Bike"; }
};
