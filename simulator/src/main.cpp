#include "socket.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <sstream>
#include <vector>

int main()
{
    const std::string caller =
        "46701112233";

    const std::string callee =
        "46709998877";

    const int duration_seconds = 10;

    TcpSocket socket;

    if (!socket.connectTo(
            "127.0.0.1",
            9000)) {

        std::cerr
            << "Cannot connect to call controller\n";

        return 1;
    }

    std::cout
        << "====================================\n"
        << "       CLOUD VSE CALL SIMULATOR\n"
        << "====================================\n\n";

    std::cout
        << "Caller: "
        << caller
        << '\n';

    std::cout
        << "Callee: "
        << callee
        << "\n\n";

    socket.sendMessage(
        "MAKE_CALL|" +
        caller +
        "|" +
        callee
    );

    std::string response =
        socket.receiveMessage();

    std::cout
        << "Controller response: "
        << response
        << "\n\n";

    auto fields = std::vector<std::string>();

    {
        std::stringstream ss(response);
        std::string item;

        while (std::getline(ss, item, '|')) {
            fields.push_back(item);
        }
    }

    if (fields.empty() ||
        fields[0] != "CALL_ACTIVE") {

        std::cerr
            << "Call setup failed\n";

        return 1;
    }

    const std::string call_id =
        fields[1];

    std::cout
        << "Call ID: "
        << call_id
        << '\n';

    std::cout
        << "Call is ACTIVE\n";

    std::cout
        << "Waiting "
        << duration_seconds
        << " seconds...\n\n";

    std::this_thread::sleep_for(
        std::chrono::seconds(
            duration_seconds
        )
    );

    TcpSocket release_socket;

    if (!release_socket.connectTo(
            "127.0.0.1",
            9000)) {

        std::cerr
            << "Cannot reconnect to controller\n";

        return 1;
    }

    release_socket.sendMessage(
        "RELEASE_CALL|" + call_id
    );

    response =
        release_socket.receiveMessage();

    std::cout
        << "Release response: "
        << response
        << '\n';

    std::cout
        << "\nCALL COMPLETE\n";

    return 0;
}
