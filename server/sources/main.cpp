#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_context io_context;

        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 12345));
        std::cout << "Server is running on port 12345..." << std::endl;

        for (;;) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);

            std::string message = "Hello from Boost.Asio!\n";
            boost::system::error_code ignored_error;
            boost::asio::write(socket, boost::asio::buffer(message), ignored_error);

            std::cout << "Sent a greeting to a client." << std::endl;
        }
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
