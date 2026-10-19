#pragma once
#include <string>
#include "payment/IPayment.h"

// Simulated cash payment at the counter.
class CashPayment : public IPayment {
public:
    bool pay(double /*amount*/) override { return true; }
    std::string getMethodName() const override { return "Cash"; }
};
