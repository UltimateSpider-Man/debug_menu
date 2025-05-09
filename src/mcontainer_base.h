#include <cstdint>
#include <cstring>
#include <iostream>

#include "variable.h"

struct mContainer_base {
    // Initializes a container (dummy implementation).
    void initialize(uint32_t* container, uint64_t flags) {
        // For example, clear the first few integers in the container.
        container[0] = 0;
        container[1] = 0;
        container[2] = 0;
        std::cout << "mContainer_base::initialize: Initialized container with flags "
            << flags << std::endl;
    }

    // Finalize any resources if needed (dummy implementation).
    void finalize() {
        std::cout << "mContainer_base::finalize: Finalizing container." << std::endl;
    }
};



Var<mContainer_base*> mContainer_base_ptr{ 0 };