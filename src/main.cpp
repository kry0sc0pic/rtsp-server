#include "RtspServer.hpp"
#include <iostream>
#include <toml.hpp>

int main()
{

	toml::table config;

	try {
		config = toml::parse_file(std::string(getenv("HOME")) + "/.local/share/rtsp-server/config.toml");

	} catch (const toml::parse_error& err) {
		std::cerr << "Parsing failed:\n" << err << "\n";
		return -1;

	} catch (const std::exception& err) {
		std::cerr << "Error: " << err.what() << "\n";
		return -1;
	}

	std::string path = config["path"].value_or("camera1");
	std::string ip = config["ip_address"].value_or("0.0.0.0");
	int port = config["port"].value_or(5600);

	RtspServer server(path, ip, std::to_string(port));
	server.run();
	return 0;
}
