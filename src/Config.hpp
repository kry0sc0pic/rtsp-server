#pragma once

#include <string>

struct ServerConfig {
	std::string path;
	std::string address;
	std::string port;
};

struct CameraConfig {
	int width;
	int height;
	int framerate;
	int bitrate;
};

struct AppConfig {
	ServerConfig server;
	CameraConfig camera;
};
