#pragma once
#include <string>
#include "payment/IPayment.h"

// Simulated UPI payment (a real one would call a payment gateway).
class UpiPayment : public IPayment {
public:
    bool pay(double /*amount*/) override { return true; }
    std::string getMethodName() const override { return "UPI"; }
};
