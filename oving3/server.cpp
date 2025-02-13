#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

class Server
{
public:
    string port;
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
            exit(1);
        }

        // Configuring the server socket
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(stoi(port));
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if(bind(server_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            cerr << "Failed to bind server socket" << endl;
            exit(1);
        }

        if(listen(server_socket_fd, 5) < 0) {
            cerr << "Failed to listen on server socket" << endl;
            exit(1);
        }

        cerr << "Server started on port : " << port << endl;
        return server_socket_fd;
    }


    ~Server();
};

Server::Server(string port)
{
    this->port = port;
}

Server::~Server()
{
}

// int main() {
//     cout << "Start server test" << endl;
//     Server server("5000");
//     server.start();
//     return 0;
// }
