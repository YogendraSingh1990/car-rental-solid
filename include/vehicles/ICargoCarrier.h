#pragma once

// ISP: only vehicles that actually carry cargo implement this.
// Car and Bike are not forced to provide a meaningless
// getCargoCapacityKg() method.
class ICargoCarrier {
public:
    virtual ~ICargoCarrier() = default;
    virtual double getCargoCapacityKg() const = 0;
};
