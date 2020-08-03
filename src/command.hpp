#ifndef DEF__COMMAND_HPP__
#define DEF__COMMAND_HPP__

#include <string.h>
#include <regex>
#include <iostream>

namespace HeartBeat
{
    class CCommand
    {   
    public:
        static ::std::string chop( const ::std::string& str )
        {
            return ::std::regex_replace( str, std::regex("\n+"), "" );
        }

    private:
        static ::std::string execute( const ::std::string& strCMD )
        {
            std::array<char, 128> buffer;
            std::string result; 
            std::unique_ptr<FILE, decltype(&pclose)> pipe( popen( strCMD.c_str(), "r" ), pclose );
            if ( !pipe ) {
                throw std::runtime_error("popen() failed!");
            }
            while ( fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr ) {
                result += buffer.data();
            }
            
            ::std::cout << "execute cmd: "  << strCMD << ::std::endl
                        << "result: "       << result << ::std::endl;
            
            return result;
        }

        template<typename ... Args>
        static std::string format( const std::string& format, Args ... args )
        {
            size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
            if ( size <= 0 ) { 
                return ::std::string();
            }
            std::unique_ptr<char[]> buf( new char[ size ] ); 
            snprintf( buf.get(), size, format.c_str(), args ... );
            return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
        }

    private:
        friend class CDNSCommand;
    };

    class CDNSCommand
    {
    public:
        struct tagRecord
        {
            ::std::string   strZoneID;
            ::std::string   strRecordID;
            ::std::string   strDNSType;
            ::std::string   strDomain;
            ::std::string   strIP;        
            unsigned int    uiTTL;
            bool            bProxied;

            tagRecord() 
                : strZoneID()
                , strRecordID()
                , strDNSType( "A" )
                , strDomain( "default" )
                , strIP( "127.0.0.1" )
                , uiTTL( 1 )
                , bProxied( false )
            {
                ;
            }

            bool isNull() const
            {
                return ( strZoneID.empty() || strRecordID.empty() || strDomain.empty() || strIP.empty() );
            }
        };

    private:
        static const ::std::string s_strZoneIDFormat;
        static const ::std::string s_strRecordIDFormat;
        static const ::std::string s_strUpdateDNSFormat;
    
    public:
        static ::std::string getZoneID( const ::std::string& strZoneName, const ::std::string& strAuthorization )
        {
            if ( strZoneName.empty() || strAuthorization.empty() ) return ::std::string();

            ::std::string strCmd = CCommand::format( s_strZoneIDFormat, strZoneName.c_str(), strAuthorization.c_str() );

            if ( strCmd.empty() ) return ::std::string();

            return CCommand::execute( strCmd );
        }

        static ::std::string getRecordID( const ::std::string& strZoneID, const ::std::string& strRecordName, const ::std::string& strAuthorization )
        {
            if ( strZoneID.empty() || strRecordName.empty() || strAuthorization.empty() )  return ::std::string();

            ::std::string strCmd = CCommand::format( s_strRecordIDFormat, strZoneID.c_str(), strRecordName.c_str(), strAuthorization.c_str() );

            if ( strCmd.empty() ) return ::std::string();

            return CCommand::execute( strCmd );
        }

        static bool updateDNSRecord( const tagRecord& record, const ::std::string& strAuthorization )
        {
            if ( record.isNull() || strAuthorization.empty() )  return false;

            ::std::string strCmd = CCommand::format( s_strUpdateDNSFormat, \
                                                record.strZoneID.c_str(), record.strRecordID.c_str(), strAuthorization.c_str(), \
                                                record.strDNSType.c_str(), record.strDomain.c_str(), record.strIP.c_str(), record.uiTTL, record.bProxied ? "true" : "false" );

            if ( strCmd.empty() ) return false;

            ::std::string strResult = CCommand::execute( strCmd );

            return ( ::std::string::npos != strResult.find( "\"success\":true" ) );
        }
    };
}
#endif