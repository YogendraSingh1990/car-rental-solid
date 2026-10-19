#pragma once
#include <memory>
#include "service/IFleetManagement.h"
#include "vehicles/IVehicle.h"

// An admin only knows IFleetManagement (ISP + DIP).
// One method accepts ANY vehicle type (OCP): adding a new type
// such as Truck needed no change here.
class Admin {
public:
    bool addVehicle(IFleetManagement& fleet,
                    std::shared_ptr<IVehicle> vehicle) const {
        return fleet.addVehicle(std::move(vehicle));
    }
};
