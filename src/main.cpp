#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

#include "vehicles/Car.h"
#include "vehicles/Bike.h"
#include "vehicles/Truck.h"
#include "payment/UpiPayment.h"
#include "payment/CardPayment.h"
#include "payment/CashPayment.h"
#include "pricing/StandardPricing.h"
#include "pricing/LongRentalDiscountPricing.h"
#include "repo/InMemoryVehicleRepository.h"
#include "notify/ConsoleNotifier.h"
#include "service/RentalService.h"
#include "roles/Admin.h"
#include "roles/Customer.h"

namespace {

void printSection(const std::string& title) {
    std::cout << "\n=== " << title << " ===\n";
}

// Works with anything that offers IRentalOperations.
void printAvailableVehicles(const IRentalOperations& rentals) {
    std::cout << "Available vehicles:\n";
    auto vehicles = rentals.getAvailableVehicles();
    if (vehicles.empty()) {
        std::cout << "  (none)\n";
        return;
    }
    for (const auto& vehicle : vehicles) {
        std::cout << "  [" << vehicle->getType() << "] " << vehicle->getId()
                  << " | " << vehicle->getName() << " | Rs. " << std::fixed
                  << std::setprecision(2) << vehicle->getDailyRate()
                  << "/day\n";
    }
}

// ISP: this function only needs cargo information, so it only
// accepts ICargoCarrier. Cars and Bikes are never involved.
void printCargoInfo(const std::string& name, const ICargoCarrier& carrier) {
    std::cout << name << " can carry " << std::fixed << std::setprecision(0)
              << carrier.getCargoCapacityKg() << " kg of cargo.\n";
}

}  // namespace

int main() {
    // ---- Composition root: concrete classes are chosen ONLY here ----
    InMemoryVehicleRepository repository;
    StandardPricing standardPricing;
    ConsoleNotifier notifier;
    RentalService service(repository, standardPricing, notifier);

    UpiPayment upi;
    CardPayment card;
    CashPayment cash;

    Admin admin;
    Customer arun("Arun");
    Customer priya("Priya");
    Customer kavya("Kavya");

    printSection("1. Admin adds vehicles");
    auto truck = std::make_shared<Truck>("T101", "Tata Ace", 1500, 750);
    admin.addVehicle(service, std::make_shared<Car>("C101", "Honda City", 2000));
    admin.addVehicle(service, std::make_shared<Bike>("B101", "Royal Enfield", 800));
    admin.addVehicle(service, truck);
    admin.addVehicle(service, std::make_shared<Car>("C101", "Duplicate Car", 999));
    printCargoInfo(truck->getName(), *truck);
    printAvailableVehicles(service);

    printSection("2. Customers rent using different payment methods");
    arun.rent(service, "C101", 3, upi);
    priya.rent(service, "B101", 2, card);
    kavya.rent(service, "T101", 1, cash);
    printAvailableVehicles(service);

    printSection("3. Invalid requests are rejected");
    priya.rent(service, "C101", 1, card);   // already rented by Arun
    priya.rent(service, "X999", 1, card);   // no such vehicle
    priya.rent(service, "B101", 0, card);   // invalid number of days
    priya.returnVehicle(service, "C101");   // Priya did not rent C101

    printSection("4. Customers return vehicles");
    arun.returnVehicle(service, "C101");
    priya.returnVehicle(service, "B101");
    kavya.returnVehicle(service, "T101");
    printAvailableVehicles(service);

    printSection("5. Extension: long-rental discount pricing");
    std::cout << "Same 6-day rental of the Honda City, two pricing policies.\n"
              << "Only the pricing object passed to RentalService differs.\n\n";

    std::cout << "-- With StandardPricing --\n";
    arun.rent(service, "C101", 6, upi);
    arun.returnVehicle(service, "C101");

    LongRentalDiscountPricing discountPricing;  // the new class
    RentalService discountService(repository, discountPricing, notifier);

    std::cout << "\n-- With LongRentalDiscountPricing --\n";
    arun.rent(discountService, "C101", 6, upi);
    arun.returnVehicle(discountService, "C101");

    return 0;
}
