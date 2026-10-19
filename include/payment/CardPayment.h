#pragma once
#include <string>
#include "payment/IPayment.h"

// Simulated card payment.
class CardPayment : public IPayment {
public:
    bool pay(double /*amount*/) override { return true; }
    std::string getMethodName() const override { return "Card"; }
};
