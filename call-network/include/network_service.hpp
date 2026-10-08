#pragma once

#include "socket.hpp"

#include <string>

class NetworkService {
public:
    explicit NetworkService(int port);

    void run();

private:
    void handleClient(TcpSocket client);

    int port_;
};
