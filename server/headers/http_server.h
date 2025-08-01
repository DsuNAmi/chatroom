#pragma once 

#include "server.h"


#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>

#include <thread>
#include <vector>
#include <set>
#include <functional>
#include <iostream>
#include <exception>
#include <fstream>

namespace net = boost::asio;
namespace http = boost::beast::http;
namespace ip = boost::asio::ip;
using tcp = net::ip::tcp;


class HttpServer : public Server{
    public:
        HttpServer(std::string ip_addr = "localhost", int port_num = 8080);
        
        virtual void Run() override;
        virtual void Close() override;
        virtual ~HttpServer();

    private:
        class Session;


        void HAccept();

        void RemoveSession(std::shared_ptr<Session> session);

        class Session : public std::enable_shared_from_this<Session>{
            public:
                Session(tcp::socket socket, HttpServer & server);
                virtual ~Session() = default;

                void Init();

            private:
                void Read();
                void Reading(boost::beast::error_code ec, std::size_t bytes_transferred);
                void HandleRequest(http::request<http::string_body> && req);
                void Write(http::response<http::string_body> && res);
                void Writing(boost::beast::error_code ec, std::size_t bytes_transferred);
                void Close();

                tcp::socket hs_s_socket;
                boost::beast::flat_buffer hs_s_buffer;
                http::request<http::string_body> hs_s_request;
                std::shared_ptr<void> hs_s_res_ptr;
                HttpServer & hs_s_server;
        };

        tcp::endpoint MakeEndpoint();

        std::string hs_ip_addr;
        int hs_port_num;
        net::io_context hs_ioc;
        tcp::acceptor hs_acceptor;

        std::vector<std::thread> hs_thread; //thread pool
        std::set<std::shared_ptr<Session>> hs_active_session;
        std::mutex hs_session_mutex;
};