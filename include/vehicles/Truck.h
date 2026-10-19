#pragma once
#include <string>
#include <utility>
#include "vehicles/VehicleBase.h"
#include "vehicles/ICargoCarrier.h"

// A Truck is a vehicle (IVehicle via VehicleBase) that can also
// carry cargo (ICargoCarrier).
// LSP: its daily rate includes a loading fee, but it is still a
// positive amount, so code using IVehicle works unchanged.
class Truck : public VehicleBase, public ICargoCarrier {
public:
    static constexpr double kLoadingFeePerDay = 500.0;

    Truck(std::string id, std::string name, double baseDailyRate,
          double cargoCapacityKg)
        : VehicleBase(std::move(id), std::move(name), baseDailyRate),
          cargoCapacityKg_(cargoCapacityKg) {}

    std::string getType() const override { return "Truck"; }

    double getDailyRate() const override {
        return VehicleBase::getDailyRate() + kLoadingFeePerDay;
    }

    double getCargoCapacityKg() const override { return cargoCapacityKg_; }

private:
    double cargoCapacityKg_;
};
