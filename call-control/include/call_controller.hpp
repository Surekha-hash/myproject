#pragma once

#include "socket.hpp"

#include <string>

class CallController {
public:
    CallController(
        int port,
        std::string network_host,
        int network_port,
        std::string billing_host,
        int billing_port);

    void run();

private:
    void handleClient(TcpSocket client);

    std::string generateCallId();

    bool setupNetworkCall(
        const std::string& call_id,
        const std::string& caller,
        const std::string& callee);

    bool startBilling(
        const std::string& call_id);

    bool stopBilling(
        const std::string& call_id);

    bool releaseNetworkCall(
        const std::string& call_id);

    int port_;

    std::string network_host_;
    int network_port_;

    std::string billing_host_;
    int billing_port_;

    unsigned long call_counter_;
};
