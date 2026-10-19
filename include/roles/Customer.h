#pragma once
#include <string>
#include <utility>
#include "service/IRentalOperations.h"
#include "payment/IPayment.h"

// A customer only knows IRentalOperations (ISP + DIP).
// It cannot see or call admin-only methods such as addVehicle().
class Customer {
public:
    explicit Customer(std::string name) : name_(std::move(name)) {}

    const std::string& getName() const { return name_; }

    bool rent(IRentalOperations& rentals, const std::string& vehicleId,
              int days, IPayment& payment) const {
        return rentals.rentVehicle(name_, vehicleId, days, payment);
    }

    bool returnVehicle(IRentalOperations& rentals,
                       const std::string& vehicleId) const {
        return rentals.returnVehicle(name_, vehicleId);
    }

private:
    std::string name_;
};
