#include <iostream>
#include "input_reader.h"
#include "stat_reader.h"

int main() {
    TransportCatalogue transport_catalogue;
    {
        InputReader reader;
        reader.ReadBaseInput(std::cin, transport_catalogue);
        reader.ReadStatInput(std::cin, std::cout, transport_catalogue);
    }
}
