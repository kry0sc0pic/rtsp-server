#include "RtspServer.hpp"
#include <array>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>

// namespace gst c lib for readability
namespace gst
{
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
}

RtspServer::RtspServer(const ServerConfig& serverConfig, const CameraConfig& cameraConfig)
	: _path(serverConfig.path)
	, _address(serverConfig.address)
	, _port(serverConfig.port)
	, _cameraConfig(cameraConfig)
{
	std::cout << "Camera config: "
		  << _cameraConfig.getWidth() << "x" << _cameraConfig.getHeight()
		  << " @ " << _cameraConfig.framerate << "fps, bitrate: "
		  << _cameraConfig.bitrate << "kbps, rotation: "
		  << _cameraConfig.getRotationDegrees() << "°" << std::endl;
}

void RtspServer::run()
{
	gst::GstRTSPServer* server;
	gst::GstRTSPMountPoints* mounts;
	gst::GstRTSPMediaFactory* factory;
	gst::GMainLoop* loop;
	std::string pipeline;

	gst::gst_init(nullptr, nullptr);

	server = gst::gst_rtsp_server_new();
	gst::gst_rtsp_server_set_address(server, _address.c_str());
	gst::gst_rtsp_server_set_service(server, _port.c_str());

	mounts = gst::gst_rtsp_server_get_mount_points(server);
	factory = gst::gst_rtsp_media_factory_new();

	// Build pipeline string
	pipeline = get_pipeline(detect_platform());

	gst::gst_rtsp_media_factory_set_launch(factory, pipeline.c_str());

	gst::gst_rtsp_mount_points_add_factory(mounts, std::string("/" + _path).c_str(), factory);
	gst::g_object_unref(mounts);

	// TODO: we need to handle bad disconnect events gracefully (client loses network connection and doesn't end session)
	gst::gst_rtsp_server_attach(server, NULL);

	std::cout << "Stream ready at rtsp://" << _address << ":" << _port << "/" << _path << std::endl << std::endl;
	gst::g_main_loop_run(gst::g_main_loop_new(NULL, FALSE));
}

std::string RtspServer::get_pipeline(Platform platform)
{
	switch (platform) {
	case Platform::Ubuntu:
		return create_ubuntu_pipeline();

	case Platform::Pi:
		return create_pi_pipeline();

	case Platform::Jetson:
		return create_jetson_pipeline();
	}

	return create_ubuntu_pipeline();
}

std::string RtspServer::create_jetson_pipeline()
{
	std::stringstream ss;

	// Original dimensions
	int width = _cameraConfig.getWidth();
	int height = _cameraConfig.getHeight();
	int framerate = _cameraConfig.framerate;

	// Select appropriate sensor mode based on resolution and framerate
	int sensorMode = 0;

	// Match to supported modes from error log
	if (width == 3280 && height == 2464) {
		sensorMode = 0;
		framerate = std::min(framerate, 21);  // Max 21fps for this mode

	} else if (width == 3280 && height == 1848) {
		sensorMode = 1;
		framerate = std::min(framerate, 28);  // Max 28fps for this mode

	} else if (width == 1920 && height == 1080) {
		sensorMode = 2;
		framerate = std::min(framerate, 30);  // Max 30fps for this mode

	} else if (width == 1640 && height == 1232) {
		sensorMode = 3;
		framerate = std::min(framerate, 30);  // Max 30fps for this mode

	} else if (width == 1280 && height == 720) {
		sensorMode = 4;
		framerate = std::min(framerate, 60);  // Max 60fps for this mode
	}

	// Start building the pipeline
	ss << "( nvarguscamerasrc "
	   << "sensor-id=0 "
	   << "sensor-mode=" << sensorMode << " ! "
	   << "video/x-raw(memory:NVMM),width=" << width
	   << ",height=" << height
	   << ",framerate=" << framerate << "/1 ! ";

	// For 90/270 rotations, we need a more complex transformation
	if (_cameraConfig.rotation == CameraRotation::ROTATE_90 ||
	    _cameraConfig.rotation == CameraRotation::ROTATE_270) {

		int flipMethod = (_cameraConfig.rotation == CameraRotation::ROTATE_90) ? 1 : 3;

		// Apply the rotation
		ss << "nvvidconv flip-method=" << flipMethod << " ! ";

		// Maintain the landscape orientation by fitting the rotated image
		// into the original frame size with black bars (letterboxing)
		int scaleWidth = height;
		int scaleHeight = width;

		// Calculate scaling factor to fit in original frame
		float scaleFactor = std::min(
					    static_cast<float>(width) / scaleWidth,
					    static_cast<float>(height) / scaleHeight
				    );

		int finalWidth = static_cast<int>(scaleWidth * scaleFactor);
		int finalHeight = static_cast<int>(scaleHeight * scaleFactor);

		// Add a videocrop element to trim to desired aspect ratio
		ss << "video/x-raw ! "
		   << "videocrop top=" << (height - finalHeight) / 2
		   << " bottom=" << (height - finalHeight) / 2
		   << " left=" << (width - finalWidth) / 2
		   << " right=" << (width - finalWidth) / 2 << " ! "
		   << "video/x-raw,width=" << width << ",height=" << height << ",format=I420 ! ";

	} else if (_cameraConfig.rotation == CameraRotation::ROTATE_180) {
		// 180 degree rotation
		ss << "nvvidconv flip-method=2 ! "
		   << "video/x-raw,format=I420 ! ";

	} else {
		// No rotation
		ss << "nvvidconv ! "
		   << "video/x-raw,format=I420 ! ";
	}

	// Complete the pipeline
	ss << "x264enc key-int-max=30 bitrate=" << _cameraConfig.bitrate
	   << " tune=zerolatency speed-preset=ultrafast ! "
	   << "video/x-h264,stream-format=byte-stream,profile=baseline ! "
	   << "rtph264pay config-interval=1 mtu=1400 name=pay0 pt=96 )";

	std::cout << "Using pipeline: " << ss.str() << std::endl;
	return ss.str();
}

