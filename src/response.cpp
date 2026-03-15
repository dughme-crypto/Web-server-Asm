#include “response.hpp”
#include <sys/uio.h>
#include <sys/socket.h>
#include <cstdio>

void send_response(int fd, int status, std::string_view status_text,
std::string_view content_type, std::string_view body) {
char header[512];
int hlen = snprintf(header, sizeof(header),
“HTTP/1.1 %d %.*s\r\n”
“Content-Type: %.*s\r\n”
“Content-Length: %zu\r\n”
“Connection: keep-alive\r\n”
“\r\n”,
status,
(int)status_text.size(), status_text.data(),
(int)content_type.size(), content_type.data(),
body.size()
);

struct iovec iov[2];
iov[0].iov_base = header;
iov[0].iov_len  = hlen;
iov[1].iov_base = const_cast<char*>(body.data());
iov[1].iov_len  = body.size();
writev(fd, iov, 2);

}

void send_200(int fd, std::string_view body, std::string_view ct) {
send_response(fd, 200, “OK”, ct, body);
}

void send_404(int fd) {
send_response(fd, 404, “Not Found”, “text/plain”, “404 Not Found”);
}
