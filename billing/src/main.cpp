#include "billing_engine.hpp"

#include <iostream>

int main()
{
    try {
        BillingEngine billing(9002);
        billing.run();
    }
    catch (const std::exception& e) {

        std::cerr
            << "[BILLING] Fatal error: "
            << e.what()
            << '\n';

        return 1;
    }

    return 0;
}
