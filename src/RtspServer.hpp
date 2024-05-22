#pragma once

#include <string>

class RtspServer
{
public:
	RtspServer(const std::string& address, const std::string& port);
	void run();
	std::string getPlatformPipeline();

private:
	std::string _address;
	std::string _port;
};
