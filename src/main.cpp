#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <iomanip>
using namespace std;

// ==========================================
// 1. ISP: Small, focused vehicle interface
// ==========================================
class IVehicle {
public:
    virtual ~IVehicle() = default;

    virtual string getId() const = 0;
    virtual string getName() const = 0;
    virtual double getDailyRate() const = 0;
    virtual bool isAvailable() const = 0;
    virtual void setAvailable(bool available) = 0;
};

// ==========================================
// Vehicle implementations: OCP and LSP
// ==========================================
class Car : public IVehicle {
private:
    string id, name;
    double rate;
    bool available = true;

public:
    Car(string id, string name, double rate)
        : id(id), name(name), rate(rate) {}

    string getId() const override { return id; }
    string getName() const override { return name; }
    double getDailyRate() const override { return rate; }
    bool isAvailable() const override { return available; }
    void setAvailable(bool value) override {
        available = value;
    }
};

class Bike : public IVehicle {
private:
    string id, name;
    double rate;
    bool available = true;

public:
    Bike(string id, string name, double rate)
        : id(id), name(name), rate(rate) {}

    string getId() const override { return id; }
    string getName() const override { return name; }
    double getDailyRate() const override { return rate; }
    bool isAvailable() const override { return available; }
    void setAvailable(bool value) override {
        available = value;
    }
};

// ==========================================
// ISP: Separate payment abstraction
// ==========================================
class IPayment {
public:
    virtual ~IPayment() = default;
    virtual bool pay(double amount) = 0;
};

class UpiPayment : public IPayment {
public:
    bool pay(double amount) override {
        cout << "Paid Rs. " << fixed << setprecision(2)
             << amount << " using UPI.\n";
        return true;
    }
};

class CardPayment : public IPayment {
public:
    bool pay(double amount) override {
        cout << "Paid Rs. " << fixed << setprecision(2)
             << amount << " using Card.\n";
        return true;
    }
};

// ==========================================
// Rental record
// ==========================================
struct Rental {
    string customerName;
    string vehicleId;
    int days;
    double totalCost;
};

// ==========================================
// SRP + DIP: RentalService coordinates rentals
// It depends on interfaces, not concrete vehicles
// or concrete payment implementations.
// ==========================================
class RentalService {
private:
    vector<shared_ptr<IVehicle>> vehicles;
    shared_ptr<IPayment> payment;
    unordered_map<string, Rental> activeRentals;

public:
    explicit RentalService(shared_ptr<IPayment> payment)
        : payment(payment) {}

    void addVehicle(shared_ptr<IVehicle> vehicle) {
        vehicles.push_back(vehicle);
        cout << vehicle->getName() << " added successfully.\n";
    }

    void showAvailableVehicles() const {
        cout << "\n--- Available Vehicles ---\n";

        bool found = false;

        for (const auto& vehicle : vehicles) {
            if (vehicle->isAvailable()) {
                cout << "ID: " << vehicle->getId()
                     << " | Name: " << vehicle->getName()
                     << " | Daily rate: Rs. "
                     << vehicle->getDailyRate() << '\n';
                found = true;
            }
        }

        if (!found)
            cout << "No vehicles available.\n";
    }

    bool rentVehicle(const string& customer,
                     const string& vehicleId, int days) {
        if (days <= 0) {
            cout << "Rental days must be positive.\n";
            return false;
        }

        if (activeRentals.count(vehicleId)) {
            cout << "This vehicle is already rented.\n";
            return false;
        }

        for (const auto& vehicle : vehicles) {
            if (vehicle->getId() == vehicleId) {
                if (!vehicle->isAvailable()) {
                    cout << "Vehicle is unavailable.\n";
                    return false;
                }

                double cost = vehicle->getDailyRate() * days;

                cout << "\nRental cost: Rs. " << fixed
                     << setprecision(2) << cost << '\n';

                // Mark the vehicle as rented only after
                // payment succeeds.
                if (!payment->pay(cost)) {
                    cout << "Payment failed. Rental cancelled.\n";
                    return false;
                }

                vehicle->setAvailable(false);

                activeRentals[vehicleId] = {
                    customer, vehicleId, days, cost
                };

                cout << "Vehicle rented successfully to "
                     << customer << ".\n";
                return true;
            }
        }

        cout << "Vehicle ID not found.\n";
        return false;
    }

    bool returnVehicle(const string& vehicleId) {
        auto rental = activeRentals.find(vehicleId);

        if (rental == activeRentals.end()) {
            cout << "No active rental found for this vehicle.\n";
            return false;
        }

        for (const auto& vehicle : vehicles) {
            if (vehicle->getId() == vehicleId) {
                vehicle->setAvailable(true);
                cout << vehicle->getName()
                     << " returned successfully by "
                     << rental->second.customerName << ".\n";

                activeRentals.erase(rental);
                return true;
            }
        }

        return false;
    }
};

// ==========================================
// SRP: Customer represents the customer role
// ==========================================
class Customer {
private:
    string name;

public:
    explicit Customer(string name) : name(name) {}

    void rent(RentalService& service,
              const string& vehicleId, int days) {
        service.rentVehicle(name, vehicleId, days);
    }

    void returnVehicle(RentalService& service,
                       const string& vehicleId) {
        service.returnVehicle(vehicleId);
    }
};

// ==========================================
// SRP: Admin handles vehicle administration
// ==========================================
class Admin {
public:
    void addCar(RentalService& service, const string& id,
                const string& name, double rate) {
        service.addVehicle(
            make_shared<Car>(id, name, rate));
    }

    void addBike(RentalService& service, const string& id,
                 const string& name, double rate) {
        service.addVehicle(
            make_shared<Bike>(id, name, rate));
    }
};

// ==========================================
// Main program
// ==========================================
int main() {
    auto payment = make_shared<UpiPayment>();
    RentalService service(payment);

    Admin admin;
    Customer customer("Arun");

    admin.addCar(service, "C101", "Honda City", 2000);
    admin.addBike(service, "B101", "Royal Enfield", 800);

    service.showAvailableVehicles();

    cout << "\n--- Customer rents a vehicle ---\n";
    customer.rent(service, "C101", 3);

    service.showAvailableVehicles();

    cout << "\n--- Customer returns the vehicle ---\n";
    customer.returnVehicle(service, "C101");

    service.showAvailableVehicles();

    return 0;
}