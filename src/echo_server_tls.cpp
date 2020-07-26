#define ASIO_STANDALONE
#include <websocketpp/config/asio.hpp>
#include <websocketpp/server.hpp>

#include <iostream>

typedef websocketpp::server<websocketpp::config::asio_tls> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

// pull out the type of messages sent by our config
typedef websocketpp::config::asio::message_type::ptr message_ptr;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;

class CConfigCertificate
{
public:
    CConfigCertificate()
    {
        ;
    }

    virtual ~CConfigCertificate()
    {
        ;
    }

    inline void                         certificateChainPath( const ::std::string& strPath )            { m_strCertificateChainPath = strPath; }
    inline const ::std::string&         certificateChainPath() const                                    { return m_strCertificateChainPath; }

    inline void                         privateKeyPath( const ::std::string& strPath )                  { m_strPrivateKeyPath = strPath; }
    inline const ::std::string&         privateKeyPath() const                                          { return m_strPrivateKeyPath; }

    // Example method of generating this file:
    // `openssl dhparam -out dh.pem 2048`
    // Mozilla Intermediate suggests 1024 as the minimum size to use
    // Mozilla Modern suggests 2048 as the minimum size to use.
    inline void                         dhparamPath( const ::std::string& strPath )                     { m_strDhparamPath = strPath; }
    inline const ::std::string&         dhparamPath() const                                             { return m_strDhparamPath; }

private:
    ::std::string       m_strCertificateChainPath;
    ::std::string       m_strCertificateChainBuffer;

    ::std::string       m_strPrivateKeyPath;
    ::std::string       m_strPrivateKeyBuffer;

    ::std::string       m_strDhparamPath;
    ::std::string       m_strDhparamBuffer;
};

class CConfig
{
public:
    enum ETlsMode {
        TM_MOZILLA_INTERMEDIATE = 1,
        TM_MOZILLA_MODERN       = 2
    };

public:
    static CConfig*                     getInstance()
    {
        if ( NULL == s_pInstance ) {
            s_pInstance = new CConfig;
        }

        return s_pInstance;
    }

    static void                         delInstance()
    {
        if ( NULL != s_pInstance ) {
            delete s_pInstance;
            s_pInstance = NULL;
        }
    }

    CConfig()
        : m_iPort( 8080 )
        , m_strName( "admin" )
        , m_strPassword( "admin" )
    {
        ;
    }
    virtual ~CConfig()
    {
        ;
    }

    void initilalize()
    {
        m_certificateConfig.certificateChainPath( "./server.pem" );
        m_certificateConfig.privateKeyPath( "./server.pem" );
        m_certificateConfig.dhparamPath( "./dh.pem" );
    }

    inline void                         port( unsigned short iPort )                    { m_iPort = iPort; }
    inline unsigned short               port()                                          { return m_iPort; }

    inline void                         name( const ::std::string& strName )            { m_strName = strName; }
    inline const ::std::string&         name() const                                    { return m_strName; }

    inline void                         password( const ::std::string& strPassword )    { m_strPassword = strPassword; }
    inline const ::std::string&         password() const                                { return m_strPassword; }
    ::std::string                       passwordcbk( std::size_t max_length, asio::ssl::context::password_purpose purpose ) const { return m_strPassword; }

    inline const CConfigCertificate&    certificate() const                             { return m_certificateConfig; }

    inline ::std::string                ciphers( ETlsMode mode )
    {
        if ( TM_MOZILLA_INTERMEDIATE == mode ) {
            return "ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-DSS-AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA256:DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:!aNULL:!eNULL:!EXPORT:!DES:!RC4:!3DES:!MD5:!PSK";
        } else {
            return "ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-DSS-AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA256:DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:AES128-GCM-SHA256:AES256-GCM-SHA384:AES128-SHA256:AES256-SHA256:AES128-SHA:AES256-SHA:AES:CAMELLIA:DES-CBC3-SHA:!aNULL:!eNULL:!EXPORT:!DES:!RC4:!MD5:!PSK:!aECDH:!EDH-DSS-DES-CBC3-SHA:!EDH-RSA-DES-CBC3-SHA:!KRB5-DES-CBC3-SHA";
        }
    }

protected:
    // void operator delete( void* ptr )
    // {
    //     if ( NULL != ptr ) {
    //         ::delete ptr;
    //         ptr = NULL;
    //     }
    // }

private:
    static CConfig*                     s_pInstance;

    unsigned short                      m_iPort;
    ::std::string                       m_strName;
    ::std::string                       m_strPassword;

    CConfigCertificate                  m_certificateConfig;
};
CConfig* CConfig::s_pInstance = NULL;

class CHeartBeat
{
public:
    CHeartBeat()
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
        m_server.set_error_channels(websocketpp::log::elevel::all);
        m_server.set_access_channels(websocketpp::log::alevel::all ^ websocketpp::log::alevel::frame_payload);

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

    void registerHandle()
    {
        m_server.set_message_handler( bind( &CHeartBeat::onMessage, this, ::_1, ::_2 ) );
        m_server.set_http_handler( bind( &CHeartBeat::onHttp, this, ::_1 ) );
        m_server.set_tls_init_handler( bind( &CHeartBeat::onTlsInit, this, CConfig::TM_MOZILLA_INTERMEDIATE, ::_1 ) );
    }

    void unregisterHander()
    {
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
    virtual void onMessage( websocketpp::connection_hdl hdl, message_ptr msg ) 
    {
        std::cout << "on_message called with hdl: " << hdl.lock().get()
                << " and message: " << msg->get_payload()
                << std::endl;

        try {
            m_server.send( hdl, msg->get_payload(), msg->get_opcode() );
        } catch (websocketpp::exception const & e) {
            std::cout << "Echo failed because: "
                    << "(" << e.what() << ")" << std::endl;
        }
    }

    virtual void onHttp( websocketpp::connection_hdl hdl ) 
    {
        server::connection_ptr con = m_server.get_con_from_hdl(hdl);
        
        con->set_body("Hello World!");
        con->set_status(websocketpp::http::status_code::ok);
    }

    virtual context_ptr onTlsInit( CConfig::ETlsMode mode, websocketpp::connection_hdl hdl ) 
    {
        namespace asio = websocketpp::lib::asio;

        std::cout << "on_tls_init called with hdl: " << hdl.lock().get() << std::endl;
        std::cout << "using TLS mode: " << ( CConfig::TM_MOZILLA_MODERN == mode ? "Mozilla Modern" : "Mozilla Intermediate" ) << std::endl;

        context_ptr ctx = websocketpp::lib::make_shared< asio::ssl::context >( asio::ssl::context::sslv23 );

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
            ctx->set_password_callback( bind( &CConfig::passwordcbk, CConfig::getInstance(), 256, asio::ssl::context::for_reading )  );
            ctx->use_certificate_chain_file( CConfig::getInstance()->certificate().certificateChainPath() );
            ctx->use_private_key_file( CConfig::getInstance()->certificate().privateKeyPath(), asio::ssl::context::pem );
            ctx->use_tmp_dh_file( CConfig::getInstance()->certificate().dhparamPath() );
            
            if ( SSL_CTX_set_cipher_list( ctx->native_handle(), CConfig::getInstance()->ciphers( mode ).c_str() ) != 1 ) {
                std::cout << "Error setting cipher list" << std::endl;
            }
        } catch ( std::exception& e ) {
            std::cout << "Exception: " << e.what() << std::endl;
        }
        return ctx;
    }

private:
    server m_server;
};


int main() {
    CHeartBeat hb;

    hb.initilalize();

    hb.run();
}
