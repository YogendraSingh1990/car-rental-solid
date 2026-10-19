#pragma once
#include <string>
#include <utility>
#include "vehicles/VehicleBase.h"

class Car : public VehicleBase {
public:
    Car(std::string id, std::string name, double dailyRate)
        : VehicleBase(std::move(id), std::move(name), dailyRate) {}

    std::string getType() const override { return "Car"; }
};
