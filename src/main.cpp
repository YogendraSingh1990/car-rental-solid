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

// ===================================================================
// Shared display helpers
// ===================================================================

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

// ===================================================================
// Input helpers (console only; they know nothing about rentals)
// Each returns false if input has ended (e.g. Ctrl+D / Ctrl+Z).
// ===================================================================

std::string trim(const std::string& text) {
    const auto first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
        return "";
    const auto last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

bool readText(const std::string& prompt, std::string& out) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line))
            return false;
        out = trim(line);
        if (!out.empty())
            return true;
        std::cout << "  Please enter a value.\n";
    }
}

bool readInt(const std::string& prompt, int minValue, int maxValue, int& out) {
    std::string text;
    while (readText(prompt, text)) {
        try {
            std::size_t used = 0;
            int value = std::stoi(text, &used);
            if (used == text.size() && value >= minValue && value <= maxValue) {
                out = value;
                return true;
            }
        } catch (...) {
        }
        std::cout << "  Please enter a whole number from " << minValue
                  << " to " << maxValue << ".\n";
    }
    return false;
}

bool readPositiveDouble(const std::string& prompt, double& out) {
    std::string text;
    while (readText(prompt, text)) {
        try {
            std::size_t used = 0;
            double value = std::stod(text, &used);
            if (used == text.size() && value > 0) {
                out = value;
                return true;
            }
        } catch (...) {
        }
        std::cout << "  Please enter a number greater than 0.\n";
    }
    return false;
}

// ===================================================================
// Scripted demo (run with:  ./rental_app --demo)
// ===================================================================

void runDemo() {
    // Composition root: concrete classes are chosen ONLY here.
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
}

// ===================================================================
// Interactive menu (default mode:  ./rental_app)
// ===================================================================

// Lets the user pick a payment method. Returns nullptr if input ended.
IPayment* choosePayment(IPayment& upi, IPayment& card, IPayment& cash) {
    std::cout << "Payment method:  1. UPI   2. Card   3. Cash\n";
    int choice = 0;
    if (!readInt("Choose payment (1-3): ", 1, 3, choice))
        return nullptr;
    if (choice == 1) return &upi;
    if (choice == 2) return &card;
    return &cash;
}

// Asks the admin for vehicle details and builds the right object.
// Creating concrete vehicles here (not in Admin or RentalService)
// keeps those classes closed to modification (OCP).
std::shared_ptr<IVehicle> askForNewVehicle() {
    std::cout << "Vehicle type:  1. Car   2. Bike   3. Truck\n";
    int type = 0;
    std::string id, name;
    double rate = 0;
    if (!readInt("Choose type (1-3): ", 1, 3, type) ||
        !readText("Vehicle ID (e.g. C102): ", id) ||
        !readText("Vehicle name (e.g. Maruti Swift): ", name) ||
        !readPositiveDouble("Daily rate in Rs.: ", rate))
        return nullptr;

    if (type == 1)
        return std::make_shared<Car>(id, name, rate);
    if (type == 2)
        return std::make_shared<Bike>(id, name, rate);

    double capacity = 0;
    if (!readPositiveDouble("Cargo capacity in kg: ", capacity))
        return nullptr;
    return std::make_shared<Truck>(id, name, rate, capacity);
}

void runInteractive() {
    std::cout << "===== Campus Car Rental System =====\n";

    // DIP + OCP in action: the user picks which pricing object is
    // plugged into RentalService. RentalService itself never changes.
    std::cout << "\nPricing policy for this session:\n"
              << "  1. Standard pricing (rate x days)\n"
              << "  2. Long-rental discount (15% off for 5+ days)  [extension]\n";
    int pricingChoice = 0;
    if (!readInt("Choose pricing (1-2): ", 1, 2, pricingChoice))
        return;

    StandardPricing standardPricing;
    LongRentalDiscountPricing discountPricing;
    const IPricingPolicy& pricing =
        (pricingChoice == 1)
            ? static_cast<const IPricingPolicy&>(standardPricing)
            : static_cast<const IPricingPolicy&>(discountPricing);

    // Composition root: concrete classes are chosen ONLY here.
    InMemoryVehicleRepository repository;
    ConsoleNotifier notifier;
    RentalService service(repository, pricing, notifier);

    UpiPayment upi;
    CardPayment card;
    CashPayment cash;
    Admin admin;

    std::cout << "\nUsing: " << pricing.getName() << "\n\nLoading sample fleet...\n";
    admin.addVehicle(service, std::make_shared<Car>("C101", "Honda City", 2000));
    admin.addVehicle(service, std::make_shared<Bike>("B101", "Royal Enfield", 800));
    admin.addVehicle(service, std::make_shared<Truck>("T101", "Tata Ace", 1500, 750));

    while (true) {
        std::cout << "\n----------- MENU -----------\n"
                  << "1. Show available vehicles\n"
                  << "2. Rent a vehicle\n"
                  << "3. Return a vehicle\n"
                  << "4. Admin: add a vehicle\n"
                  << "5. Run the scripted demo\n"
                  << "0. Exit\n";

        int choice = 0;
        if (!readInt("Enter choice: ", 0, 5, choice))
            break;

        if (choice == 0) {
            std::cout << "Goodbye!\n";
            break;
        }

        switch (choice) {
            case 1:
                printAvailableVehicles(service);
                break;

            case 2: {
                printAvailableVehicles(service);
                std::string name, vehicleId;
                int days = 0;
                if (!readText("Your name: ", name) ||
                    !readText("Vehicle ID to rent: ", vehicleId) ||
                    !readInt("Number of days: ", 1, 365, days))
                    return;
                IPayment* payment = choosePayment(upi, card, cash);
                if (!payment)
                    return;
                Customer customer(name);
                customer.rent(service, vehicleId, days, *payment);
                break;
            }

            case 3: {
                std::string name, vehicleId;
                if (!readText("Your name: ", name) ||
                    !readText("Vehicle ID to return: ", vehicleId))
                    return;
                Customer customer(name);
                customer.returnVehicle(service, vehicleId);
                break;
            }

            case 4: {
                auto vehicle = askForNewVehicle();
                if (!vehicle)
                    return;
                admin.addVehicle(service, vehicle);
                break;
            }

            case 5:
                runDemo();
                break;
        }
    }
}

}  // namespace

// ./rental_app          -> interactive menu
// ./rental_app --demo   -> scripted demo (same output every time)
int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "--demo") {
        runDemo();
    } else {
        runInteractive();
    }
    return 0;
}