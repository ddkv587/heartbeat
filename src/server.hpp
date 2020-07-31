#ifndef DEF__SERVER_HPP__
#define DEF__SERVER_HPP__

#include "common.hpp"
#include "session.hpp"

namespace HeartBeat
{
    class CHeartBeat
    {
    public:
        CHeartBeat()
            : m_session( NULL )
        {
            ;
        }

        virtual ~CHeartBeat()
        {
            ;
        }

        bool initilalize()
        {
            // Set logging settings
            m_server.set_error_channels( ::websocketpp::log::elevel::all );
            m_server.set_access_channels( ::websocketpp::log::alevel::all ^ ::websocketpp::log::alevel::frame_payload );

            // Initialize Asio
            m_server.init_asio();

            // register handle
            registerHandle();

            return true;
        }

        void unInitilalize()
        {
            unregisterHander();
        }

        void registerSession( ISession* session )
        {
            if ( session == m_session ) return;

            if ( m_session ) m_session->uninitialize( &m_server );

            m_session = session;
        }

        void unregisterSession()
        {
            if ( m_session ) m_session->uninitialize( &m_server );
            m_session = NULL;
        }

        void registerHandle()
        {
            m_server.set_open_handler( ::std::bind( &ISession::onOpen, m_session, &m_server, _1 ) );
            m_server.set_close_handler( ::std::bind( &ISession::onClose, m_session, &m_server, _1 ) );
            m_server.set_message_handler( ::std::bind( &ISession::onMessage, m_session, &m_server, _1, _2 ) );
            m_server.set_http_handler( ::std::bind( &ISession::onHttp, m_session, &m_server, _1 ) );
            m_server.set_tls_init_handler( ::std::bind( &ISession::onTlsInit, m_session, &m_server, CConfig::TM_MOZILLA_INTERMEDIATE, _1 ) );
        }

        void unregisterHander()
        {
            m_server.set_open_handler( NULL );
            m_server.set_close_handler( NULL );
            m_server.set_message_handler( NULL );
            m_server.set_http_handler( NULL );
            m_server.set_tls_init_handler( NULL );
        }

        void run()
        {
            // Listen 
            m_server.listen( CConfig::getInstance()->port() );
            // Queues a connection accept operation
            m_server.start_accept();
            // Start the Asio io_service run loop
            m_server.run();
        }

    protected:
        

    private:
        _server         m_server;
        ISession*       m_session;
    };
}
#endif