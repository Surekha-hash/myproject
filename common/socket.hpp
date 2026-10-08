#pragma once

#include <string>

class TcpSocket {
public:
    TcpSocket();
    explicit TcpSocket(int fd);
    ~TcpSocket();

    TcpSocket(const TcpSocket&) = delete;
    TcpSocket& operator=(const TcpSocket&) = delete;

    TcpSocket(TcpSocket&& other) noexcept;
    TcpSocket& operator=(TcpSocket&& other) noexcept;

    bool connectTo(const std::string& host, int port);
    bool listenOn(int port);

    TcpSocket acceptConnection();

    bool sendMessage(const std::string& message);
    std::string receiveMessage();

    bool isValid() const;

private:
    int fd_;
};
