#pragma once
#include <memory>
#include <string>
#include <vector>
#include "vehicles/IVehicle.h"
#include "payment/IPayment.h"

// ISP: the actions a customer is allowed to perform.
// Customers get read-only (const) access to vehicles, so they
// cannot change a vehicle's availability directly.
class IRentalOperations {
public:
    virtual ~IRentalOperations() = default;

    virtual std::vector<std::shared_ptr<const IVehicle>>
    getAvailableVehicles() const = 0;

    virtual bool rentVehicle(const std::string& customer,
                             const std::string& vehicleId, int days,
                             IPayment& payment) = 0;

    virtual bool returnVehicle(const std::string& customer,
                               const std::string& vehicleId) = 0;
};
