#include "ws_server.h"


WebSocketServer::WebSocketServer(std::string ip_addr = "localhost", int port_num = 9002)
: wss_ip_addr(std::move(ip_addr)), wss_port_num(port_num), wss_ioc(1)
{
   

}

tcp::endpoint WebSocketServer::MakeEndpoint(){
    if(wss_ip_addr == "localhost"){
        return tcp::endpoint(tcp::v4(), wss_port_num);
    }else{
        return tcp::endpoint(ip::make_address(wss_ip_addr),wss_port_num);
    }
}