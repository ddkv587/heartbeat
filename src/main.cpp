#include "server.hpp"
#include "session.hpp"

using namespace HeartBeat;

CHeartBeat hb;

void signalHandler( int signum ) 
{
    switch( signum ) {
    case SIGINT:
        ::std::cout << "signalHandler SIGINT" << ::std::endl;
        exit(0);
    default:
        break;
    }
}

int main() 
{
    signal(SIGINT, signalHandler);

    hb.registerSession( new CHeartBeatSession );
    hb.initilalize();
    hb.run();

	return 0;
}
