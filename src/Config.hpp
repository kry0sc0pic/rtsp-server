#pragma once

#include <string>
#include <map>
#include <utility>

// Resolution presets
enum class ResolutionPreset {
	R320x240,    // 320x240   (QVGA)
	R640x480,    // 640x480   (VGA)
	R800x600,    // 800x600   (SVGA)
	R1280x720,   // 1280x720  (HD)
	R1280x960,   // 1280x960
	R1920x1080   // 1920x1080 (Full HD)
};

// Camera rotation
enum class CameraRotation {
	ROTATE_0,    // No rotation
	ROTATE_90,   // 90 degrees clockwise
	ROTATE_180,  // 180 degrees
	ROTATE_270   // 270 degrees clockwise (90 counterclockwise)
};

// Mapping of resolution presets to width/height
const std::map<ResolutionPreset, std::pair<int, int>> RESOLUTION_MAP = {
	{ResolutionPreset::R320x240, {320, 240}},
	{ResolutionPreset::R640x480, {640, 480}},
	{ResolutionPreset::R800x600, {800, 600}},
	{ResolutionPreset::R1280x720, {1280, 720}},
	{ResolutionPreset::R1280x960, {1280, 960}},
	{ResolutionPreset::R1920x1080, {1920, 1080}}
};

// Helper functions for enum conversion to/from string
ResolutionPreset stringToResolution(const std::string& resolution);
std::string resolutionToString(ResolutionPreset resolution);

CameraRotation stringToRotation(const std::string& rotation);
std::string rotationToString(CameraRotation rotation);

struct ServerConfig {
	std::string path;
	std::string address;
	std::string port;
};

struct CameraConfig {
	ResolutionPreset resolution;
	int framerate;
	int bitrate;
	CameraRotation rotation;

	// Helper function to get width/height from resolution preset
	std::pair<int, int> getDimensions() const
	{
		return RESOLUTION_MAP.at(resolution);
	}

	int getWidth() const
	{
		return getDimensions().first;
	}

	int getHeight() const
	{
		return getDimensions().second;
	}

	// Get rotation angle in degrees
	int getRotationDegrees() const
	{
		switch (rotation) {
		case CameraRotation::ROTATE_0: return 0;

		case CameraRotation::ROTATE_90: return 90;

		case CameraRotation::ROTATE_180: return 180;

		case CameraRotation::ROTATE_270: return 270;
		}

		return 0;
	}
};

struct AppConfig {
	ServerConfig server;
	CameraConfig camera;
};
