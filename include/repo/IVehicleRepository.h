#pragma once
#include <memory>
#include <string>
#include <vector>
#include "vehicles/IVehicle.h"

// SRP: storing and finding vehicles is this abstraction's only job.
class IVehicleRepository {
public:
    virtual ~IVehicleRepository() = default;
    virtual void add(std::shared_ptr<IVehicle> vehicle) = 0;
    // Returns nullptr if no vehicle has this ID.
    virtual std::shared_ptr<IVehicle> findById(const std::string& id) const = 0;
    virtual std::vector<std::shared_ptr<IVehicle>> getAll() const = 0;
};
