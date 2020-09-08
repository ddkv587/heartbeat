#include <jsoncpp/json/json.h>
#include <fstream>
#include "config.hpp"

namespace HeartBeat
{
    CConfig* CConfig::s_pInstance = NULL;

    bool CConfig::loadConfig( const ::std::string& strConfigPath )
    {
        if ( strConfigPath.empty() )      return false;

        ::std::ifstream ifs;
        ifs.open( strConfigPath );
        if ( !ifs.is_open() )   return false;

        ::Json::Value root;
        ::Json::CharReaderBuilder builder;
        builder["collectComments"] = false;
        ::JSONCPP_STRING errs;
        if ( !parseFromStream( builder, ifs, &root, &errs ) ) {
            std::cout << errs << std::endl;
            ifs.close();
            return false;
        }
        std::cout << "loadConfig: " << std::endl
                  << root << std::endl;
        ifs.close();

        if ( ::Json::Value::null != root["port"] )              m_iPort = ( unsigned short )root["port"].asUInt();
        if ( ::Json::Value::null != root["name"] )              m_strName = root["name"].asString();
        if ( ::Json::Value::null != root["password"] )          m_strPassword = root["password"].asString();
        if ( ::Json::Value::null != root["keychain"] )          m_strKeyChain = root["keychain"].asString();

        if ( ::Json::Value::null != root["database"] )          m_strDataBasePath = root["database"].asString();

        if ( ::Json::Value::null != root["authorization"] )     m_strAuthorization = root["authorization"].asString();

        if ( ::Json::Value::null != root["certificateChain"] )  m_certificateConfig.certificateChainPath( root["certificateChain"].asString() );
        if ( ::Json::Value::null != root["privateKey"] )        m_certificateConfig.privateKeyPath( root["privateKey"].asString() );
        if ( ::Json::Value::null != root["dhparam"] )           m_certificateConfig.dhparamPath( root["dhparam"].asString() );

        return true;
    }

    void CConfig::saveConfig( const ::std::string& strConfigPath )
    {
        if ( strConfigPath.empty() ) return;

        ::std::ofstream ofs;
        ofs.open( strConfigPath );
        if ( !ofs.is_open() ) return;

        ::Json::Value root;
        ::Json::StreamWriterBuilder builder;
        const std::unique_ptr<Json::StreamWriter> writer( builder.newStreamWriter() );

        root["port"] = m_iPort;
        root["name"] = m_strName;
        root["password"] = m_strPassword;
        root["keychain"] = m_strKeyChain;

        root["database"] = m_strDataBasePath;

        root["authorization"] = m_strAuthorization;

        root["certificateChain"] = m_certificateConfig.certificateChainPath();
        root["privateKey"] = m_certificateConfig.privateKeyPath();
        root["dhparam"] = m_certificateConfig.dhparamPath();

        writer->write( root, &ofs );
        ofs.close();
    }

    bool CConfig::checkKeyChain( const ::std::string& strKeyChain )
    {
        return ( m_strKeyChain == strKeyChain );
    }
}