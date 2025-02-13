#include<string>
#include<map>
#include<fstream>
#include<sstream>
#include<iostream>

using namespace std;

class HttpResponse{ 
    string statuscode;
    string statusmsg;
    map<std::string, std::string> headers;
    string body;
    public:
    string frameHttpResponse(
        string statuscode, string statusmsg, map<string, string> headers, 
        string body, string mimetype);
};

string HttpResponse::frameHttpResponse(string statuscode, string statusmsg, map<string, string> headers, string body,string mimetype){
    headers["content-type"] = mimetype;
    headers["content-length"] = to_string(body.length());
    ostringstream buffer;
    buffer << "HTTP/1.1 " << statuscode << " " << statusmsg << "\r\n";
    for(auto x : headers){
        buffer << x.first << ": " << x.second << "\r\n";
    }
    buffer << "\r\n" << body;
    return buffer.str();
}

// int main() {
//     HttpResponse response;
//     map<std::string, std::string> headers;
//     string response_str = response.frameHttpResponse("200", "OK", headers, "<html><body>Hello, World!</body></html>", "text/html");
//     cout << response_str << std::endl;

//     return 0;
// }