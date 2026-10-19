#include "service/RentalService.h"

#include <iomanip>
#include <sstream>

namespace {
std::string formatMoney(double amount) {
    std::ostringstream out;
    out << "Rs. " << std::fixed << std::setprecision(2) << amount;
    return out.str();
}
}  // namespace

RentalService::RentalService(IVehicleRepository& repository,
                             const IPricingPolicy& pricing,
                             INotifier& notifier)
    : repository_(repository), pricing_(pricing), notifier_(notifier) {}

bool RentalService::addVehicle(std::shared_ptr<IVehicle> vehicle) {
    if (!vehicle)
        return false;

    if (repository_.findById(vehicle->getId())) {
        notifier_.notify("Cannot add " + vehicle->getName() + ": ID '" +
                         vehicle->getId() + "' is already in use.");
        return false;
    }

    notifier_.notify(vehicle->getType() + " " + vehicle->getName() + " (" +
                     vehicle->getId() + ") added to the fleet.");
    repository_.add(std::move(vehicle));
    return true;
}

std::vector<std::shared_ptr<const IVehicle>>
RentalService::getAvailableVehicles() const {
    std::vector<std::shared_ptr<const IVehicle>> available;
    for (const auto& vehicle : repository_.getAll()) {
        if (vehicle->isAvailable())
            available.push_back(vehicle);
    }
    return available;
}

bool RentalService::rentVehicle(const std::string& customer,
                                const std::string& vehicleId, int days,
                                IPayment& payment) {
    if (days <= 0) {
        notifier_.notify("Rental rejected: number of days must be positive.");
        return false;
    }

    auto vehicle = repository_.findById(vehicleId);
    if (!vehicle) {
        notifier_.notify("Rental rejected: vehicle ID '" + vehicleId +
                         "' not found.");
        return false;
    }

    // Availability is the single source of truth for "is it rented?"
    if (!vehicle->isAvailable()) {
        notifier_.notify("Rental rejected: " + vehicle->getName() +
                         " is already rented.");
        return false;
    }

    double cost = pricing_.calculate(*vehicle, days);
    notifier_.notify("Quote for " + customer + ": " + vehicle->getName() +
                     " for " + std::to_string(days) + " day(s) = " +
                     formatMoney(cost) + " [" + pricing_.getName() + "]");

    // Mark the vehicle as rented only after payment succeeds.
    if (!payment.pay(cost)) {
        notifier_.notify("Payment via " + payment.getMethodName() +
                         " failed. Rental cancelled.");
        return false;
    }

    vehicle->setAvailable(false);
    activeRentals_[vehicleId] = Rental{customer, vehicleId, days, cost};

    notifier_.notify("Paid " + formatMoney(cost) + " via " +
                     payment.getMethodName() + ". " + vehicle->getName() +
                     " rented to " + customer + ".");
    return true;
}

bool RentalService::returnVehicle(const std::string& customer,
                                  const std::string& vehicleId) {
    auto rental = activeRentals_.find(vehicleId);
    if (rental == activeRentals_.end()) {
        notifier_.notify("Return rejected: no active rental for '" +
                         vehicleId + "'.");
        return false;
    }

    if (rental->second.customerName != customer) {
        notifier_.notify("Return rejected: '" + vehicleId +
                         "' was not rented by " + customer + ".");
        return false;
    }

    auto vehicle = repository_.findById(vehicleId);
    if (!vehicle)
        return false;

    vehicle->setAvailable(true);
    activeRentals_.erase(rental);

    notifier_.notify(vehicle->getName() + " returned by " + customer + ".");
    return true;
}
