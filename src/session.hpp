#ifndef DEF__SESSION_HPP__
#define DEF__SESSION_HPP__

#include <map>

#include "common.hpp"
#include "config.hpp"

namespace HeartBeat
{
    class ISession
    {
    public:
        virtual void                initialize( _server* sever ) = 0;
        virtual void                uninitialize( _server* sever ) = 0;

        virtual void                onOpen( _server* sever, ::websocketpp::connection_hdl hdl ) = 0;
        virtual void                onClose( _server* sever, ::websocketpp::connection_hdl hdl ) = 0;
        virtual void                onMessage( _server* sever, ::websocketpp::connection_hdl hdl, _message_ptr msg ) = 0;
        virtual void                onHttp( _server* sever, websocketpp::connection_hdl hdl ) = 0;
        virtual _context_ptr        onTlsInit( _server* sever, CConfig::ETlsMode mode, websocketpp::connection_hdl hdl ) = 0;
        virtual void                onShutdown(  _server* sever, ::websocketpp::connection_hdl hdl ) = 0;
    };

    class CHeartBeatSession : public ISession
    {
    public:
        virtual void initialize( _server* sever )
        {
            ;
        }

        virtual void uninitialize( _server* sever )
        {
            ;
        }

        virtual void onOpen( _server* sever, ::websocketpp::connection_hdl hdl )
        {
            ::std::cout << " onOpen called!!! " << ::std::endl;
        }

        virtual void onClose( _server* sever,::websocketpp::connection_hdl hdl )
        {
            ::std::cout << " onClose called!!! " << ::std::endl;
        }

        virtual void onMessage( _server* sever, websocketpp::connection_hdl hdl, _message_ptr msg ) 
        {
            std::cout << "on_message called with hdl: " << hdl.lock().get()
                    << " and message: " << msg->get_payload()
                    << std::endl;

            _server::connection_ptr con = sever->get_con_from_hdl(hdl);
            std::cout << "on_message remote: " << con->get_remote_endpoint() << ::std::endl;

            try {
                sever->send( hdl, msg->get_payload(), msg->get_opcode() );
            } catch ( websocketpp::exception const & e ) {
                std::cout << "Echo failed because: "
                        << "(" << e.what() << ")" << std::endl;
            }
        }

        virtual void onHttp( _server* sever, websocketpp::connection_hdl hdl );

        virtual _context_ptr onTlsInit( _server* sever, CConfig::ETlsMode mode, websocketpp::connection_hdl hdl ) 
        {
            namespace asio = websocketpp::lib::asio;

            std::cout << "on_tls_init called with hdl: " << hdl.lock().get() << std::endl;
            std::cout << "using TLS mode: " << ( CConfig::TM_MOZILLA_MODERN == mode ? "Mozilla Modern" : "Mozilla Intermediate" ) << std::endl;

            _context_ptr ctx = websocketpp::lib::make_shared< asio::ssl::context >( asio::ssl::context::sslv23 );

            try {
                if ( CConfig::TM_MOZILLA_MODERN == mode ) {
                    // Modern disables TLSv1
                    ctx->set_options(asio::ssl::context::default_workarounds |
                                    asio::ssl::context::no_sslv2 |
                                    asio::ssl::context::no_sslv3 |
                                    asio::ssl::context::no_tlsv1 |
                                    asio::ssl::context::single_dh_use);
                } else {
                    ctx->set_options(asio::ssl::context::default_workarounds |
                                    asio::ssl::context::no_sslv2 |
                                    asio::ssl::context::no_sslv3 |
                                    asio::ssl::context::single_dh_use);
                }
                ctx->set_password_callback( bind( &CConfig::passwordcbk, CConfig::getInstance(), 256, ::asio::ssl::context::for_reading )  );
                ctx->use_certificate_chain_file( CConfig::getInstance()->certificate().certificateChainPath() );
                ctx->use_private_key_file( CConfig::getInstance()->certificate().privateKeyPath(), ::asio::ssl::context::pem );
                ctx->use_tmp_dh_file( CConfig::getInstance()->certificate().dhparamPath() );
                
                if ( SSL_CTX_set_cipher_list( ctx->native_handle(), CConfig::getInstance()->ciphers( mode ).c_str() ) != 1 ) {
                    std::cout << "Error setting cipher list" << std::endl;
                }
            } catch ( std::exception& e ) {
                std::cout   << "onTlsInit Exception: " << e.what() 
                            << " , chain: " << CConfig::getInstance()->certificate().certificateChainPath()
                            << " , private: " << CConfig::getInstance()->certificate().privateKeyPath()
                            << " , dh " << CConfig::getInstance()->certificate().dhparamPath()
                            << std::endl;
            }

            return ctx;
        }

        virtual void onShutdown(  _server* sever, ::websocketpp::connection_hdl hdl )
        {
            ::std::cout << " onTermination called!!! " << ::std::endl;
        }

    private:
        int     m_interval;         // request interval
        int     m_timeout;

        ::std::map< ::std::string, ::std::string >  m_mapDomains;
    };
}
#endif