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

        std::cout   << "on_http remote: " << con->get_remote_endpoint() 
                    << ", host: " 							<< con->get_host() 
                    << ", uri: "    						<< con->get_uri()->str()
                    << ", request header real ip: " 		<< con->get_request_header( "X-Real-IP" )
                    << ", request header forward for: " 	<< con->get_request_header( "X-Forwarded-For" )
                    << ", request header forward proto: " 	<< con->get_request_header( "X-Forwarded-Proto" )
                    << ", request header request cmd: " 	<< con->get_request_header( "X-Request-CMD" )
                    << ", request body: " 					<< con->get_request_body()
                    << ::std::endl;

        if ( !con->get_request_header( "X-Request-CMD" ).empty() ) {
            int cmdType         = ::std::stoi( con->get_request_header( "X-Request-CMD" ), 0, 16 );
            ::std::string strIP = con->get_request_header( "X-Real-IP" );

            if ( MAGIC_CMD_TYPE_UPDATE_DNS == ( cmdType & MAGIC_CMD_TYPE_MASK ) ) {
                // update dns
                ::std::cout << "begin to update dns with ip:" << strIP<< ::std::endl;

                // ::std::string strCmdZoneID      = "curl -s -X GET \"https://api.cloudflare.com/client/v4/zones?name=" + CConfig::getInstance()->zoneName() + "&status=active\" \
                //                                     -H \"Authorization: Bearer " + CConfig::getInstance()->authorization() + "\" \
                //                                     -H \"Content-Type: application/json\" | jq -r '{\"result\"}[] | .[0] | .id'";
                // ::std::string strZoneID         = ::cmd_exec( strCmdZoneID );
                // strZoneID = ::std::regex_replace( strZoneID, std::regex("\n+"), "" );
                ::std::string strZoneID = CCommand::chop( CCommand::getZoneID( CConfig::getInstance()->zoneName(), CConfig::getInstance()->authorization() ) );

                // ::std::string strCmdRecordID    = "curl -s -X GET \"https://api.cloudflare.com/client/v4/zones/" + strZoneID + "/dns_records?type=A&name=" + CConfig::getInstance()->recordName() + "\" \
                //                                     -H \"Authorization: Bearer " + CConfig::getInstance()->authorization() + "\" \
                //                                     -H \"Content-Type: application/json\" | jq -r '{\"result\"}[] | .[0] | .id'";
                // ::std::string strRecordID       = ::cmd_exec( strCmdRecordID );
                // strRecordID = ::std::regex_replace( strZoneID, std::regex("\n+"), "" );
                ::std::string strRecordID = CCommand::chop( CCommand::getRecordID( strZoneID, CConfig::getInstance()->recordName(), CConfig::getInstance()->authorization() ) );

                ::std::cout << "strCmdZoneID: "     << strCmdZoneID     << ::std::endl
                            << "strZoneID: "        << strZoneID        << ::std::endl
                            << "strCmdRecordID: "   << strCmdRecordID   << ::std::endl
                            << "strRecordID: "      << strRecordID      << ::std::endl;
            }
        }
        
        std::stringstream ss;
        auto end = std::chrono::system_clock::now();
        std::time_t end_time = std::chrono::system_clock::to_time_t( end );
        ss  << "<!doctype html><html><head>"
            << "<title>Access 200 (Request Access)</title><body>"
            << "<h1>Access 200</h1>"
            << "<p>Your IP: " << con->get_request_header( "X-Real-IP" ) << ".</p>"
            << "<p>The current time is " << std::ctime( &end_time ) << ".</p>"
            << "</body></head></html>";
        
        con->set_body( ss.str() );
        con->set_status(websocketpp::http::status_code::ok);
    }
}