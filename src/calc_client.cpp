#include "httplib.h"
#include <iostream>
#include <string>
#include <vector>
#include <cctype>

static std::string join_args(int argc, char** argv, int from) {
    std::string out;
    for (int i = from; i < argc; ++i) {
        if (!out.empty()) out += ' ';
        out += argv[i];
    }
    return out;
}

static bool parse_result_json(const std::string& body, double& out_value, std::string& out_error) {
    auto rpos = body.find("\"result\"");
    if (rpos != std::string::npos) {
        auto colon = body.find(':', rpos);
        if (colon == std::string::npos) return false;
        size_t i = colon + 1;
        while (i < body.size() && std::isspace(static_cast<unsigned char>(body[i]))) ++i;
        size_t j = i;
        while (j < body.size() && (std::isdigit(static_cast<unsigned char>(body[j])) ||
                                   body[j] == '.' || body[j] == '-' || body[j] == '+' ||
                                   body[j] == 'e' || body[j] == 'E')) {
            ++j;
        }
        try {
            out_value = std::stod(body.substr(i, j - i));
            return true;
        } catch (...) {
            return false;
        }
    }

    auto epos = body.find("\"error\"");
    if (epos != std::string::npos) {
        auto colon = body.find(':', epos);
        if (colon == std::string::npos) return false;
        auto q1 = body.find('"', colon + 1);
        if (q1 == std::string::npos) return false;
        auto q2 = body.find('"', q1 + 1);
        if (q2 == std::string::npos) return false;
        out_error = body.substr(q1 + 1, q2 - q1 - 1);
        return true;
    }

    return false;
}

static void print_usage() {
    std::cerr
        << "Usage:\n"
        << "  calc [-H host] [-P port] -e <expression...>\n"
        << "  calc [-H host] [-P port] -c <command>\n\n"
        << "Examples:\n"
        << "  calc -e 2 + 2\n"
        << "  calc -c echo\n"
        << "  calc -H localhost -P 8080 -e 2*(3+4)\n";
}

int main(int argc, char** argv) {
    std::string host = "localhost";
    int port = 8080;

    bool mode_expr = false;
    bool mode_cmd  = false;

    std::string cmd;
    std::string expr;

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];

        if (a == "-H" && i + 1 < argc) {
            host = argv[++i];
        } else if (a == "-P" && i + 1 < argc) {
            port = std::stoi(argv[++i]);
        } else if (a == "-c" && i + 1 < argc) {
            mode_cmd = true;
            cmd = argv[++i];
        } else if (a == "-e" && i + 1 < argc) {
            mode_expr = true;
            expr = join_args(argc, argv, i + 1);
            break;
        } else if (a == "-h" || a == "--help") {
            print_usage();
            return 0;
        } else {
            std::cerr << "error: unknown argument: " << a << "\n";
            print_usage();
            return 2;
        }
    }

    if ((mode_cmd && mode_expr) || (!mode_cmd && !mode_expr)) {
        std::cerr << "error: specify exactly one of -c or -e\n";
        print_usage();
        return 2;
    }

    httplib::Client client(host, port);
    client.set_connection_timeout(2);
    client.set_read_timeout(5);
    client.set_write_timeout(5);

    if (mode_cmd) {
        auto res = client.Post("/cmd", cmd, "text/plain; charset=utf-8");
        if (!res) {
            std::cerr << "error: cannot connect to " << host << ":" << port << "\n";
            return 1;
        }
        if (res->status != 200) {
            std::cerr << "error: server returned HTTP " << res->status << "\n";
            return 1;
        }
        std::cout << res->body << "\n";
        return 0;
    }

    auto res = client.Post("/calc", expr, "text/plain; charset=utf-8");
    if (!res) {
        std::cerr << "error: cannot connect to " << host << ":" << port << "\n";
        return 1;
    }

    double value = 0.0;
    std::string err;
    if (!parse_result_json(res->body, value, err)) {
        std::cerr << "error: bad server response: " << res->body << "\n";
        return 1;
    }

    if (res->status == 200) {
        std::cout << value << "\n";
        return 0;
    }

    std::cerr << "error: " << err << "\n";
    return 1;
}
