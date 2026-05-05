#include "server.h"
#include "newsserver.h"
#include "inmemorydatabase.h"

#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: server port-number" << std::endl;
        return 1;
    }
    int port = std::stoi(argv[1]);
    Server server(port);
    if (!server.isReady()) {
        std::cerr << "Server failed to start on port " << port << std::endl;
        return 2;
    }
    InMemoryDatabase db;
    NewsServer ns(server, db);
    ns.run();
}
