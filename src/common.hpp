#ifndef DEF__COMMON_HPP__
#define DEF__COMMON_HPP__

#define ASIO_STANDALONE
#include <websocketpp/config/debug_asio.hpp>
#include <websocketpp/server.hpp>

#include <iostream>

namespace HeartBeat
{
    typedef websocketpp::server<websocketpp::config::debug_asio_tls> _server;

    using websocketpp::lib::placeholders::_1;
    using websocketpp::lib::placeholders::_2;

    // pull out the type of messages sent by our config
    typedef websocketpp::config::debug_asio::message_type::ptr _message_ptr;
    typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> _context_ptr;
};

#endif