#include "Config.hpp"
#include <unordered_map>
#include <stdexcept>
#include <algorithm>

ResolutionPreset stringToResolution(const std::string& resolution)
{
	static const std::unordered_map<std::string, ResolutionPreset> mapping = {
		{"320x240", ResolutionPreset::R320x240},
		{"640x480", ResolutionPreset::R640x480},
		{"1280x720", ResolutionPreset::R1280x720},
		{"1920x1080", ResolutionPreset::R1920x1080}
	};

	auto it = mapping.find(resolution);

	if (it != mapping.end()) {
		return it->second;
	}

	return ResolutionPreset::R640x480;
}

std::string resolutionToString(ResolutionPreset resolution)
{
	static const std::unordered_map<ResolutionPreset, std::string> mapping = {
		{ResolutionPreset::R320x240, "320x240"},
		{ResolutionPreset::R640x480, "640x480"},
		{ResolutionPreset::R1280x720, "1280x720"},
		{ResolutionPreset::R1920x1080, "1920x1080"}
	};

	auto it = mapping.find(resolution);

	if (it != mapping.end()) {
		return it->second;
	}

	return "unknown";
}

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

	return CameraRotation::ROTATE_0;
}

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
