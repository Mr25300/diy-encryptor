#pragma once

#include <cstddef>

class PRF {
public:
    virtual void compute() const = 0;
    virtual std::size_t outputSize() const = 0;
};
