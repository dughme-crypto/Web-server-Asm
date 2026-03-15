#pragma once
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>

using Handler = std::function<void(int fd, std::string_view method, std::string_view path)>;

class Router {
public:
void add(const std::string& path, Handler h);
bool dispatch(int fd, std::string_view method, std::string_view path) const;

private:
std::unordered_map<std::string, Handler> routes_;
};
