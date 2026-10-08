#include "network_service.hpp"

#include <iostream>

int main()
{
    try {
        NetworkService service(9001);
        service.run();
    }
    catch (const std::exception& e) {
        std::cerr
            << "[NETWORK] Fatal error: "
            << e.what()
            << '\n';

        return 1;
    }

    return 0;
}
