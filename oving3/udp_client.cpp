#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

void sendUdpPacket(const string& server_ip, int server_port, const string& message) {
    // Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cerr << "WSAStartup failed: " << result << endl;
        return;
    }

    // Create a UDP socket
    int udp_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket_fd == INVALID_SOCKET) {
        cerr << "Failed to create UDP socket: " << WSAGetLastError() << endl;
        WSACleanup();
        return;
    }

    // Set up the server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr);

    // Send the UDP packet
    int bytes_sent = sendto(udp_socket_fd, message.c_str(), message.length(), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (bytes_sent == SOCKET_ERROR) {
        cerr << "Failed to send UDP packet: " << WSAGetLastError() << endl;
        closesocket(udp_socket_fd);
        WSACleanup();
        return;
    }

    cout << "Sent UDP packet: " << message << endl;

    // Wait for the response from the server
    char response_buffer[1024];
    struct sockaddr_in from_addr;
    int from_len = sizeof(from_addr);
    int bytes_received = recvfrom(udp_socket_fd, response_buffer, sizeof(response_buffer) - 1, 0, (struct sockaddr*)&from_addr, &from_len);
    if (bytes_received == SOCKET_ERROR) {
        cerr << "Failed to receive UDP response: " << WSAGetLastError() << endl;
        closesocket(udp_socket_fd);
        WSACleanup();
        return;
    }

    response_buffer[bytes_received] = '\0';
    cout << "Received UDP response from server: " << response_buffer << endl;


    // Close the socket
    closesocket(udp_socket_fd);
    WSACleanup();
}

int main() {
    string server_ip = "127.0.0.1";
    int server_port = 8080; 
    string message = "[1,1] [3,3]";

    if( message.size() > 1024) {
        cerr << "Message exceeds 1024 bytes" << endl;
        return 1;
    }

    sendUdpPacket(server_ip, server_port, message);

    return 0;
}