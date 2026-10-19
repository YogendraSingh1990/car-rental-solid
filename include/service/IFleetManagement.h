#pragma once
#include <memory>
#include "vehicles/IVehicle.h"

// ISP: the actions an admin is allowed to perform.
class IFleetManagement {
public:
    virtual ~IFleetManagement() = default;
    // Returns false if a vehicle with the same ID already exists.
    virtual bool addVehicle(std::shared_ptr<IVehicle> vehicle) = 0;
};
