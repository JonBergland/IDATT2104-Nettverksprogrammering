#include"server.cpp"
#include <string.h>
#include <unistd.h>
#include"request_HTTP.cpp"
#include "request_HTTPS.cpp"
#include"response.cpp"
#include<thread>
#include <vector>
#include <sstream>
#include <numeric>
#include <asio.hpp>
#include <asio/ssl.hpp>

using namespace std;

//utility function to log the serving of a file.
void logServingFile(const string& path, const string& mimetype) {
    cout << "Serving file: " << path << " with MIME type: " << mimetype << endl;
}

void handleTCPClient(int client_socket_fd){
    cout << "Client socket: " << client_socket_fd << endl;
    char client_req_buffer[1024]; 
    //handle client request.
    //read from the client.
    int bytes_read = recv(client_socket_fd, client_req_buffer, sizeof(client_req_buffer) - 1, 0);
    if(bytes_read < 0){
        cerr << "Failed to read from client socket" << endl;
        closesocket(client_socket_fd);
        return;
    }
    client_req_buffer[bytes_read] = '\0';
    
    cout << "Received request : " << client_req_buffer << endl;

    if (bytes_read == 0) {
        cerr << "Received empty request" << endl;
        closesocket(client_socket_fd);
        return;
    }
    
    HttpRequest req = HttpRequest();
    req.parseRequest(client_req_buffer);

    if (req.method.empty() || req.path.empty()) {
        cerr << "Failed to parse request: method or path is empty" << endl;
        closesocket(client_socket_fd);
        return;
    }

    string mimetype = req.getMimeType(req.path);
    cout << req.path << endl;

    HttpResponse res= HttpResponse();
    
    string body = req.readHtmlFile(req.path);
    string response = res.frameHttpResponse("200","OK",req.headers,body,mimetype);
    logServingFile(req.path, mimetype);

    // Write to client.
    int bytes_sent = send(client_socket_fd, response.c_str(), response.length(), 0);
    if (bytes_sent < 0) {
        cerr << "Failed to send response to client" << endl;
    }
    //close client socket.
    closesocket(client_socket_fd);
}

int processUdpMessage(const string& message) {
    // Process the message expected to be two vectors in the format "[X,X] [Y,Y]"
    vector<int> vector1, vector2;
    stringstream ss(message);
    string segment;

    // Parse the first vector
    if (getline(ss, segment, ' ')) {
        stringstream vectorStream(segment.substr(1, segment.size() - 2)); // Remove the brackets
        string number;
        while (getline(vectorStream, number, ',')) {
            vector1.push_back(stoi(number));
        }
    }

    // Parse the second vector
    if (getline(ss, segment, ' ')) {
        stringstream vectorStream(segment.substr(1, segment.size() - 2)); // Remove the brackets
        string number;
        while (getline(vectorStream, number, ',')) {
            vector2.push_back(stoi(number));
        }
    }

    cout << "Vector 1: ";
    for (int num : vector1) {
        cout << num << " ";
    }
    cout << endl;

    cout << "Vector 2: ";
    for (int num : vector2) {
        cout << num << " ";
    }
    cout << endl;

    return inner_product(vector1.begin(), vector1.end(), vector2.begin(), 0);
}

void handleUdpClient(int udp_socket_fd) {
    char udp_req_buffer[1024];
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Receive UDP packet
    int bytes_received = recvfrom(udp_socket_fd, udp_req_buffer, sizeof(udp_req_buffer) - 1, 0, (struct sockaddr*)&client_addr, &client_addr_len);
    if (bytes_received < 0) {
        cerr << "Failed to receive UDP packet" << endl;
        return;
    }
    udp_req_buffer[bytes_received] = '\0';

    cout << "Received UDP packet: " << udp_req_buffer << endl;

    int vector_product = processUdpMessage(udp_req_buffer);

    // Process the UDP packet (e.g., send a response)
    string response = "UDP packet received. Result is " + to_string(vector_product);
    int bytes_sent = sendto(udp_socket_fd, response.c_str(), response.length(), 0, (struct sockaddr*)&client_addr, client_addr_len);
    if (bytes_sent < 0) {
        cerr << "Failed to send UDP response" << endl;
    }
}

