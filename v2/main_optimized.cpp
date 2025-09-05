#include "optimized_game.hpp"
#include <iostream>

int main() {
    try {
        MulaWee::Game game;
        game.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
