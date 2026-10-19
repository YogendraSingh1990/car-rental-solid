#pragma once
#include <string>

// SRP: sending messages to the user is separate from rental logic.
class INotifier {
public:
    virtual ~INotifier() = default;
    virtual void notify(const std::string& message) = 0;
};