void startTCP(int argc, char* argv[]) {
    Server server = Server(argv[1]);
    struct sockaddr_in client_addr;
    socklen_t client_addr_size;
    int s_fd;
    s_fd = server.start();
    client_addr_size = sizeof(struct sockaddr_in); 

    while(1){
        //handle client requests.
        //Accept system call .
      
        int client_socket_fd = accept(s_fd, (struct sockaddr*)&client_addr, &client_addr_size);
        if(client_socket_fd < 0){
            cerr << "Failed to accept client request." << endl;
            exit(1);
        }
       
        // Create a new thread to handle the client
        thread clientThread(handleTCPClient, client_socket_fd);
        clientThread.detach(); // Detach the thread to allow it to run independently
    }

}

void startUDP(int argc, char* argv[]) {
    Server server = Server(argv[1]);
    int udp_socket_fd = server.start_udp();

    // while(1){
    //     // Create a new thread to handle the client
    //     thread clientThread(handleUdpClient, udp_socket_fd);
    //     clientThread.detach(); // Detach the thread to allow it to run independently
    // }
    while (1) {
        handleUdpClient(udp_socket_fd);
    }
}

void handleTLSClient(asio::ssl::stream<asio::ip::tcp::socket> ssl_stream) {
    try {
        // Perform SSL handshake
        ssl_stream.handshake(asio::ssl::stream_base::server);

        // Read request
        char request[1024];
        size_t request_length = ssl_stream.read_some(asio::buffer(request));
        cout << "Received request: " << string(request, request_length) << endl;

        request[request_length] = '\0';
        
        HttpRequest req = HttpRequest();
        req.parseRequest(request);
    
        if (req.method.empty() || req.path.empty()) {
            cerr << "Failed to parse request: method or path is empty" << endl;
            ssl_stream.shutdown();
            return;
        }
    
        string mimetype = req.getMimeType(req.path);
        cout << req.path << endl;
    
        HttpResponse res= HttpResponse();
        
        string body = req.readHtmlFile(req.path);
        string response = res.frameHttpResponse("200","OK",req.headers,body,mimetype);
        logServingFile(req.path, mimetype);
    
        // // Write to client.
        // int bytes_sent = send(client_socket_fd, response.c_str(), response.length(), 0);
        // if (bytes_sent < 0) {
        //     cerr << "Failed to send response to client" << endl;
        // }

        // Write response
        // string response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, TLS!";
        asio::write(ssl_stream, asio::buffer(response));
    } catch (std::exception& e) {
        cerr << "Exception in handleTLSClient: " << e.what() << endl;
    }
}

void startTLS(int argc, char* argv[]) {
    try {
        Server server = Server(argv[1]);

        // Create TCP acceptor
        asio::ip::tcp::acceptor acceptor = server.start_tls();
        
        // Start accepting connections
        for (;;) {
            asio::ip::tcp::socket socket(server.io_context);
            acceptor.accept(socket);

            // Create SSL stream
            asio::ssl::stream<asio::ip::tcp::socket> ssl_stream(std::move(socket), server.ssl_context);

            // Create a new thread to handle the client
            thread clientThread(handleTLSClient, std::move(ssl_stream));
            clientThread.detach(); // Detach the thread to allow it to run independently

        }

    } catch (std::exception& e) {
        cerr << "Exception in startTLS: " << e.what() << endl;
    }
}
 
int main(int argc, char* argv[]){
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <port>" << endl;
        return 1;
    }

    // Start TCP service in a separate thread
    // thread tcpThread(startTCP, argc, argv);
    // tcpThread.detach();

    // Start UDP service in a separate thread
    // thread udpThread(startUDP, argc, argv);
    // udpThread.detach();

    // Start TLS service in the main thread
    startTLS(argc, argv);

    return 0;
}