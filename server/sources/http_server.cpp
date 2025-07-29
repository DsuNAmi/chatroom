#include "http_server.h"


void fail(boost::beast::error_code ec, char const * what){
    std::cerr << what << ": " << ec.message() << std::endl;
}

tcp::endpoint HttpServer::MakeEndpoint(){
    if(hs_ip_addr == "localhost"){
        return tcp::endpoint(tcp::v4(), hs_port_num);   
    }else{
        return tcp::endpoint(ip::make_address(hs_ip_addr), hs_port_num);
    }
}

HttpServer::HttpServer(std::string ip_addr, int port_num)
: hs_ip_addr(std::move(ip_addr)), hs_port_num(port_num),
  hs_ioc(1),
  hs_acceptor(hs_ioc)
{
    boost::beast::error_code ec;
    tcp::endpoint t_enpoint = MakeEndpoint();
    hs_acceptor.open(t_enpoint.protocol(), ec);

    if(ec){
        fail(ec, "open");
        throw std::runtime_error("open error");
    }

    hs_acceptor.set_option(net::socket_base::reuse_address(true),ec);
    if(ec){
        fail(ec, "set_option");
        throw std::runtime_error("set_option error");
    }

    hs_acceptor.bind(t_enpoint, ec);
    if(ec){
        fail(ec,"bind");
        throw std::runtime_error("bind error");
    }

    hs_acceptor.listen(net::socket_base::max_listen_connections, ec);
    if(ec){
        fail(ec, "listening");
        throw std::runtime_error("listening error");
    }


    std::cout << std::format("HttpServer is listening on {}, port:{}\n",hs_ip_addr, hs_port_num);

}


HttpServer::~HttpServer(){
    Close();
}


void HttpServer::Run(){
    HAccept();

    hs_thread.emplace_back([this]{hs_ioc.run();});

}

void HttpServer::Close(){
    hs_ioc.stop();

    for(auto & t : hs_thread){
        if(t.joinable()){
            t.join();
        }
    }
    hs_thread.clear();

    if(hs_acceptor.is_open()){
        boost::beast::error_code ec;
        hs_acceptor.close(ec);
        if(ec){
            fail(ec, "acceptor close");
        }
    }

    std::cout << "HTTP Server Stopped\n";

    {
        std::lock_guard<std::mutex> lock(hs_session_mutex);
        hs_active_session.clear();
    }
}

void HttpServer::HAccept(){
    hs_acceptor.async_accept(
        [this](boost::beast::error_code ec, tcp::socket socket){
            if(!ec){
                std::cout << "Accepted new connection from" << socket.remote_endpoint() <<
                std::endl;
                auto session = std::make_shared<Session>(
                    std::move(socket),*this
                );

                {
                    std::lock_guard<std::mutex> lock(hs_session_mutex);
                    hs_active_session.insert(session);
                }
                session->Init();
            }else{
                fail(ec, "accept");
            }

            HAccept();
        }
    );
}

void HttpServer::RemoveSession(std::shared_ptr<Session> session){
    std::lock_guard<std::mutex> lock(hs_session_mutex);
    if(hs_active_session.erase(session)){
        std::cout << 
        "Session removed from active list" << std::endl;
    }
}




HttpServer::Session::Session(tcp::socket socket, HttpServer & server)
: hs_s_socket(std::move(socket)), hs_s_server(server)
{

}

void HttpServer::Session::Init(){
    Read();
}

void HttpServer::Session::Read(){
    // reset the request header
    hs_s_request = {};

    //get the request
    http::async_read(hs_s_socket, hs_s_buffer, hs_s_request,
        boost::beast::bind_front_handler(
            &Session::Reading,
            shared_from_this()
        ));
}

void HttpServer::Session::Reading(boost::beast::error_code ec, std::size_t bytes_transferred
    // ,boost::ignore_unused(bytes_transferred)
){
// close the client
    if(ec == http::error::end_of_stream || ec == net::error::eof){
        std::cout << "Client disconnected gracefully." << std::endl;
        return Close();
    }

    if(ec){
        return fail(ec, "read");
    }

    HandleRequest(std::move(hs_s_request));

}

void HttpServer::Session::HandleRequest(http::request<http::string_body> && req){
    http::response<http::string_body> res;

    res.version(req.version());
    res.keep_alive(req.keep_alive());

    //choose your html page
    if(req.method() == http::verb::get){
        if(req.target() == "/"){
            res.result(http::status::ok);
            res.set(http::field::server, "Myself Boost HTTP Server");
            res.set(http::field::content_type, "text/html");
            res.body() = "<h1>Hello World</h1>";
            res.prepare_payload();
        }
        else{
            res.result(http::status::not_found);
            res.set(http::field::server, "Myself Boost HTTP Server");
            res.set(http::field::content_type, "text/html");
            res.body() = "<h1>404 Not Found</h1>";
            res.prepare_payload();
        }
    }
    else if(req.method() == http::verb::post){
        
    }else{
        res.result(http::status::bad_request);
        res.set(http::field::server, "Myself Boost HTTP Server");
        res.set(http::field::content_type, "text/plain");
        res.prepare_payload();
    }

    Write(std::move(res));
}

void HttpServer::Session::Write(http::response<http::string_body> && res){
    hs_s_res_ptr = std::make_shared<http::response<http::string_body>>(std::move(res));
    http::async_write(
        hs_s_socket,
        *static_cast<http::response<http::string_body>*>(hs_s_res_ptr.get()),
        boost::beast::bind_front_handler(
            &Session::Writing,
            shared_from_this()
        )
    );
}

void HttpServer::Session::Writing(boost::beast::error_code ec, std::size_t bytes_transferred
// ,boost::ignore_unused(bytes_transferred)
){
    
    http::response<http::string_body> & curres = *static_cast<http::response<http::string_body>*>(hs_s_res_ptr.get());
    
    hs_s_res_ptr = nullptr;

    if(ec){
        return fail(ec, "write");
    }

    bool should_keep_alive = curres.keep_alive();

    if(!hs_s_socket.is_open() || !should_keep_alive){
        std::cout << "Closing Connection (Keep Alive:" << std::boolalpha << should_keep_alive << " )" << std::endl;
        return Close();
    }

    Read();
}

void HttpServer::Session::Close(){
    boost::beast::error_code ec;
    hs_s_socket.shutdown(tcp::socket::shutdown_send, ec);

    if(ec == net::error::eof || ec == boost::beast::errc::not_connected){

    }else if(ec){
        fail(ec, "shutdonw");
    }


    hs_s_server.RemoveSession(shared_from_this());
}