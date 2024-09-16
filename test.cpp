#include "client.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h> // For close()
#include <stdexcept> // For std::invalid_argument
#include <netinet/in.h> // For sockaddr_in
#include <arpa/inet.h> // For inet_pton

// Function to read and print server responses
void read_server_response(int fd) {
    char buffer[256];
    ssize_t bytesRead;

    while ((bytesRead = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0'; // Null-terminate the buffer
        std::cout << "Server Response: " << buffer;
    }

    if (bytesRead < 0) {
        std::cerr << "Error: unable to read from server" << std::endl;
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Error: usage --> ./client <server_ip> <port>" << std::endl;
        return 1;
    }

    std::string server_ip(argv[1]);
    unsigned int server_port;

    try {
        server_port = std::stoi(argv[2]);
        std::cout << server_ip;
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: invalid port number" << std::endl;
        return 1;
    } catch (const std::out_of_range& e) {
        std::cerr << "Error: port number out of range" << std::endl;
        return 1;
    }

    // Create a client
    client myClient;

    // Set client details
    myClient.set_adress_ip(server_ip);
    myClient.set_port(server_port);
    myClient.set_password("your_password"); // Set this to match the server's password

    // Create socket and connect to the server
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error: unable to create socket" << std::endl;
        return 1;
    }

    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cerr << "Error: invalid address or address not supported" << std::endl;
        close(sockfd);
        return 1;
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Error: unable to connect to server" << std::endl;
        close(sockfd);
        return 1;
    }

    myClient.set_fd(sockfd);

    // Send NICK and USER commands
    myClient.nick(sockfd, "/nick Alice");
    myClient.user(sockfd, "/user aliceuser localhost localhost Alice Wonderland");

    // Send JOIN command with a key
    myClient.join(sockfd, "/join #mychannel mykey");

    // Send a PRIVMSG
    myClient.privmsg(sockfd, "/privmsg #mychannel :Hello everyone!");

    // Read and print server responses
    read_server_response(sockfd);

    // Send QUIT command
    myClient.quit(sockfd, "/quit :Goodbye!");

    // Clean up
    close(sockfd);
    return 0;
}
