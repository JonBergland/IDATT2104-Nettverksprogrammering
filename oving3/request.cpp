#include<string>
#include<map>
#include<fstream>
#include<sstream>
#include<iostream>

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
    int currindex=0;
    while(currindex < rawRequest.length()){
        if(rawRequest[currindex] == ' '){
            break;
        }
        method += rawRequest[currindex];
        currindex++;
    }
    // cout<<method<<endl;
    headers["method"] = method;

    currindex++;
    while(currindex < rawRequest.length()){
        if(rawRequest[currindex] == ' '){
            break;
        }
        path += rawRequest[currindex];
        currindex++;
    }
   
    headers["path"] = path;
    
}

string HttpRequest::readHtmlFile(const string &path){
    if (path == "/") {
        this->path = "/main_page.html";
    } else if (path == "/page1")
    {
        this->path = "/page1.html";
    } else if (path == "/page2")
    {
        this->path = "/page2.html";
    }
    

    string filename = path.substr(1,path.length());

   
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
    string fileExtension = path.substr(path.find_last_of(".") + 1);
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