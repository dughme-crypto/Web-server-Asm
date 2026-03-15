#include “server.hpp”
#include “response.hpp”
#include “router.hpp”
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>

#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>

static constexpr size_t BUF_SIZE   = 4096;
static constexpr int    MAX_EVENTS = 1024;

static Router router;

Server::Server(int port, int backlog)
: port_(port), backlog_(backlog), server_fd_(-1), epfd_(-1) {}

Server::~Server() {
if (server_fd_ >= 0) close(server_fd_);
if (epfd_ >= 0)      close(epfd_);
}

void Server::GET(const std::string& path, Handler h) {
router.add(path, std::move(h));
}

int Server::create_socket() {
int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
if (fd < 0) { perror(“socket”); exit(1); }

int opt = 1;
setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));

sockaddr_in addr{};
addr.sin_family      = AF_INET;
addr.sin_port        = htons(port_);
addr.sin_addr.s_addr = INADDR_ANY;

if (bind(fd, (sockaddr*)&addr, sizeof(addr)) < 0) { perror("bind"); exit(1); }
if (listen(fd, backlog_) < 0) { perror("listen"); exit(1); }

return fd;

}

void Server::accept_clients() {
while (true) {
int client_fd = accept4(server_fd_, nullptr, nullptr, SOCK_NONBLOCK);
if (client_fd < 0) break;

    int opt = 1;
    setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));

    epoll_event cev{};
    cev.events  = EPOLLIN | EPOLLET;
    cev.data.fd = client_fd;
    epoll_ctl(epfd_, EPOLL_CTL_ADD, client_fd, &cev);
}

}

void Server::handle_client(int fd) {
char buf[BUF_SIZE];
ssize_t n = recv(fd, buf, sizeof(buf) - 1, 0);
if (n <= 0) { close(fd); return; }
buf[n] = ‘\0’;

std::string_view req(buf, n);

auto line_end = req.find("\r\n");
if (line_end == std::string_view::npos) { send_404(fd); return; }

std::string_view req_line = req.substr(0, line_end);

auto sp1 = req_line.find(' ');
if (sp1 == std::string_view::npos) { send_404(fd); return; }

auto sp2 = req_line.find(' ', sp1 + 1);
if (sp2 == std::string_view::npos) { send_404(fd); return; }

std::string_view method = req_line.substr(0, sp1);
std::string_view path   = req_line.substr(sp1 + 1, sp2 - sp1 - 1);

auto qmark = path.find('?');
if (qmark != std::string_view::npos)
    path = path.substr(0, qmark);

if (!router.dispatch(fd, method, path))
    send_404(fd);

}

void Server::run() {
server_fd_ = create_socket();

epfd_ = epoll_create1(0);
if (epfd_ < 0) { perror("epoll_create1"); exit(1); }

epoll_event ev{};
ev.events  = EPOLLIN;
ev.data.fd = server_fd_;
epoll_ctl(epfd_, EPOLL_CTL_ADD, server_fd_, &ev);

std::cout << "bareHTTP listening on port " << port_ << "...\n";

std::vector<epoll_event> events(MAX_EVENTS);

while (true) {
    int n = epoll_wait(epfd_, events.data(), MAX_EVENTS, -1);
    for (int i = 0; i < n; i++) {
        if (events[i].data.fd == server_fd_) {
            accept_clients();
        } else {
            int fd = events[i].data.fd;
            epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, nullptr);
            handle_client(fd);
            close(fd);
        }
    }
}

}
