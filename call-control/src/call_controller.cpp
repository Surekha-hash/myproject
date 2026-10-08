#include "call_controller.hpp"
#include "protocol.hpp"
#include <thread>
#include <iostream>
#include <sstream>

CallController::CallController(
    int port,
    std::string network_host,
    int network_port,
    std::string billing_host,
    int billing_port)
    : port_(port),
      network_host_(std::move(network_host)),
      network_port_(network_port),
      billing_host_(std::move(billing_host)),
      billing_port_(billing_port),
      call_counter_(1000)
{
}

void CallController::run()
{
    TcpSocket server;

    if (!server.listenOn(port_)) {
        throw std::runtime_error(
            "Failed to start call controller"
        );
    }

    std::cout
        << "[CONTROL] Listening on port "
        << port_
        << '\n';

    while (true) {

        TcpSocket client =
            server.acceptConnection();

        if (!client.isValid()) {
            continue;
        }

        std::thread(
            &CallController::handleClient,
            this,
            std::move(client)
        ).detach();
    }
}

void CallController::handleClient(
    TcpSocket client)
{
    std::string request =
        client.receiveMessage();

    std::cout
        << "[CONTROL] Request: "
        << request
        << '\n';

    auto fields =
        vse::split(request, '|');

    if (fields.empty()) {
        client.sendMessage(
            "ERROR|INVALID_REQUEST"
        );

        return;
    }

    if (fields[0] == "MAKE_CALL") {

        if (fields.size() < 3) {
            client.sendMessage(
                "ERROR|INVALID_MAKE_CALL"
            );

            return;
        }

        const std::string& caller =
            fields[1];

        const std::string& callee =
            fields[2];

        std::string call_id =
            generateCallId();

        std::cout
            << "[CONTROL] Creating "
            << call_id
            << '\n';

        if (!setupNetworkCall(
                call_id,
                caller,
                callee)) {

            client.sendMessage(
                "CALL_FAILED|" +
                call_id +
                "|NETWORK"
            );

            return;
        }

        if (!startBilling(call_id)) {

            releaseNetworkCall(call_id);

            client.sendMessage(
                "CALL_FAILED|" +
                call_id +
                "|BILLING"
            );

            return;
        }

        client.sendMessage(
            "CALL_ACTIVE|" + call_id
        );

        return;
    }

    if (fields[0] == "RELEASE_CALL") {

        if (fields.size() < 2) {
            client.sendMessage(
                "ERROR|INVALID_RELEASE"
            );

            return;
        }

        const std::string& call_id =
            fields[1];

        stopBilling(call_id);

        releaseNetworkCall(call_id);

        client.sendMessage(
            "CALL_RELEASED|" + call_id
        );

        return;
    }

    client.sendMessage(
        "ERROR|UNKNOWN_COMMAND"
    );
}

std::string CallController::generateCallId()
{
    std::ostringstream id;

    id << "CALL-" << ++call_counter_;

    return id.str();
}

bool CallController::setupNetworkCall(
    const std::string& call_id,
    const std::string& caller,
    const std::string& callee)
{
    TcpSocket socket;

    if (!socket.connectTo(
            network_host_,
            network_port_)) {

        std::cerr
            << "[CONTROL] Cannot connect to network service\n";

        return false;
    }

    std::string request =
        "CALL_SETUP|" +
        call_id +
        "|" +
        caller +
        "|" +
        callee;

    socket.sendMessage(request);

    std::string response =
        socket.receiveMessage();

    std::cout
        << "[CONTROL] Network response: "
        << response
        << '\n';

    return response ==
           "CALL_ACCEPTED|" + call_id;
}

bool CallController::startBilling(
    const std::string& call_id)
{
    TcpSocket socket;

    if (!socket.connectTo(
            billing_host_,
            billing_port_)) {

        std::cerr
            << "[CONTROL] Cannot connect to billing service\n";

        return false;
    }

    socket.sendMessage(
        "BILLING_START|" + call_id
    );

    std::string response =
        socket.receiveMessage();

    std::cout
        << "[CONTROL] Billing response: "
        << response
        << '\n';

    return response ==
           "BILLING_STARTED|" + call_id;
}

bool CallController::stopBilling(
    const std::string& call_id)
{
    TcpSocket socket;

    if (!socket.connectTo(
            billing_host_,
            billing_port_)) {

        return false;
    }

    socket.sendMessage(
        "BILLING_STOP|" + call_id
    );

    std::string response =
        socket.receiveMessage();

    std::cout
        << "[CONTROL] Billing result: "
        << response
        << '\n';

    return true;
}

bool CallController::releaseNetworkCall(
    const std::string& call_id)
{
    TcpSocket socket;

    if (!socket.connectTo(
            network_host_,
            network_port_)) {

        return false;
    }

    socket.sendMessage(
        "CALL_RELEASE|" + call_id
    );

    std::string response =
        socket.receiveMessage();

    std::cout
        << "[CONTROL] Network release: "
        << response
        << '\n';

    return true;
}
