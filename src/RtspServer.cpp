#include "RtspServer.hpp"
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
#include <iostream>
#include <fstream>

// Original pipeline
// const char* PI_PIPELINE = "( libcamerasrc ! videoconvert ! x264enc key-int-max=30 bitrate=2000 tune=zerolatency speed-preset=ultrafast ! \
// 							video/x-h264,stream-format=byte-stream ! rtph264pay config-interval=1 name=pay0 pt=96 )";

// const char* PI_PIPELINE = "( libcamerasrc ! video/x-raw,framerate=30/1,width=1920,height=1080 ! videoscale ! video/x-raw,width=1280,height=720 ! \
// 							videoconvert ! x264enc bitrate=2500 tune=zerolatency speed-preset=ultrafast key-int-max=30 ! \
//                             video/x-h264,stream-format=byte-stream ! h264parse ! rtph264pay config-interval=1 name=pay0 pt=96 )";

const char* PI_PIPELINE = "( libcamerasrc ! video/x-raw,framerate=30/1,width=1280,height=720 ! videoconvert ! \
							v4l2h264enc extra-controls=\"encode,video_bitrate=2500000\" ! video/x-h264,stream-format=byte-stream ! \
							h264parse ! rtph264pay config-interval=1 name=pay0 pt=96 )";

// TODO: figure out if we need to add the plugin path
// gst-launch-1.0 --gst-plugin-path=install/gst_bridge/lib/gst_bridge/ rosimagesrc ros-topic=/camera/color/image_raw \
// ! queue max-size-buffers=1 ! video/x-raw,format=RGB ! videoconvert ! x264enc bitrate=2100 tune=zerolatency speed-preset=ultrafast \
// ! video/x-h264,stream-format=byte-stream ! rtph264pay config-interval=1 pt=96 ! udpsink host=$TARGET_IP port=$TARGET_PORT sync=false
const char* JETSON_PIPELINE = "( rosimagesrc ros-topic=/camera/color/image_raw ! videoconvert ! x264enc bitrate=2000 tune=zerolatency speed-preset=ultrafast ! \
							video/x-h264,stream-format=byte-stream ! rtph264pay config-interval=1 name=pay0 pt=96 )";

const char* UB_PIPELINE = "( videotestsrc pattern=ball ! videoconvert ! x264enc bitrate=2000 tune=zerolatency speed-preset=ultrafast ! \
							video/x-h264,stream-format=byte-stream ! rtph264pay config-interval=1 name=pay0 pt=96 )";

RtspServer::RtspServer(const std::string& address, const std::string& port)
	: _address(address), _port(port)
{}

void RtspServer::run()
{
	GstRTSPServer* server;
	GstRTSPMountPoints* mounts;
	GstRTSPMediaFactory* factory;
	GMainLoop* loop;

	gst_init(nullptr, nullptr);

	server = gst_rtsp_server_new();
	gst_rtsp_server_set_address(server, _address.c_str());
	gst_rtsp_server_set_service(server, _port.c_str());

	mounts = gst_rtsp_server_get_mount_points(server);
	factory = gst_rtsp_media_factory_new();

	// TODO: select platform
	auto pipeline = getPlatformPipeline();
	gst_rtsp_media_factory_set_launch(factory, pipeline.c_str());

	gst_rtsp_mount_points_add_factory(mounts, "/fpv", factory);
	g_object_unref(mounts);

	gst_rtsp_server_attach(server, NULL);

	loop = g_main_loop_new(NULL, FALSE);
	std::cout << "Stream ready at rtsp://" << _address << ":" << _port << "/fpv" << std::endl;
	g_main_loop_run(loop);
}

std::string RtspServer::getPlatformPipeline()
{
	std::ifstream cpuinfo("/proc/cpuinfo");

	if (!cpuinfo.is_open()) {
		return UB_PIPELINE; // Assume Ubuntu Desktop if file can't be opened
	}

	std::string line;

	while (getline(cpuinfo, line)) {
		if (line.find("Raspberry Pi") != std::string::npos) {
			return PI_PIPELINE;
		}

		if (line.find("NVIDIA Jetson") != std::string::npos) {
			return JETSON_PIPELINE; // Define this pipeline similarly to PI_PIPELINE and UB_PIPELINE
		}
	}

	// Default to Ubuntu Desktop pipeline if no specific identifiers are found
	return UB_PIPELINE;
}
