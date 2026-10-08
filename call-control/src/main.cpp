#include "call_controller.hpp"

#include <iostream>

int main()
{
    try {

        CallController controller(
            9000,
            "127.0.0.1",
            9001,
            "127.0.0.1",
            9002
        );

        controller.run();
    }
    catch (const std::exception& e) {

        std::cerr
            << "[CONTROL] Fatal error: "
            << e.what()
            << '\n';

        return 1;
    }

    return 0;
}

