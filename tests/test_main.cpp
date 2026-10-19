// Simple tests without any external framework.
// Build:  g++ -std=c++17 -Iinclude tests/test_main.cpp src/RentalService.cpp -o rental_tests
// Run:    ./rental_tests

#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "vehicles/Car.h"
#include "vehicles/Bike.h"
#include "vehicles/Truck.h"
#include "payment/IPayment.h"
#include "pricing/StandardPricing.h"
#include "pricing/LongRentalDiscountPricing.h"
#include "repo/InMemoryVehicleRepository.h"
#include "notify/INotifier.h"
#include "service/RentalService.h"

namespace {

int passed = 0;
int failed = 0;

#define CHECK(condition)                                               \
    do {                                                               \
        if (condition) {                                               \
            ++passed;                                                  \
        } else {                                                       \
            ++failed;                                                  \
            std::cerr << "  FAILED: " #condition " (line " << __LINE__ \
                      << ")\n";                                        \
        }                                                              \
    } while (0)

bool near(double a, double b) { return std::fabs(a - b) < 1e-9; }

// ---------- Test doubles (possible because of DIP) ----------

// A payment we control: it can be told to fail, and it remembers
// what it was asked to charge.
class FakePayment : public IPayment {
public:
    bool shouldSucceed = true;
    int calls = 0;
    double lastAmount = -1;

