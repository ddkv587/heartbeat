#ifndef DEF__SESSION_HPP__
#define DEF__SESSION_HPP__

#include <map>

#include "common.hpp"
#include "config.hpp"
#include "db.hpp"

namespace HeartBeat
{
    class ISession
    {
    public:
        ISession()
        {
            ;
        }

        virtual ~ISession()
        {
            ::std::cout << "ISession delete" << ::std::endl;
        }

        virtual void                initialize() = 0;
        virtual void                uninitialize() = 0;

        virtual void                onOpen( _server* sever, ::websocketpp::connection_hdl hdl ) = 0;
        virtual void                onClose( _server* sever, ::websocketpp::connection_hdl hdl ) = 0;
        virtual void                onMessage( _server* sever, ::websocketpp::connection_hdl hdl, _message_ptr msg ) = 0;
        virtual void                onHttp( _server* sever, websocketpp::connection_hdl hdl ) = 0;
        virtual _context_ptr        onTlsInit( _server* sever, CConfig::ETlsMode mode, websocketpp::connection_hdl hdl ) = 0;
        virtual void                onShutdown(  _server* sever, ::websocketpp::connection_hdl hdl ) = 0;
    };

    class CHeartBeatSession : public ISession
    {
    private:
        class CDBRecord final
        {
        public:
            CDBRecord( const ::std::string& strDBPath )
                : m_pDBIAdapter( NULL )
                , m_iInsertKey( -1 )
                , m_iUpdateKey( -1 )
                , m_iDeleteKey( -1 )
                , m_iReadKey( -1 )
            {
                CSqlLiteDB* pDB = new CSqlLiteDB( CConfig::getInstance()-> dbPath() );
                m_pDBIAdapter = pDB; 
                m_pDBIAdapter->initialize();

                ::std::vector<::std::string> aryArgs;
                aryArgs.emplace_back( "record" );     // table
                aryArgs.emplace_back( "ID integer PRIMARY KEY, ZoneName char(32) NOT NULL, RecordName char(32) NOT NULL, IP char(15) NOT NULL" );     // value
                assert( pDB->dbiCreate( aryArgs ) );

                ::std::vector<::std::string> columnlist;
                ::std::vector<::std::string> valuelist;
                ::std::vector<::std::string> setlist;
                ::std::vector<::std::string> wherelist;

                {
                    columnlist.clear();
                    columnlist.emplace_back( "ZoneName" );
                    columnlist.emplace_back( "RecordName" ) ;
                    columnlist.emplace_back( "IP" );

                    m_iInsertKey = pDB->registerInsertStmt( "record", columnlist );
                }

                {
                    setlist.clear();
                    setlist.emplace_back( "IP" );

                    wherelist.clear();
                    wherelist.emplace_back( "ZoneName" );
                    wherelist.emplace_back( "RecordName" );

                    m_iUpdateKey = pDB->registerUpdateStmt( "record", setlist, wherelist );
                }

                {
                    wherelist.clear();
                    wherelist.emplace_back( "ID" );

                    m_iDeleteKey = pDB->registerDeleteStmt( "record", wherelist );
                }

                {
                    columnlist.clear();
                    columnlist.emplace_back( "ID" );
                    columnlist.emplace_back( "IP" );

                    wherelist.clear();
                    wherelist.emplace_back( "ZoneName" );
                    wherelist.emplace_back( "RecordName" );

                    m_iReadKey = pDB->registerReadStmt( "record", columnlist, wherelist );
                }
            }

            ~CDBRecord()
            {
                m_pDBIAdapter->uninitialize();

                delete m_pDBIAdapter;
            }

            ::std::string find( const ::std::string& strZoneName, const ::std::string& strRecordName, bool bSingle = true )
            {
                assert( m_pDBIAdapter );

                int iCol = 0;
                int iRow = 0;
                ::std::vector<IDBInterface::tagValue> aryArgs;
                ::std::vector<sqlite3_value *> result;

                aryArgs.emplace_back( strZoneName );
                aryArgs.emplace_back( strRecordName );
                if ( !m_pDBIAdapter->dbiRead( m_iReadKey, aryArgs, result, iCol, iRow ) ) {
                    ::std::cout << "sql read error!" << ::std::endl;
                    return ::std::string();
                } else {
                    if ( 0 == iRow ) {
                        return ::std::string();
                    } else {
                        if ( iRow > 1 && bSingle ) {
                            for ( int i = 0; i < ( iRow - 1 ); ++i ) {
                                int id = sqlite3_value_int( result[ i * 2 ] );

                                aryArgs.clear();
                                aryArgs.emplace_back( id );
                                assert( m_pDBIAdapter->dbiDelete( m_iDeleteKey, aryArgs ) );
                            }
                        }

                        return ::std::string( reinterpret_cast< const char* >( sqlite3_value_text( result[ ( iRow - 1 ) * 2 + 1 ] ) ) );
                    }
                }
            }

            bool checkUpdate( const ::std::string& strZoneName, const ::std::string& strRecordName, const ::std::string& strIP, bool bInsert = true )
            {
                ::std::string strIPSrc = find( strZoneName, strRecordName );
                
                if ( strIP == strIPSrc ) {
                    return false;
                } else {
                    ::std::vector<IDBInterface::tagValue> aryArgs;

                    if ( strIPSrc.empty() ) {
                        if ( bInsert ) {
                            // insert
                            aryArgs.clear();
                            aryArgs.emplace_back( strZoneName );
                            aryArgs.emplace_back( strRecordName );
                            aryArgs.emplace_back( strIP );

                            assert( m_pDBIAdapter->dbiInsert( m_iInsertKey, aryArgs ) );
                        }
                    } else {
                        // update
                        aryArgs.clear();
                        aryArgs.emplace_back( strIP );
                        aryArgs.emplace_back( strZoneName );
                        aryArgs.emplace_back( strRecordName );

                        assert( m_pDBIAdapter->dbiUpdate( m_iUpdateKey, aryArgs ) );
                    }
                    return true;
                }
            }

        private:
            IDBInterface*               m_pDBIAdapter;

            int                         m_iInsertKey;
            int                         m_iUpdateKey;
            int                         m_iDeleteKey;
            int                         m_iReadKey;
        };

    public:
        CHeartBeatSession()
            : m_pRecordDB( NULL )
        {
            ;
        }

        virtual ~CHeartBeatSession()
        {
            ::std::cout << " session ~CHeartBeatSession" << ::std::endl;
            ;
        }

        virtual void initialize()
        {
            if ( !m_pRecordDB ) {
                m_pRecordDB = new CDBRecord( CConfig::getInstance()-> dbPath() );
            }
        }

        virtual void uninitialize()
        {
            ::std::cout << " session uninitialize" << ::std::endl;
            if ( m_pRecordDB ) {
                delete m_pRecordDB;
                m_pRecordDB = NULL;
            }
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
        int                         m_interval;         // request interval
        int                         m_timeout;
        CDBRecord*                  m_pRecordDB;

        ::std::map< ::std::string, ::std::string >  m_mapDomains;
    };
}
#endif