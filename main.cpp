#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

class Client {
public:
    Client(const std::string &server_ip, int server_port);
    ~Client();

    bool connect();
    bool send_request(const std::string &request);
    std::string receive_response();
    void disconnect();

private:
    std::string server_ip;
    int server_port;
    int sockfd;
    struct sockaddr_in server_addr;

    bool is_connected;
};

Client::Client(const std::string &server_ip, int server_port)
    : server_ip(server_ip), server_port(server_port), sockfd(-1), is_connected(false) {
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr);
}

Client::~Client() {
    if (is_connected) {
        disconnect();
    }
}

bool Client::connect() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return false;
    }

    if (::connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sockfd);
        return false;
    }

    is_connected = true;
    return true;
}

bool Client::send_request(const std::string &request) {
    if (!is_connected) {
        std::cerr << "Client not connected to server." << std::endl;
        return false;
    }

    if (send(sockfd, request.c_str(), request.size(), 0) < 0) {
        perror("send");
        return false;
    }

    return true;
}

std::string Client::receive_response() {
    if (!is_connected) {
        std::cerr << "Client not connected to server." << std::endl;
        return "";
    }

    char buffer[1024];
    int bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received < 0) {
        perror("recv");
        return "";
    }

    buffer[bytes_received] = '\0';
    return std::string(buffer);
}

void Client::disconnect() {
    if (is_connected) {
        close(sockfd);
        is_connected = false;
    }
}

int main() {
    std::string server_ip = "127.0.0.1";
    int server_port = 7777;
    std::string request = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    int num_clients = 10;
    std::vector<std::unique_ptr<Client>> clients;

    // Create and connect clients
    for (int i = 0; i < num_clients; ++i) {
        clients.emplace_back(std::make_unique<Client>(server_ip, server_port));
        if (clients.back()->connect()) {
            std::cout << "Client " << i + 1 << " connected to server." << std::endl;
        } else {
            std::cerr << "Failed to connect client " << i + 1 << std::endl;
        }
    }

    // Send requests and receive responses
    for (int i = 0; i < num_clients; ++i) {
        if (clients[i]->send_request(request)) {
            std::cout << "Client " << i + 1 << " sent request: " << request << std::endl;
            std::string response = clients[i]->receive_response();
            std::cout << "Client " << i + 1 << " received response: " << response << std::endl;
        }
    }

    // Disconnect clients
    for (int i = 0; i < num_clients; ++i) {
        clients[i]->disconnect();
        std::cout << "Client " << i + 1 << " disconnected from server." << std::endl;
    }

    return 0;
}
