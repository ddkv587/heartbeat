#include "server.hpp"
#include "session.hpp"

using namespace HeartBeat;

int main() 
{
    CHeartBeat hb;

    hb.registerSession( new CHeartBeatSession );

    hb.initilalize();

    hb.run();

	return 0;
}
