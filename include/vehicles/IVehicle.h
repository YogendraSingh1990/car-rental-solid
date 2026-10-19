#pragma once
#include <string>

// Abstraction every vehicle must honour.
// Contract (LSP):
//  - getDailyRate() always returns a positive amount
//  - setAvailable(false) marks the vehicle as rented,
//    setAvailable(true) marks it as free again
//  - no method throws or returns a dummy value
class IVehicle {
public:
    virtual ~IVehicle() = default;

    virtual std::string getId() const = 0;
    virtual std::string getName() const = 0;
    virtual std::string getType() const = 0;
    virtual double getDailyRate() const = 0;
    virtual bool isAvailable() const = 0;
    virtual void setAvailable(bool available) = 0;
};
