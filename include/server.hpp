#pragma once
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>

using Handler = std::function<void(int fd, std::string_view method, std::string_view path)>;

class Server {
public:
Server(int port, int backlog = 1024);
~Server();

void GET(const std::string& path, Handler h);
void run();

private:
int port_;
int backlog_;
int server_fd_;
int epfd_;
std::unordered_map<std::string, Handler> routes_;

int  create_socket();
void accept_clients();
void handle_client(int fd);

};
