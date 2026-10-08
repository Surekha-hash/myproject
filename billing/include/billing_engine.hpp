#pragma once

#include "socket.hpp"

#include <chrono>
#include <string>
#include <unordered_map>
#include <mutex>

class BillingEngine {
public:
    explicit BillingEngine(int port);

    void run();

private:
    void handleClient(TcpSocket client);

    void startBilling(
        const std::string& call_id,
        TcpSocket& client);

    void stopBilling(
        const std::string& call_id,
        TcpSocket& client);

    int port_;

    std::unordered_map<
        std::string,
        std::chrono::steady_clock::time_point
    > active_calls_;

    std::mutex mutex_;

    static constexpr double RATE_PER_MINUTE = 0.02;
};
