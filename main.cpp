#include <App.h>

int main() {
    uWS::App().get("/*", [](auto *res, auto *req) {
        res->end("Hello World!");
    }).listen(9001, [](auto *listenSocket) {
        if (listenSocket) {
            std::cout << "Listening for connections..." << std::endl;
        }
    }).run();

    std::cout << "Shoot! We failed to listen and the App fell through, exiting now!" << std::endl;
}
