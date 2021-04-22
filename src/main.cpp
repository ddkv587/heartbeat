#include <iostream>
#include <string>
#include <vector>
#include <sys/stat.h>
#include "server.hpp"
#include "session.hpp"

using namespace HeartBeat;

static void showUsage( std::string name )
{
    std::cerr << "Usage: " << name << " <option(s)> SOURCES"
              << "Options:\n"
              << "\t-h,--help\t\tShow this help message\n"
              << "\t-c,--config Config path\tSpecify the config path for load"
              << std::endl;
}

static void signalHandler( int signum ) 
{
    switch( signum ) {
    case SIGINT:
        ::std::cout << "signalHandler SIGINT" << ::std::endl;
        exit(0);
    default:
        break;
    }
}

int main( int argc, char* argv[] ) 
{
    signal(SIGINT, signalHandler);

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ( ( arg == "-h" ) || ( arg == "--help" ) ) {
            showUsage( argv[0] );
            return 0;
        } else if ( ( arg == "-c" ) || ( arg == "--config" ) ) {
            if ( ( i + 1 ) < argc) {
                std::string strConfigPath = argv[++i];
                struct stat buffer;   
                  if ( stat ( strConfigPath.c_str(), &buffer ) != 0 ) {
                    std::cerr << "--config with a non-exist file path!" << std::endl;
                    return -1;
                }
                CConfig::getInstance()->loadConfig( strConfigPath );
            } else { 
                std::cerr << "--config option requires one argument." << std::endl;
                return -1;
            }  
        } else {
            ; // do nothing
        }
    }

	CHeartBeat* hb = new CHeartBeat;
	if ( NULL == hb ) return false;

    hb->registerSession( new CHeartBeatSession );
    hb->initialize();
    hb->run();

	hb->unInitialize();
    return 0;
}