std::string RtspServer::create_pi_pipeline()
{
	std::stringstream ss;
	ss << "( libcamerasrc ";

	// On Pi, libcamerasrc supports rotation parameter directly
	if (_cameraConfig.getRotationDegrees() != 0) {
		ss << "rotation=" << _cameraConfig.getRotationDegrees() << " ";
	}

	ss << "! "
	   << "video/x-raw,width=" << _cameraConfig.getWidth()
	   << ",height=" << _cameraConfig.getHeight()
	   << ",framerate=" << _cameraConfig.framerate << "/1 ! "
	   << "videoconvert ! ";

	// For safety, add videoflip element as a fallback if the built-in rotation fails
	// or if the libcamerasrc version doesn't support the rotation parameter
	if (_cameraConfig.getRotationDegrees() != 0) {
		bool needsVideoflip = true;
		std::string videoflipMethod;

		switch (_cameraConfig.rotation) {
		case CameraRotation::ROTATE_90:
			videoflipMethod = "clockwise";
			break;

		case CameraRotation::ROTATE_180:
			videoflipMethod = "rotate-180";
			break;

		case CameraRotation::ROTATE_270:
			videoflipMethod = "counterclockwise";
			break;

		default:
			needsVideoflip = false;
			break;
		}

		if (needsVideoflip) {
			ss << "videoflip method=" << videoflipMethod << " ! ";
		}
	}

	// Add format specification for consistent behavior
	ss << "video/x-raw,format=I420 ! "
	   << "x264enc key-int-max=30 bitrate=" << _cameraConfig.bitrate
	   << " tune=zerolatency speed-preset=ultrafast ! "
	   << "video/x-h264,stream-format=byte-stream,profile=baseline ! "
	   << "rtph264pay config-interval=1 mtu=1400 name=pay0 pt=96 )";

	std::cout << "Using pipeline: " << ss.str() << std::endl;
	return ss.str();
}

std::string RtspServer::create_ubuntu_pipeline()
{
	std::stringstream ss;
	ss << "( videotestsrc pattern=ball ! "
	   << "video/x-raw,width=" << _cameraConfig.getWidth()
	   << ",height=" << _cameraConfig.getHeight()
	   << ",framerate=" << _cameraConfig.framerate << "/1 ! "
	   << "videoconvert ! ";

	// Add videoflip element for rotation
	if (_cameraConfig.getRotationDegrees() != 0) {
		std::string videoflipMethod;

		switch (_cameraConfig.rotation) {
		case CameraRotation::ROTATE_90:
			videoflipMethod = "clockwise";
			break;

		case CameraRotation::ROTATE_180:
			videoflipMethod = "rotate-180";
			break;

		case CameraRotation::ROTATE_270:
			videoflipMethod = "counterclockwise";
			break;

		default:
			videoflipMethod = "none";
			break;
		}

		if (videoflipMethod != "none") {
			ss << "videoflip method=" << videoflipMethod << " ! ";
		}
	}

	// Add format specification for consistent behavior
	ss << "video/x-raw,format=I420 ! "
	   << "x264enc bitrate=" << _cameraConfig.bitrate
	   << " tune=zerolatency speed-preset=ultrafast ! "
	   << "video/x-h264,stream-format=byte-stream,profile=baseline ! "
	   << "rtph264pay config-interval=1 mtu=1400 name=pay0 pt=96 )";

	std::cout << "Using pipeline: " << ss.str() << std::endl;
	return ss.str();
}

RtspServer::Platform RtspServer::detect_platform()
{
	std::array<char, 128> buffer;
	std::string result;
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("uname -a", "r"), pclose);

	if (!pipe) {
		return Platform::Ubuntu; // Assume Ubuntu Desktop if command can't be executed
	}

	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		result += buffer.data();
	}

	std::cout << "result: " << result << std::endl;

	if (result.find("tegra") != std::string::npos) {
		std::cout << "Platform: Jetson" << std::endl;
		return Platform::Jetson;

	} else if (result.find("rpi") != std::string::npos) {
		std::cout << "Platform: Raspberry Pi" << std::endl;
		return Platform::Pi;
	}

	std::cout << "Platform: Ubuntu Desktop" << std::endl;
	return Platform::Ubuntu;
}
