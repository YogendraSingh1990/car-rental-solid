# Car Rental System – SOLID Principles

BCSE102L – Structured and Object-Oriented Programming, SOLID case study (Part B).

## Problem

A campus car rental desk lends cars, bikes and trucks to customers. Requirements are expected to grow: new vehicle types, new payment methods, new pricing rules and new user roles. The goal is a design where these can be added with minimal changes to existing code, demonstrated through all five SOLID principles.

## Features

- Interactive console menu driven by user input, plus a scripted `--demo` mode.
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
./rental_app            # interactive menu
./rental_app --demo     # scripted demo
```

**Windows (MinGW g++)**
```
g++ -std=c++17 -Iinclude src\RentalService.cpp src\main.cpp -o rental_app.exe
.\rental_app.exe
.\rental_app.exe --demo
```

## Using the interactive menu

On start, choose a pricing policy (standard, or the long-rental discount extension). A sample fleet is loaded, then this menu appears:

```
----------- MENU -----------
1. Show available vehicles
2. Rent a vehicle
3. Return a vehicle
4. Admin: add a vehicle
5. Run the scripted demo
0. Exit
Enter choice:
```

Example session (user input after each prompt):

```
Enter choice: 2
Your name: Ravi
Vehicle ID to rent: C101
Number of days: 6
Payment method:  1. UPI   2. Card   3. Cash
Choose payment (1-3): 1
Quote for Ravi: Honda City for 6 day(s) = Rs. 10200.00 [Long-rental discount (15% off for 5+ days)]
Paid Rs. 10200.00 via UPI. Honda City rented to Ravi.
```

Invalid menu input is rejected and asked again.

## Run the tests

```
g++ -std=c++17 -Iinclude tests/test_main.cpp src/RentalService.cpp -o rental_tests
./rental_tests
```

Expected last line: `Passed: 55  Failed: 0`. The tests use a `FakePayment` (which can be made to fail) and a `RecordingNotifier`, which are only possible because `RentalService` depends on interfaces.

## Sample output of `--demo` (abridged)

```
=== 1. Admin adds vehicles ===
Car Honda City (C101) added to the fleet.
Bike Royal Enfield (B101) added to the fleet.
Truck Tata Ace (T101) added to the fleet.
Cannot add Duplicate Car: ID 'C101' is already in use.
Tata Ace can carry 750 kg of cargo.
Available vehicles:
  [Car] C101 | Honda City | Rs. 2000.00/day
  [Bike] B101 | Royal Enfield | Rs. 800.00/day
  [Truck] T101 | Tata Ace | Rs. 2000.00/day

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

## Contributors

| Name | Registration number | Contribution |
|------|--------------------|--------------|
| Yogendra Singh | 23BCE2361 | Sole contributor. Chose the car rental problem, designed the initial class structure (IVehicle, IPayment, RentalService, Customer, Admin) and wrote the first working single-file version. Set up the GitHub repository and build environment, built and ran the program and tests, and reviewed the final design. |

**Acknowledgement:** The refactoring into the final multi-file SOLID design (repository, pricing policies, notifier, role interfaces, Truck/ICargoCarrier), the test suite, the long-rental discount extension and the interactive console menu were developed with assistance from Claude (an AI assistant by Anthropic), and reviewed and run by the contributor.
