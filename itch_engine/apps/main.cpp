#include "parser.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <path_to_itch_binary>" << std::endl;
        return 1;
    }

    ItchParser engine;
    if (!engine.initialize(argv[1])) {
        return 1;
    }

    std::cout << "[+] Running high-performance stream loop..." << std::endl;
    engine.parse_stream();
    std::cout << "[+] Stream parsing complete." << std::endl;

    return 0;
}
