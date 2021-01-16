#include <iostream>
#include "bluraydump.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "usage: " << argv[0] << " path\n";
        return EXIT_FAILURE;
    }

    BlurayDump blurayDump;
    auto bd_path = argv[1];
    if (!blurayDump.init(bd_path)) {
        std::cerr << "cannot open " << bd_path << '\n';
        return EXIT_FAILURE;
    }

    auto disc_info = blurayDump.dumpDiscInfo();
    std::cout << "\n============ dump disc info ============\n";
    for (const auto &item : disc_info) {
        std::cout << item.first << " : " << item.second << '\n';
    }

    auto meta = blurayDump.dumpMetadata();
    std::cout << "\n=========== dump metadata ============\n";
    for (const auto &item : meta) {
        std::cout << item.first << " : " << item.second << '\n';
    }
    return 0;
}
