#ifndef DEF__CONFIG_HPP__
#define DEF__CONFIG_HPP__

#include "common.hpp"

namespace HeartBeat
{
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
        static CConfig* getInstance()
        {
            if ( NULL == s_pInstance ) {
                s_pInstance = new CConfig;
            }

            return s_pInstance;
        }

        static void delInstance()
        {
            if ( NULL != s_pInstance ) {
                delete s_pInstance;
                s_pInstance = NULL;
            }
        }

        void initialize()
        {
            m_iPort     = 8080;
            m_strName   = "admin";
            m_strDataBasePath = "./record.db";
            m_strKeyChain="nibiru";
            m_certificateConfig.certificateChainPath( "./nibiru.com.cert.pem" );
            m_certificateConfig.privateKeyPath( "./nibiru.com.key.pem" );
            m_certificateConfig.dhparamPath( "./dh.pem" );

            loadConfig();
        }

        void uninitialize()
        {
            saveConfig();
        }

        bool                                loadConfig( const ::std::string& strConfigPath = "./config.json" );
        void                                saveConfig( const ::std::string& strConfigPath = "./config.json" );

        inline void                         port( unsigned short iPort )                    { m_iPort = iPort; }
        inline unsigned short               port()                                          { return m_iPort; }

        inline void                         name( const ::std::string& strName )            { m_strName = strName; }
        inline const ::std::string&         name() const                                    { return m_strName; }

        inline void                         password( const ::std::string& strPassword )    { m_strPassword = strPassword; }
        inline const ::std::string&         password() const                                { return m_strPassword; }
        ::std::string                       passwordcbk( std::size_t max_length, asio::ssl::context::password_purpose purpose ) const { return m_strPassword; }

        inline void                         keyChain( const ::std::string& strKeyChain )    { m_strKeyChain = strKeyChain; }
        inline const ::std::string&         keyChain() const                                { return m_strKeyChain; }
        bool                                checkKeyChain( const ::std::string& strKeyChain );

        inline void                         dbPath( const ::std::string& strPath )          { m_strDataBasePath = strPath; }
        inline const ::std::string&         dbPath() const                                  { return m_strDataBasePath; }

        inline void                         authorization( const ::std::string& strKey )    { m_strAuthorization = strKey; }
        inline const ::std::string&         authorization() const                           { return m_strAuthorization; }

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
        CConfig()
        {
            initialize();
        }

        virtual ~CConfig()
        {
            uninitialize();
        }

        // void operator delete( void* ptr )
        // {
        //     if ( NULL != ptr ) {
        //         ::delete ptr;
        //         ptr = NULL;
        //     }
        // }

    private:
        static CConfig*                     s_pInstance;

        // for server
        unsigned short                      m_iPort;
        ::std::string                       m_strName;
        ::std::string                       m_strPassword;
        ::std::string                       m_strKeyChain;

        // for database
        ::std::string                       m_strDataBasePath;

        // for dns authorization
        ::std::string                       m_strAuthorization;

        CConfigCertificate                  m_certificateConfig;
    };
}
#endif
