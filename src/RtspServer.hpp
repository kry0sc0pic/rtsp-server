#pragma once

#include <string>

class RtspServer
{
public:
	RtspServer(const std::string& address, const std::string& port);
	void run();

private:
	enum class Platform {
		Ubuntu,
		Pi,
		Jetson,
	};

	Platform detect_platform();
	std::string get_pipeline(Platform platform);

	std::string _address;
	std::string _port;
};
