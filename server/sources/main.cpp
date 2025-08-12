#include "ws_server.h"
#include "utils.h"
#include "sql_database.h"
#include "user.h"
#include "admin.h"
#include "http_server.h"
#include "threadpool.h"

#include <chrono>

int main() {
    // auto run_server = [](const std::shared_ptr<Server> & server){
    //     server->Run();
    // };

    // try
    // {
    //     std::shared_ptr<Server> ws_server = std::make_shared<WebSocketServer>();
    //     std::shared_ptr<Server> ht_server = std::make_shared<HttpServer>();

    //     ThreadPool pools(2);
    //     pools.Enqueue(run_server, ws_server);
    //     pools.Enqueue(run_server, ht_server);


    //     // std::cout << "Servers are all stopped." << std::endl;


    //     // std::this_thread::sleep_for(std::chrono::seconds(3));

    //     // std::cout << "Waiting for all threads, goodbye." << std::endl;

    // }
    // catch(const std::exception& e)
    // {
    //     std::cerr << e.what() << '\n';
    //     return EXIT_FAILURE;
    // }

    SQLiteDatabase s_database("../database/chatroom.db");
    std::vector<std::string> create_table_sqls = s_database.ImportCreateSQL("../database/tables.txt");
    for(const auto & cts : create_table_sqls){
        std::cout << "Ready to create table:" << std::endl;
        std::cout << cts << std::endl;
        std::cout << "One table end." << std::endl;
    }
    

    return EXIT_SUCCESS;
}