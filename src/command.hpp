#ifndef DEF__COMMAND_HPP__
#define DEF__COMMAND_HPP__

#include <string.h>
#include <format>
#include <regex>

namespace HeartBeat
{
    class CCommand
    {
    private:
        const static ::std::string s_strZoneIDFormat = "curl -s -X GET \"https://api.cloudflare.com/client/v4/zones?name=%s&status=active\" \
                                                        -H \"Authorization: Bearer %s\" \
                                                        -H \"Content-Type: application/json\" | jq -r '{\"result\"}[] | .[0] | .id'";
        
        const static ::std::string s_strRecordIDFormat = "curl -s -X GET \"https://api.cloudflare.com/client/v4/zones/%s/dns_records?type=A&name=%s\" \
                                                            -H \"Authorization: Bearer %s\" \
                                                            -H \"Content-Type: application/json\" | jq -r '{\"result\"}[] | .[0] | .id'";
    public:
        static ::std::string getZoneID( const ::std::string& strZoneName, const ::std::string& strAuthorization )
        {
            if ( strZoneName.empty() || strAuthorization.empty() ) return ::std::string();

            ::std::string strCmd = format( s_strZoneIDFormat, strZoneName, strAuthorization );

            if ( strCmd.empty() ) return ::std::string();

            return execute( strCmd );
        }

        static ::std::string getRecordID( const ::std::string& strZoneID, const ::std::string& strRecordName, const ::std::string& strAuthorization )
        {
            if ( strZoneID.empty() || strRecordName.empty() || strAuthorization.empty() )  return ::std::string();

            ::std::string strCmd = format( s_strRecordIDFormat, strZoneID, strRecordName, strAuthorization );

            if ( strCmd.empty() ) return ::std::string();
            
            return execute( strCmd );
        }

        static ::std::string chop(const ::std::string& str )
        {
            ::std::regex_replace( str, std::regex("\n+"), "" );
        }

    private:
        static ::std::string execute( const ::std::string& strCMD )
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

        template<typename ... Args>
        std::string format( const std::string& format, Args ... args )
        {
            size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
            if ( size <= 0 ) { 
                return ::std::string();
            }
            std::unique_ptr<char*> buf( new char[ size ] ); 
            snprintf( buf.get(), size, format.c_str(), args ... );
            return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
        }
    };
}
#endif