    bool pay(double amount) override {
        ++calls;
        lastAmount = amount;
        return shouldSucceed;
    }
    std::string getMethodName() const override { return "Fake"; }
};

// Collects messages instead of printing them, keeping test output clean.
class RecordingNotifier : public INotifier {
public:
    std::vector<std::string> messages;
    void notify(const std::string& message) override {
        messages.push_back(message);
    }
};

// Fresh, isolated objects for each test.
struct TestSetup {
    InMemoryVehicleRepository repository;
    StandardPricing pricing;
    RecordingNotifier notifier;
    RentalService service{repository, pricing, notifier};
};

void run(const std::string& name, void (*test)()) {
    std::cout << "- " << name << '\n';
    test();
}

// ---------- Tests ----------

void rentingAvailableVehicleSucceeds() {
    TestSetup t;
    t.service.addVehicle(std::make_shared<Car>("C1", "Test Car", 1000));
    FakePayment payment;

    CHECK(t.service.rentVehicle("Asha", "C1", 2, payment));
    CHECK(!t.repository.findById("C1")->isAvailable());
    CHECK(near(payment.lastAmount, 2000));
    CHECK(t.service.getAvailableVehicles().empty());
}

void rentingRentedVehicleFails() {
    TestSetup t;
    t.service.addVehicle(std::make_shared<Car>("C1", "Test Car", 1000));
    FakePayment payment;

    CHECK(t.service.rentVehicle("Asha", "C1", 1, payment));
    CHECK(!t.service.rentVehicle("Ravi", "C1", 1, payment));
    CHECK(payment.calls == 1);  // second customer was never charged
}

void unknownVehicleFails() {
    TestSetup t;
    FakePayment payment;

    CHECK(!t.service.rentVehicle("Asha", "NOPE", 1, payment));
    CHECK(payment.calls == 0);
}

void nonPositiveDaysFail() {
    TestSetup t;
    t.service.addVehicle(std::make_shared<Car>("C1", "Test Car", 1000));
    FakePayment payment;

    CHECK(!t.service.rentVehicle("Asha", "C1", 0, payment));
    CHECK(!t.service.rentVehicle("Asha", "C1", -3, payment));
    CHECK(payment.calls == 0);
    CHECK(t.repository.findById("C1")->isAvailable());
}

void failedPaymentKeepsVehicleAvailable() {
    TestSetup t;
    t.service.addVehicle(std::make_shared<Car>("C1", "Test Car", 1000));
    FakePayment payment;
    payment.shouldSucceed = false;

    CHECK(!t.service.rentVehicle("Asha", "C1", 2, payment));
    CHECK(t.repository.findById("C1")->isAvailable());
    CHECK(t.service.getAvailableVehicles().size() == 1);
}

void returnRulesAreEnforced() {
    TestSetup t;
    t.service.addVehicle(std::make_shared<Car>("C1", "Test Car", 1000));
    FakePayment payment;

    CHECK(!t.service.returnVehicle("Asha", "C1"));  // not rented yet
    CHECK(t.service.rentVehicle("Asha", "C1", 1, payment));
    CHECK(!t.service.returnVehicle("Ravi", "C1"));  // wrong customer
    CHECK(t.service.returnVehicle("Asha", "C1"));   // correct customer
    CHECK(t.repository.findById("C1")->isAvailable());
    CHECK(!t.service.returnVehicle("Asha", "C1"));  // already returned
}

void duplicateVehicleIdIsRejected() {
    TestSetup t;
    CHECK(t.service.addVehicle(std::make_shared<Car>("C1", "First", 1000)));
    CHECK(!t.service.addVehicle(std::make_shared<Bike>("C1", "Second", 500)));
    CHECK(t.repository.getAll().size() == 1);
}

// LSP: every vehicle type works through the same IVehicle code path.
void allVehicleTypesAreSubstitutable() {
    TestSetup t;
    std::vector<std::shared_ptr<IVehicle>> fleet = {
        std::make_shared<Car>("C1", "Car", 2000),
        std::make_shared<Bike>("B1", "Bike", 800),
        std::make_shared<Truck>("T1", "Truck", 1500, 750),
    };

    for (const auto& vehicle : fleet)
        CHECK(t.service.addVehicle(vehicle));

    for (const auto& vehicle : fleet) {
        FakePayment payment;
        CHECK(vehicle->getDailyRate() > 0);
        CHECK(t.service.rentVehicle("Asha", vehicle->getId(), 3, payment));
        CHECK(near(payment.lastAmount, t.pricing.calculate(*vehicle, 3)));
        CHECK(!vehicle->isAvailable());
        CHECK(t.service.returnVehicle("Asha", vehicle->getId()));
        CHECK(vehicle->isAvailable());
    }
}

void truckRateIncludesLoadingFee() {
    Truck truck("T1", "Truck", 1500, 750);
    CHECK(near(truck.getDailyRate(), 1500 + Truck::kLoadingFeePerDay));
    CHECK(near(truck.getCargoCapacityKg(), 750));
}

void standardPricingIsRateTimesDays() {
    Car car("C1", "Car", 1200);
    StandardPricing pricing;
    CHECK(near(pricing.calculate(car, 1), 1200));
    CHECK(near(pricing.calculate(car, 4), 4800));
}

// EXTENSION: the new pricing policy, used by an unchanged RentalService.
void longRentalDiscountExtension() {
    Car car("C1", "Car", 1000);
    LongRentalDiscountPricing discount;  // 15% off for 5+ days

    CHECK(near(discount.calculate(car, 4), 4000));   // below threshold
    CHECK(near(discount.calculate(car, 5), 4250));   // 5000 - 15%
    CHECK(near(discount.calculate(car, 10), 8500));  // 10000 - 15%

    InMemoryVehicleRepository repository;
    RecordingNotifier notifier;
    RentalService service(repository, discount, notifier);
    service.addVehicle(std::make_shared<Car>("C1", "Car", 1000));

    FakePayment payment;
    CHECK(service.rentVehicle("Asha", "C1", 10, payment));
    CHECK(near(payment.lastAmount, 8500));
}

}  // namespace

int main() {
    std::cout << "Running tests...\n";

    run("Renting an available vehicle succeeds", rentingAvailableVehicleSucceeds);
    run("Renting an already-rented vehicle fails", rentingRentedVehicleFails);
    run("Renting an unknown vehicle fails", unknownVehicleFails);
    run("Zero or negative days are rejected", nonPositiveDaysFail);
    run("Failed payment leaves the vehicle available", failedPaymentKeepsVehicleAvailable);
    run("Only the renter can return, and only once", returnRulesAreEnforced);
    run("Duplicate vehicle IDs are rejected", duplicateVehicleIdIsRejected);
    run("LSP: Car, Bike and Truck are interchangeable", allVehicleTypesAreSubstitutable);
    run("Truck daily rate includes loading fee", truckRateIncludesLoadingFee);
    run("Standard pricing = rate x days", standardPricingIsRateTimesDays);
    run("Extension: long-rental discount pricing", longRentalDiscountExtension);

    std::cout << "\nPassed: " << passed << "  Failed: " << failed << '\n';
    return failed == 0 ? 0 : 1;
}
