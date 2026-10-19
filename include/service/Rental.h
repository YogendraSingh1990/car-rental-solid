#pragma once
#include <string>

// Details of one active rental.
struct Rental {
    std::string customerName;
    std::string vehicleId;
    int days;
    double totalCost;
};
