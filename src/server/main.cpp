#include <iostream>
#include <string>

#include "httplib.h"

int main(){
    httplib::Server server;

    server.Post("/echo", [](const httplib::Request& req, httplib::Response& res){
        res.status = 200;
        res.set_content(req.body, "text/plain; charset=utf-8");
    });

    const std::string host = "0.0.0.0";
    const int port = 8080;

    std::cout << "Echo server listening on http://" << host << ":" << port << std::endl;
    std::cout << "Try: curl -X POST http://localhost:8080/echo -d 'hi'" << std::endl;

    const bool ok = server.listen(host,port);

    if(!ok){
        std::cerr << "Failed to start server. Is port " << port << " already in use?\n";
        return 1;
    }

    return 0;
}