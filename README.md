# Car Rental System – SOLID Principles

BCSE102L – Structured and Object-Oriented Programming, SOLID case study (Part B).

## Problem

A campus car rental desk lends cars, bikes and trucks to customers. Requirements are expected to grow: new vehicle types, new payment methods, new pricing rules and new user roles. The goal is a design where these can be added with minimal changes to existing code, demonstrated through all five SOLID principles.

## Features

- Admins add vehicles (Car, Bike, Truck) to the fleet; duplicate IDs are rejected.
- Customers view available vehicles, rent them for a number of days and return them.
- Each rental chooses its own payment method: UPI, Card or Cash.
- Pricing is pluggable: standard pricing, plus a long-rental discount (the extension).
- Trucks carry cargo and include a loading fee in their daily rate.
- Invalid requests are rejected: unknown vehicle, already rented, non-positive days, failed payment, or returning a vehicle rented by someone else.

## Project structure

```
include/
  vehicles/  IVehicle, VehicleBase, Car, Bike, Truck, ICargoCarrier
  payment/   IPayment, UpiPayment, CardPayment, CashPayment
  pricing/   IPricingPolicy, StandardPricing, LongRentalDiscountPricing
  repo/      IVehicleRepository, InMemoryVehicleRepository
  notify/    INotifier, ConsoleNotifier
  service/   IRentalOperations, IFleetManagement, Rental, RentalService
  roles/     Customer, Admin
src/         RentalService.cpp, main.cpp
tests/       test_main.cpp
```

## Build and run

Requires a C++17 compiler (g++ 7 or newer). Run these from the project folder.

**Linux / macOS / WSL**
```
g++ -std=c++17 -Iinclude src/*.cpp -o rental_app
./rental_app
```

**Windows (MinGW g++)**
```
g++ -std=c++17 -Iinclude src\RentalService.cpp src\main.cpp -o rental_app.exe
.\rental_app.exe
```

## Run the tests

```
g++ -std=c++17 -Iinclude tests/test_main.cpp src/RentalService.cpp -o rental_tests
./rental_tests
```

Expected last line: `Passed: 55  Failed: 0`. The tests use a `FakePayment` (which can be made to fail) and a `RecordingNotifier`, which are only possible because `RentalService` depends on interfaces.

## Sample output (abridged)

```
=== 2. Customers rent using different payment methods ===
Quote for Arun: Honda City for 3 day(s) = Rs. 6000.00 [Standard pricing]
Paid Rs. 6000.00 via UPI. Honda City rented to Arun.
Quote for Priya: Royal Enfield for 2 day(s) = Rs. 1600.00 [Standard pricing]
Paid Rs. 1600.00 via Card. Royal Enfield rented to Priya.
Quote for Kavya: Tata Ace for 1 day(s) = Rs. 2000.00 [Standard pricing]
Paid Rs. 2000.00 via Cash. Tata Ace rented to Kavya.

=== 3. Invalid requests are rejected ===
Rental rejected: Honda City is already rented.
Rental rejected: vehicle ID 'X999' not found.
Rental rejected: number of days must be positive.
Return rejected: 'C101' was not rented by Priya.

=== 5. Extension: long-rental discount pricing ===
-- With StandardPricing --
Quote for Arun: Honda City for 6 day(s) = Rs. 12000.00 [Standard pricing]
-- With LongRentalDiscountPricing --
Quote for Arun: Honda City for 6 day(s) = Rs. 10200.00 [Long-rental discount (15% off for 5+ days)]
```

## Demonstrated extension

**New requirement:** rentals of 5 or more days get a 15% discount.

**What was added:** one new file, `include/pricing/LongRentalDiscountPricing.h`, implementing `IPricingPolicy`.

**What was not changed:** `RentalService`, `IPricingPolicy`, the vehicles, payments, `Customer` and `Admin`. Only `main.cpp` creates the new policy and passes it to a `RentalService`, and a test was added for it. Section 5 of the program output shows the same rental priced both ways.

## Contributors

| Name | Registration number | Contribution |
|------|--------------------|--------------|
| _Your name_ | _Reg. no._ | _Describe what you did_ |
