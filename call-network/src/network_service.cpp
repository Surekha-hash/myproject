#include "network_service.hpp"
#include "protocol.hpp"

#include <iostream>
#include <thread>

NetworkService::NetworkService(int port)
    : port_(port)
{
}

void NetworkService::run()
{
    TcpSocket server;

    if (!server.listenOn(port_)) {
        throw std::runtime_error(
            "Failed to start network service"
        );
    }

    std::cout
        << "[NETWORK] Listening on port "
        << port_
        << '\n';

    while (true) {
        TcpSocket client = server.acceptConnection();

        if (!client.isValid()) {
            continue;
        }

        std::thread(
            &NetworkService::handleClient,
            this,
            std::move(client)
        ).detach();
    }
}

void NetworkService::handleClient(TcpSocket client)
{
    std::string request = client.receiveMessage();

    std::cout
        << "[NETWORK] Request: "
        << request
        << '\n';

    auto fields = vse::split(request, '|');

    if (fields.empty()) {
        client.sendMessage("ERROR|INVALID_REQUEST");
        return;
    }

    if (fields[0] == "CALL_SETUP") {

        if (fields.size() < 4) {
            client.sendMessage(
                "CALL_REJECTED|INVALID_FIELDS"
            );
            return;
        }

        const std::string& call_id = fields[1];
        const std::string& caller = fields[2];
        const std::string& callee = fields[3];

        std::cout
            << "[NETWORK] Setting up call "
            << call_id
            << " caller="
            << caller
            << " callee="
            << callee
            << '\n';

        client.sendMessage(
            "CALL_ACCEPTED|" + call_id
        );

        std::cout
            << "[NETWORK] Call accepted: "
            << call_id
            << '\n';

        return;
    }

    if (fields[0] == "CALL_RELEASE") {

        if (fields.size() < 2) {
            client.sendMessage(
                "ERROR|INVALID_RELEASE"
            );
            return;
        }

        const std::string& call_id = fields[1];

        std::cout
            << "[NETWORK] Releasing call "
            << call_id
            << '\n';

        client.sendMessage(
            "CALL_RELEASED|" + call_id
        );

        return;
    }

    client.sendMessage(
        "ERROR|UNKNOWN_MESSAGE"
    );
}
