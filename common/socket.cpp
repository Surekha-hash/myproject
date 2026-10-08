#include "socket.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

TcpSocket::TcpSocket()
    : fd_(-1)
{
}

TcpSocket::TcpSocket(int fd)
    : fd_(fd)
{
}

TcpSocket::~TcpSocket()
{
    if (fd_ != -1) {
        close(fd_);
    }
}

TcpSocket::TcpSocket(TcpSocket&& other) noexcept
    : fd_(other.fd_)
{
    other.fd_ = -1;
}

TcpSocket& TcpSocket::operator=(TcpSocket&& other) noexcept
{
    if (this != &other) {
        if (fd_ != -1) {
            close(fd_);
        }

        fd_ = other.fd_;
        other.fd_ = -1;
    }

    return *this;
}

bool TcpSocket::connectTo(const std::string& host, int port)
{
    fd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (fd_ == -1) {
        perror("socket");
        return false;
    }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(),
                  &server_address.sin_addr) <= 0) {
        std::cerr << "Invalid address: " << host << '\n';
        return false;
    }

    if (connect(fd_,
                reinterpret_cast<sockaddr*>(&server_address),
                sizeof(server_address)) < 0) {
        perror("connect");
        return false;
    }

    return true;
}

bool TcpSocket::listenOn(int port)
{
    fd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (fd_ == -1) {
        perror("socket");
        return false;
    }

    int option = 1;

    setsockopt(
        fd_,
        SOL_SOCKET,
        SO_REUSEADDR,
        &option,
        sizeof(option)
    );

    sockaddr_in server_address{};

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    if (bind(
            fd_,
            reinterpret_cast<sockaddr*>(&server_address),
            sizeof(server_address)) < 0) {
        perror("bind");
        return false;
    }

    if (listen(fd_, 20) < 0) {
        perror("listen");
        return false;
    }

    return true;
}

TcpSocket TcpSocket::acceptConnection()
{
    int client_fd = accept(fd_, nullptr, nullptr);

    if (client_fd < 0) {
        perror("accept");
        return TcpSocket();
    }

    return TcpSocket(client_fd);
}

bool TcpSocket::sendMessage(const std::string& message)
{
    std::string data = message + "\n";

    ssize_t bytes_sent =
        send(fd_, data.c_str(), data.size(), 0);

    return bytes_sent ==
           static_cast<ssize_t>(data.size());
}

std::string TcpSocket::receiveMessage()
{
    std::string result;
    char buffer[1024];

    while (true) {
        ssize_t bytes_received =
            recv(fd_, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received <= 0) {
            break;
        }

        buffer[bytes_received] = '\0';

        result += buffer;

        if (result.find('\n') != std::string::npos) {
            break;
        }
    }

    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }

    return result;
}

bool TcpSocket::isValid() const
{
    return fd_ != -1;
}
//This is deliberately simple. Later we'll improve framing, timeouts, partial sends, connection pooling, and eventually epoll.
