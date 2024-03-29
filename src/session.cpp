#include <chrono>
#include <ctime>

#include "config.hpp"
#include "command.hpp"
#include "session.hpp"

const static int MAGIC_CMD_TYPE_MASK        = (size_t)0xFFFFFFFF;
const static int MAGIC_CMD_TYPE_UPDATE_DNS  = 0x10100101;

::std::string cmd_exec( const ::std::string& strCMD )
{
    std::array<char, 128> buffer;
    std::string result; 
    std::unique_ptr<FILE, decltype(&pclose)> pipe( popen( strCMD.c_str(), "r" ), pclose );
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

namespace HeartBeat
{
    void CHeartBeatSession::onHttp( _server* sever, websocketpp::connection_hdl hdl ) 
    {
        std::cout << "on_http called with hdl: " << hdl.lock().get() << ::std::endl;
        _server::connection_ptr con = sever->get_con_from_hdl( hdl );

        std::cout << "on_http remote: "                   << con->get_remote_endpoint() 
                    << ", host: " 							<< con->get_host() 
                    << ", uri: "    						<< con->get_uri()->str()
                    << ", request header real ip: " 		<< con->get_request_header( "X-Real-IP" )
                    << ", request header forward for: " 	<< con->get_request_header( "X-Forwarded-For" )
                    << ", request header forward proto: " 	<< con->get_request_header( "X-Forwarded-Proto" )
                    << ", request header request cmd: " 	<< con->get_request_header( "X-Request-CMD" )
                    << ", request body: " 					<< con->get_request_body()
                    << ::std::endl;

        EErrorType status = ET_NO_ERROR;
        if ( !con->get_request_header( "X-Request-CMD" ).empty() ) {
            int cmdType         = ::std::stoi( con->get_request_header( "X-Request-CMD" ), 0, 16 );

            if ( MAGIC_CMD_TYPE_UPDATE_DNS == ( cmdType & MAGIC_CMD_TYPE_MASK ) ) {
                // request
                ::std::string strZoneName   = con->get_request_header( "X-Request-DNS-ZoneName" );
                ::std::string strRecordName = con->get_request_header( "X-Request-DNS-RecordName" );
                ::std::string strKeyChain   = con->get_request_header( "X-Request-DNS-KeyChain" );
                
                // optional
                ::std::string strType       = con->get_request_header( "X-Request-DNS-Type" );
                ::std::string strDomain     = con->get_request_header( "X-Request-DNS-Domain" );
                ::std::string strIP         = con->get_request_header( "X-Request-DNS-IP" );
                ::std::string strTTL        = con->get_request_header( "X-Request-DNS-TTL" );
                ::std::string strProxy      = con->get_request_header( "X-Request-DNS-Proxy" );

                if ( !CConfig::getInstance()->checkKeyChain( strKeyChain ) ) {
                    // key error
                    status = ET_KEY_NOT_MATCH;
                } else if ( strZoneName.empty() || strRecordName.empty() ) {
                    ::std::cout << "update dns error with empty "
                                << " zone name "        << strZoneName          <<  ::std::endl
                                << " Record name "      << strRecordName        <<  ::std::endl;

                    status = ET_CONTENT_ERROR;
                } else {                        
                    if ( strDomain.empty() )    strDomain = strRecordName;
                    assert( !strDomain.empty() );

                    if ( strIP.empty() )        strIP = con->get_request_header( "X-Real-IP" );
                    assert( !strIP.empty() );

                    // check ip update
                    if ( m_pRecordDB && m_pRecordDB->checkUpdate( strZoneName, strRecordName, strIP ) ) {
                        CDNSCommand::tagRecord  record;

                        record.strDomain    = strDomain;
                        record.strIP        = strIP;
                        if ( !strType.empty() )         record.strDNSType   = strType;
                        if ( !strTTL.empty() )          record.uiTTL        = ::std::atoi( strTTL.c_str() );
                        if ( !strProxy.empty() )        record.bProxied     = ( ( strProxy == "true" ) || ( strProxy == "TRUE" ) );

                        ::std::cout << "begin to update dns " << record.strDomain << " with ip: " << record.strIP << ::std::endl;
                        record.strZoneID    = CCommand::chop( CDNSCommand::getZoneID( strZoneName, CConfig::getInstance()->authorization() ) );
                        record.strRecordID  = CCommand::chop( CDNSCommand::getRecordID( record.strZoneID, strRecordName, CConfig::getInstance()->authorization() ) );
                
                        if ( !CDNSCommand::updateDNSRecord( record, CConfig::getInstance()->authorization() ) ) {
                            ::std::cout << "update dns error with updateDNSRecord" << ::std::endl;
                            status = ET_DNS_UPDATE_ERROR;
                        }
                    } else {
                        ::std::cout << "skip this update with " << strDomain << "( " << strIP << " )" << ::std::endl;
                    }
                }
            } else {
                // other cmd
                status = ET_CMD_NOT_SUPPORT;
            }
        } else {
            // no cmd
            status = ET_REQUEST_ERROR;
        }
        
        std::stringstream ss;
        auto end = std::chrono::system_clock::now();
        std::time_t end_time = std::chrono::system_clock::to_time_t( end );
        ss  << "<!doctype html><html><head>"
            << "<title>Access 200 (Request Access)</title><body>"
            << "<h1>Access 200</h1>"
            << "<p>Your IP: " << con->get_request_header( "X-Real-IP" ) << ".</p>"
            << "<p>The current time is " << std::ctime( &end_time ) << status << ".</p>"
            << "</body></head></html>";
        
        con->set_body( ss.str() );
        con->set_status(websocketpp::http::status_code::ok);
    }
}