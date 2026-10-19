#pragma once
#include <string>
#include <utility>
#include "vehicles/IVehicle.h"

// Shared data and behaviour for all vehicles, written once.
// Subclasses only add what makes them different.
class VehicleBase : public IVehicle {
protected:
    VehicleBase(std::string id, std::string name, double dailyRate)
        : id_(std::move(id)), name_(std::move(name)), dailyRate_(dailyRate) {}

public:
    std::string getId() const override { return id_; }
    std::string getName() const override { return name_; }
    double getDailyRate() const override { return dailyRate_; }
    bool isAvailable() const override { return available_; }
    void setAvailable(bool available) override { available_ = available; }

private:
    std::string id_;
    std::string name_;
    double dailyRate_;
    bool available_ = true;
};
