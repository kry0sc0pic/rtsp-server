#include "RtspServer.hpp"

int main()
{
	RtspServer server("0.0.0.0", "8554");
	server.run();
	return 0;
}
