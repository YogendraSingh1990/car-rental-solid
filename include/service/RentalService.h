#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "service/IRentalOperations.h"
#include "service/IFleetManagement.h"
#include "service/Rental.h"
#include "repo/IVehicleRepository.h"
#include "pricing/IPricingPolicy.h"
#include "notify/INotifier.h"

// SRP: coordinates the rental process only. Storage, pricing,
//      payment and messaging are delegated to other objects.
// DIP: depends only on interfaces, all supplied from outside
//      through the constructor (or, for payment, per rental).
// ISP: implements two separate role interfaces, so customers and
//      admins each see only the methods they need.
class RentalService : public IRentalOperations, public IFleetManagement {
public:
    RentalService(IVehicleRepository& repository,
                  const IPricingPolicy& pricing,
                  INotifier& notifier);

    // IFleetManagement (admin role)
    bool addVehicle(std::shared_ptr<IVehicle> vehicle) override;

    // IRentalOperations (customer role)
    std::vector<std::shared_ptr<const IVehicle>>
    getAvailableVehicles() const override;

    bool rentVehicle(const std::string& customer,
                     const std::string& vehicleId, int days,
                     IPayment& payment) override;

    bool returnVehicle(const std::string& customer,
                       const std::string& vehicleId) override;

private:
    IVehicleRepository& repository_;
    const IPricingPolicy& pricing_;
    INotifier& notifier_;
    std::unordered_map<std::string, Rental> activeRentals_;
};
