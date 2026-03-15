#include “router.hpp”
#include “response.hpp”
#include <string>

void Router::add(const std::string& path, Handler h) {
routes_[path] = std::move(h);
}

bool Router::dispatch(int fd, std::string_view method, std::string_view path) const {
std::string key(path);
auto it = routes_.find(key);
if (it == routes_.end()) return false;
it->second(fd, method, path);
return true;
}
