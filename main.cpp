#include "vse_app.hpp"

// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>


int main() {
    vse::VseApp app{};
    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << "Application error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}