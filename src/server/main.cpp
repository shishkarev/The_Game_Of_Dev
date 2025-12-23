#include <iostream>
#include <string>
#include <mutex>
#include <unordered_map>
#include <random>
#include <sstream>
#include <iomanip>
#include "calc/lexer.h"
#include "calc/parser.h"
#include "calc/eval.h"

#include "httplib.h"

static std::mutex g_sessions_mtx;
static std::unordered_map<std::string, calc::Env> g_sessions;

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

static std::string make_session_id() {
    std::random_device rd;
    std::uniform_int_distribution<int> dist(0, 15);

    std::ostringstream oss;
    for (int i = 0; i < 32; ++i) oss << std::hex << dist(rd);
    return oss.str();
}

static std::string get_user_id(const httplib::Request& req) {
    auto it = req.headers.find("X-User");
    std::string user = (it != req.headers.end()) ? it->second : "";
    if (user.empty()) user = "anonymous";
    return user;
}

static std::string get_session_key(const httplib::Request& req, httplib::Response& res, std::string& out_sid) {
    const std::string user = get_user_id(req);

    auto it = req.headers.find("X-Session-Id");
    std::string sid = (it != req.headers.end()) ? it->second : "";

    if (sid.empty()) sid = make_session_id();

    res.set_header("X-Session-Id", sid);
    out_sid = sid;

    return user + ":" + sid;
}

int main(){
    httplib::Server server;

    server.Post("/calc", [](const httplib::Request& req, httplib::Response& res) {
        try {
            std::string sid;
            auto key = get_session_key(req, res, sid);
            calc::Lexer lex(req.body);
            calc::Parser parser(std::move(lex));
            auto program = parser.parse();

            std::lock_guard<std::mutex> lock(g_sessions_mtx);
            auto& env = g_sessions[key];
            double result = calc::eval(program, env);
            env["_"] = result;

            res.status = 200;
            res.set_content("{\"result\":" + std::to_string(result) + "}",
                            "application/json; charset=utf-8");
        } catch (const std::exception& ex) {
            res.status = 400;
            res.set_content("{\"error\":\"" + json_escape(ex.what()) + "\"}",
                            "application/json; charset=utf-8");
        }
    });


    server.Post("/cmd", [](const httplib::Request& req, httplib::Response& res) {
        const std::string cmd = req.body;

        if (cmd == "echo") {
            std::string sid;
            auto key = get_session_key(req, res, sid);
            (void)key;
            res.status = 200;
            res.set_content("echo", "text/plain; charset=utf-8");
            return;
        }

        if (cmd == "clean") {
            std::string sid;
            auto key = get_session_key(req, res, sid);
            std::lock_guard<std::mutex> lock(g_sessions_mtx);
            g_sessions.erase(key);
            res.status = 200;
            res.set_content("OK", "text/plain; charset=utf-8");
            return;
        }

        res.status = 400;
        res.set_content("{\"error\":\"Unknown command\"}", "application/json; charset=utf-8");
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