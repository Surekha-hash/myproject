#include "call_controller.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

std::string getEnv(
    const char* name,
    const std::string& defaultValue)
{
    const char* value = std::getenv(name);

    if (value == nullptr) {
        return defaultValue;
    }

    return value;
}

int getEnvInt(
    const char* name,
    int defaultValue)
{
    const char* value = std::getenv(name);

    if (value == nullptr) {
        return defaultValue;
    }

    return std::stoi(value);
}

int main()
{
    try {

        int controlPort =
            getEnvInt("CONTROL_PORT", 9000);

        std::string networkHost =
            getEnv("NETWORK_HOST", "127.0.0.1");

        int networkPort =
            getEnvInt("NETWORK_PORT", 9001);

        std::string billingHost =
            getEnv("BILLING_HOST", "127.0.0.1");

        int billingPort =
            getEnvInt("BILLING_PORT", 9002);

        std::cout
            << "[CONTROL] Configuration\n"
            << "  Control port: "
            << controlPort
            << '\n'
            << "  Network: "
            << networkHost
            << ":"
            << networkPort
            << '\n'
            << "  Billing: "
            << billingHost
            << ":"
            << billingPort
            << '\n';

        CallController controller(
            controlPort,
            networkHost,
            networkPort,
            billingHost,
            billingPort
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
