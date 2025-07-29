#pragma once
#include "server.h"

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>

#include <exception>
#include <iostream>
#include <mutex>
#include <set>
#include <functional>
#include <deque>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ip = boost::asio::ip;

using tcp = boost::asio::ip::tcp;

class WebSocketServer: public Server{
    public:
        WebSocketServer(std::string ip_addr = "localhost", int port_num = 9002);
        virtual void Run() override;
        virtual void Close() override;
        virtual ~WebSocketServer();
    private:

        class Session;

        void StartAccept();

        void RemoveSession(std::shared_ptr<Session> session_to_remove);

        void Broadcast(const std::string & message);

        class Session : public std::enable_shared_from_this<Session> {
            public:
                Session(tcp::socket socket, WebSocketServer & server,
                std::function<void(std::shared_ptr<Session>)> on_close_callback);
                virtual ~Session() = default;
                void Init();
                void Send(const std::string & message);
                void Close();
            private:
                void Read();
                void Reading(beast::error_code ec, std::size_t bytes_transferred);
                void Writing(beast::error_code ec, std::size_t bytes_transferred);

                websocket::stream<tcp::socket> wss_ss_ws;
                beast::flat_buffer wss_ss_buffer;
                WebSocketServer& wss_server;
                std::function<void(std::shared_ptr<Session>)> wss_ss_occallback;

                std::deque<std::string> wss_ss_writequeue;
                std::mutex wss_ss_wqmutex;
                bool wss_ss_iswriting;
        };

        tcp::endpoint MakeEndpoint();
        net::io_context wss_ioc;
        int wss_port_num;
        std::string wss_ip_addr;
        tcp::acceptor wss_acceptor;
        std::set<std::shared_ptr<Session>> wss_session;
        std::mutex wss_session_mutex;
};