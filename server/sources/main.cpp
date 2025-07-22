#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <iostream>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;


using tcp = boost::asio::ip::tcp;

int main() {
    try
    {
        net::io_context ioc{1};

        tcp::acceptor acceptor {ioc, tcp::endpoint(tcp::v4(), 9002)};

        std::cout << 
        "WebSocket server listening on ws://localhost:9002\n";

        for(;;){
            tcp::socket socket {ioc};
            acceptor.accept(socket);
            websocket::stream<tcp::socket> ws {std::move(socket)};

            ws.accept();

            for(;;){
                beast::flat_buffer buffer;

                //read messages
                ws.read(buffer);

                //output web recive
                std::string msg = beast::buffers_to_string(buffer.data());
                std::cout << "Received: " << msg << std::endl;

                //echo
                ws.text(ws.got_text()); //保留原消息格式
                ws.write(buffer.data());
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
    

    return EXIT_SUCCESS;
}
