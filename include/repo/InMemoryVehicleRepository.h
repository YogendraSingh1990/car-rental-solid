#pragma once
#include <memory>
#include <string>
#include <vector>
#include "repo/IVehicleRepository.h"

// Keeps vehicles in memory. Could be swapped for a file or
// database version without changing RentalService.
class InMemoryVehicleRepository : public IVehicleRepository {
public:
    void add(std::shared_ptr<IVehicle> vehicle) override {
        vehicles_.push_back(std::move(vehicle));
    }

    std::shared_ptr<IVehicle> findById(const std::string& id) const override {
        for (const auto& vehicle : vehicles_) {
            if (vehicle->getId() == id)
                return vehicle;
        }
        return nullptr;
    }

    std::vector<std::shared_ptr<IVehicle>> getAll() const override {
        return vehicles_;
    }

private:
    std::vector<std::shared_ptr<IVehicle>> vehicles_;
};
