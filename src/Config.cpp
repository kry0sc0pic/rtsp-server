#include "Config.hpp"
#include <unordered_map>
#include <stdexcept>

// String to ResolutionPreset mapping
ResolutionPreset stringToResolution(const std::string& resolution)
{
	static const std::unordered_map<std::string, ResolutionPreset> mapping = {
		{"320x240", ResolutionPreset::R320x240},
		{"640x480", ResolutionPreset::R640x480},
		{"800x600", ResolutionPreset::R800x600},
		{"1280x720", ResolutionPreset::R1280x720},
		{"1280x960", ResolutionPreset::R1280x960},
		{"1920x1080", ResolutionPreset::R1920x1080}
	};

	auto it = mapping.find(resolution);

	if (it != mapping.end()) {
		return it->second;
	}

	// Default to 640x480 if not found
	return ResolutionPreset::R640x480;
}

// ResolutionPreset to string mapping
std::string resolutionToString(ResolutionPreset resolution)
{
	static const std::unordered_map<ResolutionPreset, std::string> mapping = {
		{ResolutionPreset::R320x240, "320x240"},
		{ResolutionPreset::R640x480, "640x480"},
		{ResolutionPreset::R800x600, "800x600"},
		{ResolutionPreset::R1280x720, "1280x720"},
		{ResolutionPreset::R1280x960, "1280x960"},
		{ResolutionPreset::R1920x1080, "1920x1080"}
	};

	auto it = mapping.find(resolution);

	if (it != mapping.end()) {
		return it->second;
	}

	return "unknown";
}

// String to CameraRotation mapping
CameraRotation stringToRotation(const std::string& rotation)
{
	static const std::unordered_map<std::string, CameraRotation> mapping = {
		{"0", CameraRotation::ROTATE_0},
		{"90", CameraRotation::ROTATE_90},
		{"180", CameraRotation::ROTATE_180},
		{"270", CameraRotation::ROTATE_270}
	};

	auto it = mapping.find(rotation);

	if (it != mapping.end()) {
		return it->second;
	}

	// Default to no rotation if not found
	return CameraRotation::ROTATE_0;
}

// CameraRotation to string mapping
std::string rotationToString(CameraRotation rotation)
{
	static const std::unordered_map<CameraRotation, std::string> mapping = {
		{CameraRotation::ROTATE_0, "0"},
		{CameraRotation::ROTATE_90, "90"},
		{CameraRotation::ROTATE_180, "180"},
		{CameraRotation::ROTATE_270, "270"}
	};

	auto it = mapping.find(rotation);

	if (it != mapping.end()) {
		return it->second;
	}

	return "0";
}
