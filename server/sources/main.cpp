#include "ws_server.h"
#include "utils.h"
#include "sql_database.h"
#include "user.h"
#include "admin.h"
#include "http_server.h"
#include "threadpool.h"

#include <chrono>

int main() {
    auto run_server = [](const std::shared_ptr<Server> & server){
        server->Run();
    };

    try
    {
        std::shared_ptr<Server> ws_server = std::make_shared<WebSocketServer>();
        std::shared_ptr<Server> ht_server = std::make_shared<HttpServer>();

        ThreadPool pools(2);
        pools.Enqueue(run_server, ws_server);
        pools.Enqueue(run_server, ht_server);


        // std::cout << "Servers are all stopped." << std::endl;


        // std::this_thread::sleep_for(std::chrono::seconds(3));

        // std::cout << "Waiting for all threads, goodbye." << std::endl;

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
    

    return EXIT_SUCCESS;
}