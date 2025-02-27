#include "RtspServer.hpp"
#include "Config.hpp"
#include <iostream>
#include <toml.hpp>

int main()
{
	// Initialize default configuration
	AppConfig config = {
		.server = {
			.path = "camera1",
			.address = "0.0.0.0",
			.port = "5600"
		},
		.camera = {
			.width = 640,
			.height = 480,
			.framerate = 15,
			.bitrate = 700
		}
	};

	try {
		// Parse config file
		toml::table tomlConfig = toml::parse_file(std::string(getenv("HOME")) + "/.local/share/rtsp-server/config.toml");

		// Server config
		if (tomlConfig.contains("path")) {
			config.server.path = tomlConfig["path"].value_or("camera1");
		}

		if (tomlConfig.contains("ip_address")) {
			config.server.address = tomlConfig["ip_address"].value_or("0.0.0.0");
		}

		if (tomlConfig.contains("port")) {
			int port = tomlConfig["port"].value_or(5600);
			config.server.port = std::to_string(port);
		}

		// Camera config
		if (toml::table* camera = tomlConfig["camera"].as_table()) {
			if (camera->contains("width")) {
				config.camera.width = (*camera)["width"].value_or(640);
			}

			if (camera->contains("height")) {
				config.camera.height = (*camera)["height"].value_or(480);
			}

			if (camera->contains("framerate")) {
				config.camera.framerate = (*camera)["framerate"].value_or(15);
			}

			if (camera->contains("bitrate")) {
				config.camera.bitrate = (*camera)["bitrate"].value_or(700);
			}
		}

	} catch (const toml::parse_error& err) {
		std::cerr << "Parsing failed:\n" << err << "\n";
		std::cerr << "Using default configuration." << std::endl;

	} catch (const std::exception& err) {
		std::cerr << "Error: " << err.what() << "\n";
		std::cerr << "Using default configuration." << std::endl;
	}

	// Create and run the RTSP server with the loaded configuration
	RtspServer server(config.server, config.camera);
	server.run();

	return 0;
}
