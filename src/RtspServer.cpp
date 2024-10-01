#include "RtspServer.hpp"
#include <array>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <memory>

// namespace gst c lib for readability
namespace gst
{
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
}

// Original pipeline
const char* PI_PIPELINE = "( libcamerasrc ! videoconvert ! x264enc key-int-max=15 bitrate=2500 tune=zerolatency speed-preset=ultrafast ! \
							video/x-h264,stream-format=byte-stream ! rtph264pay config-interval=1 name=pay0 pt=96 )";

const char* JETSON_PIPELINE = "( nvarguscamerasrc ! nvvidconv ! x264enc key-int-max=15 bitrate=2500 tune=zerolatency speed-preset=ultrafast ! \
							video/x-h264,stream-format=byte-stream ! rtph264pay config-interval=1 name=pay0 pt=96 )";

const char* UB_PIPELINE = "( videotestsrc pattern=ball ! videoconvert ! x264enc bitrate=2000 tune=zerolatency speed-preset=ultrafast ! \
							video/x-h264,stream-format=byte-stream ! rtph264pay config-interval=1 name=pay0 pt=96 )";

RtspServer::RtspServer(const std::string& path, const std::string& address, const std::string& port)
	: _path(path)
	, _address(address)
	, _port(port)
{}

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
		return UB_PIPELINE;

	case Platform::Pi:
		return PI_PIPELINE;

	case Platform::Jetson:
		return JETSON_PIPELINE;
	}

	return UB_PIPELINE;
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
