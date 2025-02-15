#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "dns.hpp"

int main() {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // Disable output buffering
    setbuf(stdout, NULL);

    // You can use print statements as follows for debugging, they'll be visible when running tests.
    std::cout << "Logs from your program will appear here!" << std::endl;

    // Uncomment this block to pass the first stage
    int udpSocket;
    struct sockaddr_in clientAddress;

    udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket == -1) {
        std::cerr << "Socket creation failed: " << strerror(errno) << "..." << std::endl;
        return 1;
    }

    // Since the tester restarts your program quite often, setting REUSE_PORT
    // ensures that we don't run into 'Address already in use' errors
    int reuse = 1;
    if (setsockopt(udpSocket, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
        std::cerr << "SO_REUSEPORT failed: " << strerror(errno) << std::endl;
        return 1;
    }

    sockaddr_in serv_addr = { .sin_family = AF_INET,
                              .sin_port = htons(2053),
                              .sin_addr = { htonl(INADDR_ANY) },
                            };

    if (bind(udpSocket, reinterpret_cast<struct sockaddr*>(&serv_addr), sizeof(serv_addr)) != 0) {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        return 1;
    }

    int bytesRead, bytesSend;
    char buffer[512];
    socklen_t clientAddrLen = sizeof(clientAddress);

    while (true) {
        // Receive data
        bytesRead = recvfrom(udpSocket, buffer, sizeof(buffer), 0, reinterpret_cast<struct sockaddr*>(&clientAddress), &clientAddrLen);
        if (bytesRead == -1) {
            perror("Error receiving data");
            break;
        }

        // process DNS requests
        net::dns::header req_header;
        memcpy(req_header.value, buffer, 12); // Copy raw bytes into the union
        printf("Transaction ID (host-byte order): %u\n", ntohs(req_header.fields.id));
        printf("Transaction ID (network-byte order): %u\n", req_header.fields.id);

        net::dns::header res_header{};
        res_header.fields.id = ntohs(res_header.fields.id);
        res_header.fields.flags.value = ntohs(res_header.fields.flags.value);
        res_header.fields.id = 1234;
        res_header.fields.flags.value = 0x8000;
        res_header.fields.flags.value = htons(res_header.fields.flags.value);
        res_header.fields.id = htons(res_header.fields.id);

        buffer[bytesRead] = '\0';
        std::cout << "Received " << bytesRead << " bytes" << std::endl;

        // Send response
        bytesSend = sendto(udpSocket, res_header.value, sizeof(net::dns::header), 0, reinterpret_cast<struct sockaddr*>(&clientAddress), sizeof(clientAddress));
        if (bytesSend == -1) {
          perror("Failed to send response");
        }
        std::cout << "Sent " << bytesSend << " bytes" << std::endl;
    }

    close(udpSocket);

    return 0;
}
