#include “server.hpp”
#include “response.hpp”

static const char* INDEX_HTML =
“<!DOCTYPE html><html lang='en'><head>”
“<meta charset='UTF-8'>”
“<title>bareHTTP</title>”
“<style>”
“:root{–bg:#0a0a0f;–gold:#f0a500;–fg:#e8e0d0;–dim:#555}”
“*{margin:0;padding:0;box-sizing:border-box}”
“body{background:var(–bg);color:var(–fg);font-family:‘Courier New’,monospace;”
“display:flex;align-items:center;justify-content:center;min-height:100vh}”
“.grid{position:fixed;inset:0;”
“background-image:linear-gradient(rgba(240,165,0,.04) 1px,transparent 1px),”
“linear-gradient(90deg,rgba(240,165,0,.04) 1px,transparent 1px);”
“background-size:40px 40px;pointer-events:none}”
“.box{text-align:center;z-index:1;padding:3rem 2rem}”
“h1{font-size:clamp(3rem,10vw,6rem);color:var(–gold);”
“text-shadow:0 0 40px rgba(240,165,0,.5);border:3px solid var(–gold);”
“display:inline-block;padding:.3em .7em;animation:flicker 4s infinite}”
“p{margin-top:2rem;opacity:.8;line-height:1.9;max-width:480px;”
“margin-left:auto;margin-right:auto}”
“.badge{display:inline-block;margin-top:2rem;font-size:.7rem;”
“letter-spacing:.2em;text-transform:uppercase;color:var(–dim);”
“border:1px solid var(–dim);padding:.4em 1em}”
“@keyframes flicker{0%,95%,100%{opacity:1}96%{opacity:.8}97%{opacity:1}98%{opacity:.6}99%{opacity:1}}”
“</style></head><body>”
“<div class='grid'></div>”
“<div class='box'>”
“<h1>bareHTTP</h1>”
“<p>Welcome to this website, built using <strong>bareHTTP</strong>,<br>”
“a webserver written in C++ with epoll.</p>”
“<div class='badge'>HTTP/1.1  ·  epoll  ·  C++  ·  port 8080</div>”
“</div></body></html>”;

int main() {
Server app(8080);
  
app.GET("/", [](int fd, std::string_view, std::string_view) {
    send_200(fd, INDEX_HTML);
});

app.GET("/ping", [](int fd, std::string_view, std::string_view) {
    send_200(fd, "pong", "text/plain");
});

app.GET("/json", [](int fd, std::string_view, std::string_view) {
    send_200(fd, R"({"status":"ok","server":"bareHTTP"})", "application/json");
});

app.run();
return 0;

}
