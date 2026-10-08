#include "billing_engine.hpp"
#include "protocol.hpp"

#include <iostream>
#include <iomanip>
#include <thread>

BillingEngine::BillingEngine(int port)
    : port_(port)
{
}

void BillingEngine::run()
{
    TcpSocket server;

    if (!server.listenOn(port_)) {
        throw std::runtime_error(
            "Failed to start billing service"
        );
    }

    std::cout
        << "[BILLING] Listening on port "
        << port_
        << '\n';

    while (true) {

        TcpSocket client =
            server.acceptConnection();

        if (!client.isValid()) {
            continue;
        }

        std::thread(
            &BillingEngine::handleClient,
            this,
            std::move(client)
        ).detach();
    }
}

void BillingEngine::handleClient(TcpSocket client)
{
    std::string request =
        client.receiveMessage();

    std::cout
        << "[BILLING] Request: "
        << request
        << '\n';

    auto fields =
        vse::split(request, '|');

    if (fields.size() < 2) {
        client.sendMessage(
            "ERROR|INVALID_REQUEST"
        );

        return;
    }

    const std::string& command =
        fields[0];

    const std::string& call_id =
        fields[1];

    if (command == "BILLING_START") {
        startBilling(call_id, client);
        return;
    }

    if (command == "BILLING_STOP") {
        stopBilling(call_id, client);
        return;
    }

    client.sendMessage(
        "ERROR|UNKNOWN_BILLING_COMMAND"
    );
}

void BillingEngine::startBilling(
    const std::string& call_id,
    TcpSocket& client)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);

        active_calls_[call_id] =
            std::chrono::steady_clock::now();
    }

    std::cout
        << "[BILLING] Started billing for "
        << call_id
        << '\n';

    client.sendMessage(
        "BILLING_STARTED|" + call_id
    );
}

void BillingEngine::stopBilling(
    const std::string& call_id,
    TcpSocket& client)
{
    std::chrono::steady_clock::time_point start;

    {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it =
            active_calls_.find(call_id);

        if (it == active_calls_.end()) {
            client.sendMessage(
                "ERROR|CALL_NOT_FOUND"
            );

            return;
        }

        start = it->second;

        active_calls_.erase(it);
    }

    auto end =
        std::chrono::steady_clock::now();

    auto duration =
        std::chrono::duration_cast<
            std::chrono::seconds
        >(end - start);

    long seconds =
        duration.count();

    double minutes =
        static_cast<double>(seconds) / 60.0;

    double cost =
        minutes * RATE_PER_MINUTE;

    std::cout
        << "[BILLING] Call "
        << call_id
        << " duration="
        << seconds
        << "s cost=$"
        << std::fixed
        << std::setprecision(2)
        << cost
        << '\n';

    std::ostringstream response;

    response
        << "BILLING_RESULT|"
        << call_id
        << "|"
        << seconds
        << "|"
        << std::fixed
        << std::setprecision(2)
        << cost;

    client.sendMessage(
        response.str()
    );
}
