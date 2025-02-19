#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <asio.hpp>
#include <asio/ssl.hpp>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

class Server
{
public:
    string port;
    asio::io_context io_context;
    asio::ssl::context ssl_context;

    Server(string port);

    int start() {
        // Initialize Winsock 
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0) {
            cerr << "WSAStartup failed: " << result << endl;
            return -1;
        }

        // Creating the server socket
        int server_socket_fd;
        struct sockaddr_in server_addr;
        server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

        if(server_socket_fd < 0) {
            cerr << "Failed to create server socket" << endl;
            WSACleanup();
            return -1;
        }

        // Configuring the server socket
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(stoi(port));
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if(bind(server_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            cerr << "Failed to bind server socket" << endl;
            closesocket(server_socket_fd);
            WSACleanup();
            return -1;
        }

        if(listen(server_socket_fd, 5) < 0) {
            cerr << "Failed to listen on server socket" << endl;
            closesocket(server_socket_fd);
            WSACleanup();
            return -1;
        }

        cerr << "Server started on port : " << port << endl;
        return server_socket_fd;
    }

    int start_udp() {
        // Initialize Winsock 
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0) {
            cerr << "WSAStartup failed: " << result << endl;
            return -1;
        }

        // Create a UDP socket
        int udp_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (udp_socket_fd < 0) {
            cerr << "Failed to create UDP socket" << endl;
            WSACleanup();
            return -1;
        }

        // Bind the UDP socket to the same port as the TCP server
        struct sockaddr_in udp_server_addr;
        udp_server_addr.sin_family = AF_INET;
        udp_server_addr.sin_port = htons(stoi(port));
        udp_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(udp_socket_fd, (struct sockaddr*)&udp_server_addr, sizeof(udp_server_addr)) < 0) {
            cerr << "Failed to bind UDP socket" << endl;
            closesocket(udp_socket_fd);
            WSACleanup();
            return -1;
        }

        cout << "UDP server started on port : " << port << endl;
        return udp_socket_fd;
    }

    asio::ip::tcp::acceptor start_tls() {
        try {
            // Create TCP acceptor
            asio::ip::tcp::acceptor acceptor(this->io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), stoi(port)));
            return acceptor;

        } catch (std::exception& e) {
            cerr << "Exception: " << e.what() << endl;
            throw;
        }
    }


    ~Server();
};

Server::Server(string port)
    : port(port), ssl_context(asio::ssl::context::tlsv12_server)
{
    ssl_context.set_options(
        asio::ssl::context::default_workarounds |
        asio::ssl::context::no_sslv2 |
        asio::ssl::context::single_dh_use);
    ssl_context.use_certificate_chain_file("server_chain.crt");
    ssl_context.use_private_key_file("server.key", asio::ssl::context::pem);
}

Server::~Server()
{
    WSACleanup();
}

// int main() {
//     cout << "Start server test" << endl;
//     Server server("5000");
//     server.start();
//     return 0;
// }
