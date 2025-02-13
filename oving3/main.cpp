#include"server.cpp"
#include <string.h>
#include <unistd.h>
#include"request.cpp"
#include"response.cpp"
#include<thread>

using namespace std;

//utility function to log the serving of a file.
void logServingFile(const string& path, const string& mimetype) {
    cout << "Serving file: " << path << " with MIME type: " << mimetype << endl;
}

void handleClient(int client_socket_fd){
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
int main(int argc, char* argv[]){
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
        thread clientThread(handleClient, client_socket_fd);
        clientThread.detach(); // Detach the thread to allow it to run independently
       

    }

    return 0;
}