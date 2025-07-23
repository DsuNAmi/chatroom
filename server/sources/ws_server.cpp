#include "ws_server.h"


WebSocketServer::WebSocketServer(std::string ip_addr, int port_num)
: wss_ioc(1),
wss_ip_addr(std::move(ip_addr)), 
wss_port_num(port_num),
wss_acceptor(wss_ioc, MakeEndpoint())
{
   

}

WebSocketServer::~WebSocketServer(){
    Close();
    wss_ioc.stop();
}

tcp::endpoint WebSocketServer::MakeEndpoint(){
    if(wss_ip_addr == "localhost"){
        return tcp::endpoint(tcp::v4(), wss_port_num);
    }else{
        return tcp::endpoint(ip::make_address(wss_ip_addr),wss_port_num);
    }
}

void WebSocketServer::Run()
{
    if(!wss_acceptor.is_open()){
        std::cerr << "Server acceptor not open. Check initialization." << std::endl;
        return;
    }
    std::cout << std::format("Server listening on {}, port:{}", wss_ip_addr, wss_port_num) << std::endl;
    StartAccept();
    wss_ioc.run();
}

void WebSocketServer::StartAccept(){
    wss_acceptor.async_accept([this](beast::error_code ec, tcp::socket socket){
        if(!ec){
            auto session = std::make_shared<Session>(std::move(socket), *this,
            [this](std::shared_ptr<Session> s){
                this->RemoveSession(s);
            });
            {
                std::lock_guard<std::mutex> lock(wss_session_mutex);
                wss_session.insert(session);
            }
            session->Init();
        }else{
            std::cerr << "Accept error: " << ec.message() << std::endl;
        }
        StartAccept();
    });
}

void WebSocketServer::RemoveSession(std::shared_ptr<Session> session_to_remove){
    std::lock_guard<std::mutex> lock(wss_session_mutex);
    // if session exists, return 1, o otherwise
    if(wss_session.erase(session_to_remove)){
        std::cout << "Session removed from active list." << std::endl; 
    }
}

void WebSocketServer::Broadcast(const std::string & message){
    std::lock_guard<std::mutex> lock(wss_session_mutex);
    for(auto & session : wss_session){
        session->Send(message);
    }
}

void WebSocketServer::Close(){
    std::lock_guard<std::mutex> lock(wss_session_mutex);
    for(auto & session : wss_session){
        session->Close();
    }
    wss_session.clear();
    std::cout << "All sessions cleared and server is shutting down." << std::endl;
}


//Session

WebSocketServer::Session::Session(tcp::socket socket, WebSocketServer & server,
std::function<void(std::shared_ptr<Session>)> on_close_callback)
: wss_ss_ws(std::move(socket)), wss_server(server),
wss_ss_occallback(on_close_callback),
wss_ss_iswriting(false)
{

}

void WebSocketServer::Session::Init(){
    wss_ss_ws.async_accept([self = shared_from_this()](beast::error_code ec){
        if(!ec){
            self->Read();
        }else{
            std::cerr << "Session accept error: " << ec.message() << std::endl;
            self->wss_ss_occallback(self); // bind RemoveSession(...)
        }
    });
}

void WebSocketServer::Session::Read(){
    wss_ss_ws.async_read(wss_ss_buffer, [self = shared_from_this()](beast::error_code ec,
    std::size_t bytes_transferred){
        self->Reading(ec, bytes_transferred);
    });
}

void WebSocketServer::Session::Reading(beast::error_code ec, std::size_t bytes_transferred){

    if(ec == websocket::error::closed || ec == net::error::eof){
        std::cerr << "Client disconnected gracefully." << std::endl;
        wss_ss_occallback(shared_from_this());
        return;
    }

    if(ec){
        std::cerr << "Read error: " << ec.message() << std::endl;
        wss_ss_occallback(shared_from_this());
        return;
    }
    std::string msg = beast::buffers_to_string(wss_ss_buffer.data());
    wss_ss_buffer.consume(wss_ss_buffer.size()); //clear the buffer

    std::cout << "Received: " << msg << std::endl;
    wss_server.Broadcast(msg);

    //continue read
    Read();
}

void WebSocketServer::Session::Send(const std::string & message){
    {
        std::lock_guard<std::mutex> lock(wss_ss_wqmutex);
        wss_ss_writequeue.push_back(message);
        if(wss_ss_iswriting){
            return;
        }
        wss_ss_iswriting = true;
    }
    wss_ss_ws.text(true);
    wss_ss_ws.async_write(net::buffer(wss_ss_writequeue.front()),
    [self = shared_from_this()](beast::error_code ec, std::size_t bytes_transferred){
        self->Writing(ec, bytes_transferred);
    });
}

void WebSocketServer::Session::Writing(beast::error_code ec, std::size_t bytes_transferred){
    if(ec){
        std::cerr << "Write error" << ec.message() << std::endl;
        wss_ss_occallback(shared_from_this());
        return;
    }

    {
        std::lock_guard<std::mutex> lock(wss_ss_wqmutex);
        wss_ss_writequeue.pop_front();

        if(!wss_ss_writequeue.empty()){
            wss_ss_ws.async_write(net::buffer(wss_ss_writequeue.front()),
        [self = shared_from_this()](beast::error_code ec, std::size_t bytes_transferred){
            self->Writing(ec, bytes_transferred);
        });
        }else{
            wss_ss_iswriting = false;
        }
    }
}

void WebSocketServer::Session::Close(){
    beast::error_code ec;
    wss_ss_ws.close(websocket::close_code::normal, ec);
    if(ec){
        std::cerr << "Error during close: " << ec.message() << std::endl;
    }
}