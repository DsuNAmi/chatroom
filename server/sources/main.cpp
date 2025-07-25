#include "ws_server.h"
#include "utils.h"
#include "sql_database.h"
#include "user.h"
#include "admin.h"

int main() {
    // try
    // {
    //     net::io_context ioc{1};

    //     tcp::acceptor acceptor {ioc, tcp::endpoint(tcp::v4(), 9002)};

    //     std::cout << 
    //     "WebSocket server listening on ws://localhost:9002\n";

    //     for(;;){
    //         tcp::socket socket {ioc};
    //         acceptor.accept(socket);
    //         websocket::stream<tcp::socket> ws {std::move(socket)};

    //         ws.accept();

    //         for(;;){
    //             beast::flat_buffer buffer;

    //             //read messages
    //             ws.read(buffer);

    //             //output web recive
    //             std::string msg = beast::buffers_to_string(buffer.data());
    //             std::cout << "Received: " << msg << std::endl;

    //             //echo
    //             ws.text(ws.got_text()); //保留原消息格式
    //             ws.write(buffer.data());
    //         }
    //     }
    // }
    // catch(const std::exception& e)
    // {
    //     std::cerr << e.what() << '\n';
    //     return EXIT_FAILURE;
    // }

    // try{
    //     // auto websocket_server = WebSocketServer();
    //     // websocket_server.Run();
    //     server->Run();

    // }catch(const std::exception & e){
    //     std::cerr << e.what() << "\n";
    //     return EXIT_FAILURE;
    // }

    // Utils::queryArgs test = Utils::ConstructQuery({"name"sv,"password"sv}, std::make_tuple("Jingrui"sv,"123445"sv));

    // for(const auto & [k, v] : test){
    //     std::cout << std::format("key:{}, value:{}\n",k,v);
    // }

    std::unique_ptr<Database> lite_db = std::make_unique<SQLiteDatabase>();
    // lite_db->Isin("users"sv, std::move(test));

    auto user_type = User::CheckAccount(lite_db.get(),"admin","1234");
    std::unique_ptr<User> user = nullptr;
    if(user_type.has_value()){
        switch (user_type.value())
        {
        case Level::Admin:
            std::cout << "Create a admin object.\n";
            user = std::make_unique<Admin>();
            break;
        default:
            std::cout << "other account type.\n";
            break;
        }
    }else{  
        std::cout << "Account is not exists.\n";
    }
    if(user) std::cout << user->GetIdentity() << std::endl;


    return EXIT_SUCCESS;
}