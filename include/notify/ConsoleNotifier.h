#pragma once
#include <iostream>
#include <string>
#include "notify/INotifier.h"

// Prints messages to the console.
class ConsoleNotifier : public INotifier {
public:
    void notify(const std::string& message) override {
        std::cout << message << '\n';
    }
};
