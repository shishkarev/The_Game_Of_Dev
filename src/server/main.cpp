#include <iostream>
#include <string>
#include "calc/lexer.h"
#include "calc/parser.h"
#include "calc/eval.h"

#include "httplib.h"

static std::string json_escape(const std::string& s){
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        switch (c) {
            case '\\': out += "\\\\"; break;
            case '"':  out += "\\\""; break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:   out += c;      break;
        }
    }
    return out;
}

int main(){
    httplib::Server server;

    server.Post("/calc", [](const httplib::Request& req, httplib::Response& res) {
        const std::string expr = req.body;

        try {
            calc::Lexer lex(expr);
            calc::Parser parser(std::move(lex));
            auto ast = parser.parse();
            double result = calc::eval(*ast);

            res.status = 200;
            res.set_content("{\"result\":" + std::to_string(result) + "}", "application/json; charset=utf-8");
        } catch (const std::exception& ex) {
            res.status = 400;
            res.set_content("{\"error\":\"" + json_escape(ex.what()) + "\"}", "application/json; charset=utf-8");
        }
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