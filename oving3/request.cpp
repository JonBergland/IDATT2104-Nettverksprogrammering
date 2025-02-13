#include<string>
#include<map>
#include<fstream>
#include<sstream>
#include<iostream>
#include <algorithm>

using namespace std;

class HttpRequest{
public:
    string method;
    string path;
    map<string, string> headers;
    void parseRequest(const string& rawRequest);
    
    string readHtmlFile(const string &path);
    string getMimeType(const string &path);
};


void HttpRequest::parseRequest(const string& rawRequest){
    string request = rawRequest;
    request.erase(remove(request.begin(), request.end(), '\r'), request.end());
    cout << "Parsing request: " << request << endl;
    int currindex=0;
    while(currindex < request.length()){
        if(rawRequest[currindex] == ' '){
            break;
        }
        method += request[currindex];
        currindex++;
    }
    // cout<<method<<endl;
    cout << "Parsed method: " << method << endl;
    headers["method"] = method;

    currindex++;
    while(currindex < request.length()){
        if(request[currindex] == ' '){
            break;
        }
        path += request[currindex];
        currindex++;
    }
   
    cout << "Parsed path: " << path << endl;
    headers["path"] = path;
    
}

string HttpRequest::readHtmlFile(const string &path){
    if (path == "/") {
        this->path = "/main_page.html";
    } else if (path == "/page1") {
        this->path = "/page1.html";
    } else if (path == "/page2") {
        this->path = "/page2.html";
    } else {
        this->path = path;
    }
    

    string filename = this->path.substr(1,path.length());

   
    ifstream file(filename);
    int flag=0;
    if(!file){
        flag=1;
        cerr<<"File not found."<<endl;  
    }

    // cout<<"file found"<<endl;
    if(flag==0){
        stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    return "";
}

string HttpRequest::getMimeType(const string &path){
    map<string, string> mimeTypes = {
        {"html","text/html"},
        {"css","text/css"},
        {"js","text/javascript"},
        {"jpg","image/jpeg"},
        {"png","image/png"}
    };

    size_t pos = path.find_last_of('.');
    if (pos == string::npos || pos + 1 >= path.size()) {
        // default to HTML if no extension
        return "text/html";
    }
    string fileExtension = path.substr(pos + 1);
    // cout<<fileExtension<<endl;
    return mimeTypes[fileExtension];
}

// int main() {
//     HttpRequest request;

//     // Example raw HTTP request
//     string rawRequest = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
//     request.parseRequest(rawRequest);

//     cout << "Method: " << request.method << endl;
//     cout << "Path: " << request.path << endl;

//     // Read HTML file
//     string htmlContent = request.readHtmlFile(request.path);
//     cout << "HTML Content: " << htmlContent << endl;

//     // Get MIME type
//     string mimeType = request.getMimeType(request.path);
//     cout << "MIME Type: " << mimeType << endl;

//     return 0;
// }