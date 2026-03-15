#pragma once
#include <string_view>

void send_response(int fd, int status, std::string_view status_text,
std::string_view content_type, std::string_view body);

void send_200(int fd, std::string_view body,
std::string_view ct = “text/html; charset=utf-8”);

void send_404(int fd);
