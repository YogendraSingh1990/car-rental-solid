#pragma once
#include <string>

// Any payment method. Contract (LSP): pay() returns true if the
// amount was collected, false otherwise. It never throws.
class IPayment {
public:
    virtual ~IPayment() = default;
    virtual bool pay(double amount) = 0;
    virtual std::string getMethodName() const = 0;
};
