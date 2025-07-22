#pragma once
#include "server.h"

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>

#include <exception>
#include <iostream>
#include <mutex>



namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ip = boost::asio::ip;

using tcp = boost::asio::ip::tcp;

class WebSocketServer: public Server{
    public:
        virtual void Run(std::string_view ip_addr = "localhost", int port_num = 9002) override;
        virtual void Close() override;
    private:
        WebSocketServer(std::string ip_addr, int port_num);

        class Session : public std::enable_shared_from_this<Session> {
            public:
                Session(tcp::socket socket, WebSocketServer & Server);
                void Init();
                void Send();
            private:
                void Read();
                void Reading(beast::error_code ec, std::size_t bytes_transferred);
                void Writing(beast::error_code ec, std::size_t bytes_transferred);

                websocket::stream<tcp::socket> wss_ss_ws;
                beast::flat_buffer wss_ss_buffer;
                WebSocketServer& wss_server;
        };

        tcp::endpoint MakeEndpoint();
        net::io_context wss_ioc;
        int wss_port_num;
        std::string wss_ip_addr;
        tcp::acceptor wss_acceptor;
        std::mutex wss_session_mutex;
};